#include "graph_generator.hpp"
#include <algorithm>
#include <atomic>
#include <list>
#include <optional>
#include <random>
#include <thread>

namespace {

using uni_course_cpp::Graph;

static constexpr float kGreenEdgeProbability = 0.1f;
static constexpr float kRedEdgeProbability = 1.0f / 3.0f;
static constexpr uni_course_cpp::GraphDepth kDefaultDepth = 1;
static constexpr uni_course_cpp::GraphDepth kYellowDepthStep = 1;
static constexpr uni_course_cpp::GraphDepth kRedDepthStep = 2;
static const int kMaxThreadsCount = std::thread::hardware_concurrency();

bool check_probability(float probability) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution distribution(probability);
  return distribution(gen);
}

int random_number_in_range(int size) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution distribution(0, size - 1);
  return distribution(gen);
}

uni_course_cpp::VertexId get_random_vertex_id(
    const std::vector<uni_course_cpp::VertexId>& pickable_vertex_ids) {
  const auto random_number = random_number_in_range(pickable_vertex_ids.size());
  return pickable_vertex_ids[random_number];
}

std::vector<uni_course_cpp::VertexId> get_unconnected_vertex_ids(
    const Graph& graph,
    uni_course_cpp::VertexId from_vertex_id,
    uni_course_cpp::GraphDepth depth,
    std::mutex& colored_edges_mutex) {
  const auto& next_depth_vertex_ids =
      graph.get_depth_vertex_ids(depth + kYellowDepthStep);
  std::vector<uni_course_cpp::VertexId> pickable_vertex_ids;
  for (const auto to_vertex_id : next_depth_vertex_ids) {
    const auto are_connected = [&graph, from_vertex_id, to_vertex_id,
                                &colored_edges_mutex]() {
      const std::lock_guard<std::mutex> lock(colored_edges_mutex);
      return graph.are_connected(from_vertex_id, to_vertex_id);
    }();

    if (!are_connected) {
      pickable_vertex_ids.emplace_back(to_vertex_id);
    }
  }
  return pickable_vertex_ids;
}

}  // namespace

namespace uni_course_cpp {

void GraphGenerator::generate_grey_branch(Graph& graph,
                                          VertexId from_vertex_id,
                                          GraphDepth depth,
                                          std::mutex& grey_edges_mutex) const {
  if (depth >= params_.depth())
    return;
  const float depth_probability =
      (params_.depth() - depth) /
      static_cast<float>((params_.depth() - kDefaultDepth));

  if (!check_probability(depth_probability))
    return;

  const auto new_id = [&graph, &grey_edges_mutex, from_vertex_id]() {
    const std::lock_guard<std::mutex> lock(grey_edges_mutex);
    const auto to_vertex_id = graph.add_vertex();
    graph.add_edge(from_vertex_id, to_vertex_id);
    return to_vertex_id;
  }();

  for (int i = 0; i < params_.new_vertices_count(); ++i) {
    generate_grey_branch(graph, new_id, depth + 1, grey_edges_mutex);
  }
}

void GraphGenerator::generate_grey_edges(Graph& graph) const {
  const VertexId root_id = graph.add_vertex();

  std::atomic<int> active_jobs_counter = params_.new_vertices_count();
  std::mutex grey_edges_mutex, jobs_mutex;
  using JobCallback = std::function<void()>;
  auto jobs = std::list<JobCallback>();

  for (int i = 0; i < params_.new_vertices_count(); ++i) {
    jobs.push_back(
        [&graph, &grey_edges_mutex, &active_jobs_counter, root_id, this]() {
          generate_grey_branch(graph, root_id, kDefaultDepth, grey_edges_mutex);
          --active_jobs_counter;
        });
  }

  std::atomic<bool> should_terminate = false;
  const auto worker = [&should_terminate, &jobs_mutex, &jobs]() {
    while (true) {
      if (should_terminate) {
        return;
      }

      const auto job_optional = [&jobs_mutex,
                                 &jobs]() -> std::optional<JobCallback> {
        const std::lock_guard<std::mutex> lock(jobs_mutex);
        if (jobs.size()) {
          const auto job = *jobs.begin();
          jobs.erase(jobs.begin());
          return job;
        }
        return std::nullopt;
      }();

      if (job_optional) {
        const auto& job = *job_optional;
        job();
      }
    }
  };

  const auto threads_count =
      std::min(kMaxThreadsCount, params_.new_vertices_count());

  auto threads = std::vector<std::thread>();
  threads.reserve(threads_count);
  for (int i = 0; i < threads_count; ++i) {
    threads.emplace_back(worker);
  }

  while (active_jobs_counter) {
  }

  should_terminate = true;
  for (auto& thread : threads) {
    thread.join();
  }
}

void GraphGenerator::generate_green_edges(
    Graph& graph,
    std::mutex& colored_edges_mutex) const {
  graph.for_each_vertex([&graph, &colored_edges_mutex](const IVertex& vertex) {
    if (check_probability(kGreenEdgeProbability)) {
      const std::lock_guard<std::mutex> lock(colored_edges_mutex);
      graph.add_edge(vertex.id(), vertex.id());
    }
  });
}

void GraphGenerator::generate_yellow_edges(
    Graph& graph,
    std::mutex& colored_edges_mutex) const {
  for (GraphDepth depth = kDefaultDepth;
       depth <= graph.depth() - kYellowDepthStep; ++depth) {
    const float depth_probability =
        (depth - kDefaultDepth) /
        static_cast<float>((graph.depth() - kYellowDepthStep - kDefaultDepth));
    const auto& current_depth_vertex_ids = graph.get_depth_vertex_ids(depth);
    std::for_each(
        current_depth_vertex_ids.cbegin(), current_depth_vertex_ids.cend(),
        [&graph, &colored_edges_mutex, depth_probability,
         depth](auto from_vertex_id) {
          if (check_probability(depth_probability)) {
            std::vector<VertexId> pickable_vertex_ids =
                get_unconnected_vertex_ids(graph, from_vertex_id, depth,
                                           colored_edges_mutex);
            if (!pickable_vertex_ids.empty()) {
              const std::lock_guard<std::mutex> lock(colored_edges_mutex);
              graph.add_edge(from_vertex_id,
                             get_random_vertex_id(pickable_vertex_ids));
            }
          }
        });
  }
}

void GraphGenerator::generate_red_edges(Graph& graph,
                                        std::mutex& colored_edges_mutex) const {
  for (GraphDepth depth = kDefaultDepth; depth <= graph.depth() - kRedDepthStep;
       ++depth) {
    const auto& next_depth_vertex_ids =
        graph.get_depth_vertex_ids(depth + kRedDepthStep);
    const auto& current_depth_vertex_ids = graph.get_depth_vertex_ids(depth);
    std::for_each(
        current_depth_vertex_ids.cbegin(), current_depth_vertex_ids.cend(),
        [&graph, &colored_edges_mutex,
         &next_depth_vertex_ids](auto from_vertex_id) {
          if (check_probability(kRedEdgeProbability)) {
            const auto random_number =
                random_number_in_range(next_depth_vertex_ids.size());
            const std::lock_guard<std::mutex> lock(colored_edges_mutex);
            graph.add_edge(
                from_vertex_id,
                *(std::next(next_depth_vertex_ids.begin(), random_number)));
          }
        });
  }
}

std::unique_ptr<IGraph> GraphGenerator::generate() const {
  auto graph = Graph();
  if (params_.depth() == 0)
    return std::make_unique<Graph>(std::move(graph));

  std::mutex colored_edges_mutex;
  generate_grey_edges(graph);

  std::thread green_thread([&graph, &colored_edges_mutex, this]() {
    generate_green_edges(graph, colored_edges_mutex);
  });
  std::thread yellow_thread([&graph, &colored_edges_mutex, this]() {
    generate_yellow_edges(graph, colored_edges_mutex);
  });
  std::thread red_thread([&graph, &colored_edges_mutex, this]() {
    generate_red_edges(graph, colored_edges_mutex);
  });

  green_thread.join();
  yellow_thread.join();
  red_thread.join();

  return std::make_unique<Graph>(std::move(graph));
}
}  // namespace uni_course_cpp

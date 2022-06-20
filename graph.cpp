#include "graph.hpp"
#include <stdexcept>
namespace uni_course_cpp {

static constexpr GraphDepth kDefaultDepth = 1;

EdgeColor Graph::calculate_edge_color(VertexId from_vertex_id,
                                      VertexId to_vertex_id) const {
  const auto from_vertex_depth = get_vertex_depth(from_vertex_id);
  const auto to_vertex_depth = get_vertex_depth(to_vertex_id);
  if (from_vertex_id == to_vertex_id) {
    return EdgeColor::Green;
  }
  if (get_connected_edge_ids(to_vertex_id).size() == 0) {
    return EdgeColor::Grey;
  }
  if (to_vertex_depth - from_vertex_depth == 1 &&
      !are_connected(from_vertex_id, to_vertex_id)) {
    return EdgeColor::Yellow;
  }
  if (to_vertex_depth - from_vertex_depth == 2) {
    return EdgeColor::Red;
  }
  throw std::runtime_error("Failed to determine color");
}

void Graph::for_each_vertex(
    const std::function<void(const IVertex& vertex)>& handler) const {
  std::for_each(vertices_.cbegin(), vertices_.cend(),
                [&handler](const auto& element) { handler(element); });
}

void Graph::for_each_edge(
    const std::function<void(const IEdge& edge)>& handler) const {
  std::for_each(edges_.cbegin(), edges_.cend(),
                [&handler](const auto& element) { handler(element); });
}

bool Graph::are_connected(VertexId from_vertex_id,
                          VertexId to_vertex_id) const {
  if (from_vertex_id == to_vertex_id) {
    for (const auto edge_id : get_connected_edge_ids(from_vertex_id)) {
      if (edges_[edge_id].color() == EdgeColor::Green)
        return true;
    }
    return false;
  } else {
    const auto& from_vertex_edge_ids = get_connected_edge_ids(from_vertex_id);
    const auto& to_vertex_edge_ids = get_connected_edge_ids(to_vertex_id);

    for (const auto edge_id : from_vertex_edge_ids) {
      if (to_vertex_edge_ids.find(edge_id) != to_vertex_edge_ids.cend())
        return true;
    }
    return false;
  }
}

void Graph::set_vertex_depth(VertexId id, GraphDepth depth) {
  depth_vertex_ids_[depth].emplace(id);
  depth_vertex_ids_[kDefaultDepth].erase(id);
  vertex_depths_[id] = depth;
}

VertexId Graph::add_vertex() {
  const VertexId new_vertex_id = get_new_vertex_id();
  depth_vertex_ids_[kDefaultDepth].emplace(new_vertex_id);
  vertex_depths_[new_vertex_id] = kDefaultDepth;
  vertices_.emplace_back(new_vertex_id);
  adjacency_list_.emplace(new_vertex_id, std::unordered_set<EdgeId>());
  return new_vertex_id;
}

EdgeId Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  const EdgeId new_edge_id = get_new_edge_id();
  const auto color = calculate_edge_color(from_vertex_id, to_vertex_id);
  edges_.emplace_back(new_edge_id, from_vertex_id, to_vertex_id, color);
  if (color == EdgeColor::Grey) {
    set_vertex_depth(to_vertex_id, get_vertex_depth(from_vertex_id) + 1);
  }
  adjacency_list_[from_vertex_id].emplace(new_edge_id);
  if (from_vertex_id != to_vertex_id) {
    adjacency_list_[to_vertex_id].emplace(new_edge_id);
  }

  return new_edge_id;
}

}  // namespace uni_course_cpp

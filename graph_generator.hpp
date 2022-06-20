#pragma once
#include <memory>
#include <mutex>
#include "graph.hpp"
#include "interfaces/i_graph.hpp"

namespace uni_course_cpp {
class GraphGenerator {
 public:
  struct Params {
   public:
    Params(GraphDepth depth, int new_vertices_count)
        : depth_(depth), new_vertices_count_(new_vertices_count) {}
    GraphDepth depth() const { return depth_; }
    int new_vertices_count() const { return new_vertices_count_; }

   private:
    GraphDepth depth_ = 0;
    int new_vertices_count_ = 0;
  };

  explicit GraphGenerator(Params&& params) : params_(std::move(params)) {}
  void generate_grey_branch(Graph& graph,
                            VertexId from_vertex_id,
                            GraphDepth depth,
                            std::mutex& grey_edges_mutex) const;
  void generate_grey_edges(Graph& graph) const;
  void generate_green_edges(Graph& graph,
                            std::mutex& colored_edges_mutex) const;
  void generate_yellow_edges(Graph& graph,
                             std::mutex& colored_edges_mutex) const;
  void generate_red_edges(Graph& graph, std::mutex& colored_edges_mutex) const;

  std::unique_ptr<IGraph> generate() const;

 private:
  Params params_ = Params(0, 0);
};
}  // namespace uni_course_cpp

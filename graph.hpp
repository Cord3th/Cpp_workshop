#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "interfaces/i_graph.hpp"
namespace uni_course_cpp {

class Graph : public IGraph {
 public:
  VertexId add_vertex() override;
  EdgeId add_edge(VertexId from_vertex_id, VertexId to_vertex_id) override;
  bool are_connected(VertexId from_vertex_id,
                     VertexId to_vertex_id) const override;
  GraphDepth get_vertex_depth(VertexId id) const override {
    return vertex_depths_.at(id);
  }
  GraphDepth depth() const override { return depth_vertex_ids_.size(); }
  int vertices_count() const override { return vertices_.size(); }
  int edges_count() const override { return edges_.size(); }
  void for_each_vertex(
      const std::function<void(const IVertex& vertex)>& handler) const override;
  void for_each_edge(
      const std::function<void(const IEdge& edge)>& handler) const override;
  const std::unordered_set<EdgeId>& get_connected_edge_ids(
      VertexId id) const override {
    return adjacency_list_.at(id);
  }
  const std::unordered_set<VertexId>& get_depth_vertex_ids(
      GraphDepth depth) const override {
    return depth_vertex_ids_.at(depth);
  }

 private:
  struct Vertex : IVertex {
   public:
    explicit Vertex(VertexId init_id) : id_(init_id) {}

    VertexId id() const override { return id_; };

   private:
    const VertexId id_ = 0;
  };

  struct Edge : IEdge {
   public:
    Edge(EdgeId init_id,
         VertexId init_from_vertex_id,
         VertexId init_to_vertex_id,
         EdgeColor init_color)
        : id_(init_id),
          from_vertex_id_(init_from_vertex_id),
          to_vertex_id_(init_to_vertex_id),
          color_(init_color) {}
    EdgeId id() const override { return id_; }
    VertexId from_vertex_id() const override { return from_vertex_id_; }
    VertexId to_vertex_id() const override { return to_vertex_id_; }
    EdgeColor color() const override { return color_; }

   private:
    const EdgeId id_ = 0;
    const VertexId from_vertex_id_ = 0;
    const VertexId to_vertex_id_ = 0;
    const EdgeColor color_ = EdgeColor::Grey;
  };
  VertexId vertex_id_counter_ = 0;
  EdgeId edge_id_counter_ = 0;
  VertexId get_new_vertex_id() { return vertex_id_counter_++; }
  EdgeId get_new_edge_id() { return edge_id_counter_++; }
  EdgeColor calculate_edge_color(VertexId from_vertex_id,
                                 VertexId to_vertex_id) const;
  void set_vertex_depth(VertexId id, GraphDepth depth);
  std::vector<Edge> edges_;
  std::vector<Vertex> vertices_;
  std::unordered_map<VertexId, std::unordered_set<EdgeId>> adjacency_list_;
  std::unordered_map<GraphDepth, std::unordered_set<VertexId>>
      depth_vertex_ids_;
  std::unordered_map<VertexId, GraphDepth> vertex_depths_;
};
}  // namespace uni_course_cpp

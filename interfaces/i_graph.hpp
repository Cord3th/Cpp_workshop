#pragma once

#include <functional>
#include <unordered_set>

#include "i_edge.hpp"
#include "i_vertex.hpp"

namespace uni_course_cpp {

using GraphDepth = int;

class IGraph {
 public:
  virtual ~IGraph(){};

  virtual VertexId add_vertex() = 0;
  virtual EdgeId add_edge(VertexId from_vertex_id, VertexId to_vertex_id) = 0;
  virtual bool are_connected(VertexId from_vertex_id,
                             VertexId to_vertex_id) const = 0;
  virtual GraphDepth get_vertex_depth(VertexId id) const = 0;
  virtual GraphDepth depth() const = 0;
  virtual const std::unordered_set<EdgeId>& get_connected_edge_ids(
      VertexId id) const = 0;
  virtual int vertices_count() const = 0;
  virtual int edges_count() const = 0;
  virtual void for_each_vertex(
      const std::function<void(const IVertex& vertex)>& handler) const = 0;
  virtual void for_each_edge(
      const std::function<void(const IEdge& edge)>& handler) const = 0;

  virtual const std::unordered_set<VertexId>& get_depth_vertex_ids(
      GraphDepth depth) const = 0;
};

}  // namespace uni_course_cpp


#include "graph_json_printer.hpp"
#include <sstream>
#include "graph_printer.hpp"

namespace uni_course_cpp {
namespace printing {
namespace json {

std::string print_vertex(const IVertex& vertex, const IGraph& graph) {
  const auto& connected_edges_ids = graph.get_connected_edge_ids(vertex.id());
  std::ostringstream vertex_print_stream;
  vertex_print_stream << "\t{ \"id\": " << vertex.id() << ", \"edge_ids\": [";
  for (auto it = connected_edges_ids.cbegin(); it != connected_edges_ids.cend();
       ++it) {
    if (it != connected_edges_ids.cbegin()) {
      vertex_print_stream << ", ";
    }
    vertex_print_stream << *it;
  }
  vertex_print_stream << "], \"depth\": " << graph.get_vertex_depth(vertex.id())
                      << "}";
  return vertex_print_stream.str();
}

std::string print_edge(const IEdge& edge) {
  std::ostringstream edge_print_stream;
  edge_print_stream << "\t{ \"id\": " << edge.id() << ", \"vertex_ids\": ["
                    << edge.from_vertex_id() << ", " << edge.to_vertex_id()
                    << "], \"color\": \"" << print_edge_color(edge.color())
                    << "\""
                    << "}";
  return edge_print_stream.str();
}

std::string print_edges(const IGraph& graph) {
  std::ostringstream edges_print_stream;
  edges_print_stream << '[' << std::endl;
  bool is_first_elem = true;
  graph.for_each_edge([&edges_print_stream, &is_first_elem](const IEdge& edge) {
    if (!is_first_elem) {
      edges_print_stream << ",\n";
    }
    edges_print_stream << print_edge(edge);
    is_first_elem = false;
  });
  edges_print_stream << "\n]" << std::endl;
  return edges_print_stream.str();
}

std::string print_vertices(const IGraph& graph) {
  std::ostringstream vertices_print_stream;
  vertices_print_stream << '[' << std::endl;
  bool is_first_elem = true;
  graph.for_each_vertex(
      [&vertices_print_stream, &graph, &is_first_elem](const IVertex& vertex) {
        if (!is_first_elem) {
          vertices_print_stream << ",\n";
        }
        vertices_print_stream << print_vertex(vertex, graph);
        is_first_elem = false;
      });
  vertices_print_stream << "\n]," << std::endl;
  return vertices_print_stream.str();
}

std::string print_graph(const IGraph& graph) {
  std::ostringstream graph_print_stream;
  graph_print_stream << "{" << std::endl;
  graph_print_stream << "\"depth\": " << graph.depth() << ',' << std::endl;
  graph_print_stream << "\"vertices\":" << print_vertices(graph);
  graph_print_stream << "\"edges\":" << print_edges(graph);
  graph_print_stream << '}' << std::endl;
  return graph_print_stream.str();
}
}  // namespace json
}  // namespace printing
}  // namespace uni_course_cpp

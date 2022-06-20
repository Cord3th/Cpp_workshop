#include "graph_printer.hpp"
#include <sstream>

namespace uni_course_cpp {
namespace {

static constexpr GraphDepth kDefaultDepth = 1;
constexpr std::array<EdgeColor, 4> kAllColors = {
    EdgeColor::Grey, EdgeColor::Green, EdgeColor::Yellow, EdgeColor::Red};

std::unordered_map<EdgeColor, int> get_edge_color_distribution(
    const IGraph& graph) {
  std::unordered_map<EdgeColor, int> distribution;
  for (const auto color : kAllColors)
    distribution[color] = 0;
  graph.for_each_edge(
      [&distribution](const IEdge& edge) { distribution[edge.color()]++; });
  return distribution;
}

}  // namespace

namespace printing {
std::string print_graph(const IGraph& graph) {
  std::ostringstream graph_print_stream;
  graph_print_stream << "{" << std::endl
                     << "\tdepth: " << graph.depth() << "," << std::endl
                     << "\tvertices: {amount: " << graph.vertices_count()
                     << ", distribution: [";

  for (auto depth = kDefaultDepth; depth != graph.depth(); ++depth) {
    if (depth != kDefaultDepth) {
      graph_print_stream << ",";
    }
    graph_print_stream << " " << graph.get_depth_vertex_ids(depth).size();
  }

  graph_print_stream << "]}," << std::endl
                     << "\tedges: {amount: " << graph.edges_count()
                     << ", distribution: {";

  const auto edge_color_distribution = get_edge_color_distribution(graph);
  for (const auto color : kAllColors) {
    if (color != EdgeColor::Grey) {
      graph_print_stream << ",";
    }
    graph_print_stream << " " << print_edge_color(color) << ": "
                       << edge_color_distribution.at(color);
  }
  graph_print_stream << "}}" << std::endl << "}";
  return graph_print_stream.str();
}

std::string print_edge_color(const EdgeColor& color) {
  switch (color) {
    case EdgeColor::Grey:
      return "grey";
    case EdgeColor::Green:
      return "green";
    case EdgeColor::Yellow:
      return "yellow";
    case EdgeColor::Red:
      return "red";
  }
}
}  // namespace printing
}  // namespace uni_course_cpp

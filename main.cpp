#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "config.hpp"
#include "graph.hpp"
#include "graph_generation_controller.hpp"
#include "graph_generator.hpp"
#include "graph_json_printer.hpp"
#include "graph_printer.hpp"
#include "interfaces/i_graph.hpp"
#include "logger.hpp"

static constexpr int kMinValue = 0;

void prepare_temp_directory() {
  std::filesystem::create_directory(uni_course_cpp::config::kTempDirectoryPath);
}

int handle_input_value(std::string message) {
  std::cout << "Enter the " << message << " (has to be integer and >= 0): \n";
  int input_value;
  std::cin >> input_value;
  while (std::cin.fail() || input_value < kMinValue) {
    std::cout << "Please, enter a correct value, that is integer and >= 0:"
              << std::endl;
    std::cin.clear();
    std::cin.ignore(256, '\n');
    std::cin >> input_value;
  }
  return input_value;
}

void write_to_file(const std::string& graph_string,
                   const std::string& file_name) {
  std::ofstream output_file(uni_course_cpp::config::kTempDirectoryPath +
                            file_name);
  output_file << graph_string;
  output_file.close();
}

std::string generation_started_string(int index) {
  std::stringstream output;
  output << "Graph " << index << ", Generation Started";
  return output.str();
}

std::string generation_finished_string(int index,
                                       const std::string& graph_description) {
  std::stringstream output;
  output << "Graph " << index << ", Generation Finished " << graph_description;
  return output.str();
}

std::vector<std::unique_ptr<uni_course_cpp::IGraph>> generate_graphs(
    uni_course_cpp::GraphGenerator::Params&& params,
    int graphs_count,
    int threads_count) {
  auto generation_controller = uni_course_cpp::GraphGenerationController(
      threads_count, graphs_count, std::move(params));

  auto& logger = uni_course_cpp::Logger::get_logger();

  auto graphs =
      std::vector<std::unique_ptr<uni_course_cpp::IGraph>>(graphs_count);

  generation_controller.generate(
      [&logger](int index) { logger.log(generation_started_string(index)); },
      [&logger, &graphs](int index,
                         std::unique_ptr<uni_course_cpp::IGraph> graph) {
        const auto graph_description =
            uni_course_cpp::printing::print_graph(*graph);
        logger.log(generation_finished_string(index, graph_description));

        const auto graph_json =
            uni_course_cpp::printing::json::print_graph(*graph);
        write_to_file(graph_json, "graph_" + std::to_string(index) + ".json");

        graphs.push_back(std::move(graph));
      });

  return graphs;
}

int main() {
  const int depth = handle_input_value("graph depth");
  const int new_vertices_count = handle_input_value("new_vertices_count");
  const int graphs_count = handle_input_value("graphs count");
  const int threads_count = handle_input_value("threads count");
  prepare_temp_directory();

  auto params =
      uni_course_cpp::GraphGenerator::Params(depth, new_vertices_count);
  const auto graphs =
      generate_graphs(std::move(params), graphs_count, threads_count);

  return 0;
}

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "config.hpp"
#include "logger.hpp"

namespace uni_course_cpp {
namespace {

std::string get_current_date_time() {
  const auto date_time = std::chrono::system_clock::now();
  const auto date_time_t = std::chrono::system_clock::to_time_t(date_time);
  std::stringstream date_time_string;
  date_time_string << std::put_time(std::localtime(&date_time_t),
                                    "%Y.%m.%d %H:%M:%S");
  return date_time_string.str();
}
}  // namespace

Logger::Logger()
    : log_file_(std::ofstream(config::kLogFilePath, std::ios_base::app)) {}

void Logger::log(const std::string& text_to_log) {
  const std::lock_guard lock(mutex_);
  log_file_ << get_current_date_time() << " " << text_to_log << std::endl;
  std::cout << get_current_date_time() << " " << text_to_log << std::endl;
}
}  // namespace uni_course_cpp

#include <memory>
#include <thread>

#include <rclcpp/executors/multi_threaded_executor.hpp>
#include <rclcpp/rclcpp.hpp>

#include "map_processing.h"

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);

  rclcpp::NodeOptions options;
  options.use_intra_process_comms(true);

  auto node = std::make_shared<ellipselio::MappingNode>(options);

  const auto hardware_threads = std::thread::hardware_concurrency();
  const auto executor_threads =
      hardware_threads > 1 ? hardware_threads : static_cast<unsigned int>(2);

  rclcpp::executors::MultiThreadedExecutor executor(
      rclcpp::ExecutorOptions(), executor_threads);
  executor.add_node(node);
  executor.spin();

  rclcpp::shutdown();
  return 0;
}

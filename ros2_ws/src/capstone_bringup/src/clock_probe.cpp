#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <rosgraph_msgs/msg/clock.hpp>
#include "capstone_bringup/clock_progress.hpp"

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("capstone_clock_probe");
  capstone::ClockProgress progress;
  auto subscription = node->create_subscription<rosgraph_msgs::msg::Clock>(
      "/clock", rclcpp::QoS(10).best_effort(),
      [&progress](const rosgraph_msgs::msg::Clock &message) {
        if (message.clock.nanosec >= 1000000000U) {
          progress.observe(-1);
          return;
        }
        const auto ns = static_cast<std::int64_t>(message.clock.sec) * 1000000000LL
                        + message.clock.nanosec;
        progress.observe(ns);
      });
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(45);
  while (rclcpp::ok() && !progress.ready() && !progress.invalid()
         && std::chrono::steady_clock::now() < deadline) {
    rclcpp::spin_some(node);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  const bool passed = progress.ready();
  std::cout << (passed ? "PASS: simulation clock advanced\n"
                       : "FAIL: /clock missing, frozen, invalid or regressed\n");
  rclcpp::shutdown();
  return passed ? 0 : 1;
}

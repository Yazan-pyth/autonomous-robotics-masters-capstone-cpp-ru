#pragma once

#include <cstddef>
#include <cstdint>

namespace capstone {
// Smoke-test state only; not a production clock-reset/safety implementation.
class ClockProgress {
public:
  void observe(std::int64_t nanoseconds) {
    if (nanoseconds < 0 || (seen_ && nanoseconds < previous_)) {
      invalid_ = true;
      return;
    }
    if (seen_ && nanoseconds > previous_) {
      ++advances_;
    }
    previous_ = nanoseconds;
    seen_ = true;
  }

  bool ready() const { return !invalid_ && advances_ >= 20; }
  bool invalid() const { return invalid_; }

private:
  bool seen_{false};
  bool invalid_{false};
  std::int64_t previous_{0};
  std::size_t advances_{0};
};
}  // namespace capstone

#include <stdexcept>
#include "capstone_bringup/clock_progress.hpp"

void require(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

int main() {
  capstone::ClockProgress normal;
  require(!normal.ready(), "No clock must not pass");
  for (int i = 0; i < 100; ++i) {
    normal.observe(0);
  }
  require(!normal.ready(), "Repeated timestamps must not pass");
  for (int i = 1; i < 20; ++i) {
    normal.observe(i * 1000000LL);
  }
  require(!normal.ready(), "Require twenty actual advances");
  normal.observe(20000000);
  require(normal.ready(), "Advancing simulation clock must pass");
  normal.observe(0);
  require(normal.invalid() && !normal.ready(), "A reset invalidates this smoke episode");

  capstone::ClockProgress negative;
  negative.observe(-1);
  require(negative.invalid(), "Negative simulation time must fail");
  for (int i = 0; i < 30; ++i) {
    negative.observe(i);
  }
  require(!negative.ready(), "Invalid state must remain failed");
  return 0;
}

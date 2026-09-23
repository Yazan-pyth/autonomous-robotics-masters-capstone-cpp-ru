#!/usr/bin/env bash
set -euo pipefail

gz sim -s -r /opt/capstone/smoke.sdf &
sim_pid=$!
ros2 run ros_gz_bridge parameter_bridge '/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock' &
bridge_pid=$!

cleanup() {
  kill "$bridge_pid" "$sim_pid" 2>/dev/null || true
  wait "$bridge_pid" "$sim_pid" 2>/dev/null || true
}
trap cleanup EXIT
trap 'exit 143' TERM
trap 'exit 130' INT

set +e
wait -n "$sim_pid" "$bridge_pid"
status=$?
set -e
# Neither long-running process should end during a healthy simulation.
if [ "$status" -eq 0 ]; then
  status=1
fi
exit "$status"

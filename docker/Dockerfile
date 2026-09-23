FROM ros:jazzy-ros-base AS common
SHELL ["/bin/bash", "-o", "pipefail", "-c"]
RUN apt-get update && apt-get install -y --no-install-recommends \
    ros-jazzy-rmw-cyclonedds-cpp \
    && rm -rf /var/lib/apt/lists/*
ENV RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
ENV ROS_DOMAIN_ID=42
ENV ROS_LOCALHOST_ONLY=0

FROM common AS simulator
RUN apt-get update && apt-get install -y --no-install-recommends \
    ros-jazzy-ros-gz-sim ros-jazzy-ros-gz-bridge \
    && rm -rf /var/lib/apt/lists/*
COPY simulation/smoke.sdf /opt/capstone/smoke.sdf
COPY docker/start_simulator.sh /opt/capstone/start_simulator.sh
CMD ["bash", "/opt/capstone/start_simulator.sh"]

FROM common AS autonomy
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake python3-colcon-common-extensions \
    ros-jazzy-rosgraph-msgs \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /ws
COPY ros2_ws/src /ws/src
RUN source /opt/ros/jazzy/setup.bash \
    && colcon build --cmake-args -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    && colcon test --event-handlers console_direct+ \
    && colcon test-result --verbose
CMD ["bash", "-c", "source /ws/install/setup.bash && exec ros2 run capstone_bringup clock_probe"]

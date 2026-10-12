# Robotics — First Project

ROS 1 (Noetic) package `first_project`: converts GPS fixes to odometry, broadcasts both
encoder and GPS odometry as TF, and republishes the LiDAR cloud attached to either frame.

## Nodes

| Node | In | Out |
|---|---|---|
| `gps_to_odom` | `/fix` (`sensor_msgs/NavSatFix`) | `/gps_odom` (`nav_msgs/Odometry`, frame `world`, child `gps_odom`) |
| `odom_to_tf` (×2) | `input_odom` (remapped to `/odom` or `/gps_odom`) | TF `root_frame → child_frame` |
| `lidar_remap` | `/os_cloud_node/points` | `/pointcloud_remapped` with `frame_id` = `wheel_odom` or `gps_odom` (dynamic_reconfigure) |

TF tree:

```
world ─┬─ wheel_odom   (from /odom, encoders)
       └─ gps_odom     (from /gps_odom, GPS)
```

The point cloud is published in the selected robot frame; TF places it in the world, so the
points themselves are not transformed.

### Parameters (`gps_to_odom`, private or global)

| Name | Default | Meaning |
|---|---|---|
| `lat_r`, `lon_r`, `alt_r` | required | ENU reference point (WGS84 degrees, metres) |
| `yaw_offset` | `130.0` | Rotation (deg, CCW about Z) from ENU to the wheel odometry frame |
| `min_move` | `0.5` | Metres travelled before the heading is recomputed (rejects stationary GPS noise) |
| `root_frame` | `world` | Frame of the published odometry |

Heading is the direction of travel between GPS fixes, so it is only meaningful while moving
forward. Fixes with `status < STATUS_FIX` or non-finite coordinates are dropped.

## Build and run

Requires ROS Noetic (EOL May 2025 — use the Docker image below for a pinned environment).

```bash
cd ~/catkin_ws/src && git clone https://github.com/lexdumitrache/robotics-first-project-final.git
cd ~/catkin_ws && catkin_make && source devel/setup.bash
roslaunch first_project launch.launch
rosbag play <project bag>              # bag from the course material, not in the repo
rosrun rqt_reconfigure rqt_reconfigure  # switch lidar_remap frame_id
```

Tests: `catkin_make run_tests && catkin_test_results`.

### Docker

```bash
cd docker && docker build -t robotics:stable .
docker run -it --rm -v "$PWD/../src:/home/ros/catkin_ws/src" robotics:stable
```

`build-docker-image.ps1` / `run-ros.ps1` / `run-vnc.ps1` do the same on Windows (with a noVNC display).

## Academic project and licensing

This repository contains coursework developed for the Robotics course at Politecnico di Milano and is shared for educational and portfolio purposes, subject to applicable course and contributor permissions.

Original contributors should be acknowledged, and third-party components remain subject to their respective licenses. No open-source license is currently granted for the original project code; permission is required before copying, modifying, or redistributing it.

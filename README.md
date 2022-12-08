#  A fast rosbag time slicing tool

## Installation

Build procedure is the standard way for every ROS package (clone this
repo into "src" directory of workspace, then catkin build)

## Tools

```
rosrun fast_rosbag_slice fast_rosbag_slice -i input_bag -o output_bag -s start_time -e stop_time
```
Start and stop time are in seconds UTC (see rosbag info).

## License

This software is issued under the Apache License Version 2.0.

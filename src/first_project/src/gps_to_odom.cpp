#include "ros/ros.h"
#include <nav_msgs/Odometry.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/Quaternion.h>
#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/NavSatStatus.h>
#include <first_project/gps_math.h>
#include <cmath>

struct Config {
    GPS_point ref;
    double alt_r;
    double yaw_offset;   // degrees, rotation from ENU to the wheel odometry frame
    double min_move;     // metres travelled before the heading is updated
    std::string frame_id;
};

bool initialized = false;
Vector resultPrev;      // last position the heading was computed from
double heading = 0;

void odomCallback(const sensor_msgs::NavSatFix::ConstPtr& msg, const Config& cfg, ros::Publisher& pub){
    if (msg->status.status < sensor_msgs::NavSatStatus::STATUS_FIX ||
        !std::isfinite(msg->latitude) || !std::isfinite(msg->longitude) || !std::isfinite(msg->altitude)) {
        ROS_WARN_THROTTLE(5, "Dropping invalid GPS fix (status %d)", msg->status.status);
        return;
    }
    GPS_point point;
    point.lat = msg->latitude;
    point.lon = msg->longitude;
    Vector resultCurrent = GPSToENU(point, cfg.ref, cfg.alt_r, msg->altitude, cfg.yaw_offset);

    if (!initialized) {
        resultPrev = resultCurrent;
        initialized = true;
    } else {
        double dx = resultCurrent.x - resultPrev.x;
        double dy = resultCurrent.y - resultPrev.y;
        // Only trust the direction of travel once the robot has moved further than GPS noise;
        // while stationary the previous heading is kept.
        // ponytail: assumes forward motion; reversing flips the heading until an IMU/encoder yaw is fused.
        if (std::hypot(dx, dy) >= cfg.min_move) {
            heading = atan2(dy, dx);
            resultPrev = resultCurrent;
        }
    }

    nav_msgs::Odometry odom_msg;
    odom_msg.header.stamp = ros::Time::now();
    odom_msg.header.frame_id = cfg.frame_id;
    odom_msg.child_frame_id = "gps_odom";
    odom_msg.pose.pose.position.x = resultCurrent.x;
    odom_msg.pose.pose.position.y = resultCurrent.y;
    odom_msg.pose.pose.position.z = resultCurrent.z;
    tf::Quaternion q;
    q.setRPY(0, 0, heading);
    tf::quaternionTFToMsg(q, odom_msg.pose.pose.orientation);
    pub.publish(odom_msg);
}

int main(int argc, char **argv){
    ros::init(argc, argv, "gps_to_odom");
    ros::NodeHandle n;
    ros::NodeHandle pn("~");
    ros::Publisher gps_to_odom = n.advertise<nav_msgs::Odometry>("gps_odom", 100);

    // Private params win; global ones are kept as a fallback for launch.launch.
    auto param = [&](const std::string& name, double& out, double def) {
        if (!pn.getParam(name, out) && !n.getParam(name, out)) {
            out = def;
            return false;
        }
        return true;
    };
    Config cfg;
    if (!param("lat_r", cfg.ref.lat, 0) | !param("lon_r", cfg.ref.lon, 0) | !param("alt_r", cfg.alt_r, 0)) {
        ROS_FATAL("Reference position lat_r/lon_r/alt_r must be set");
        return 1;
    }
    param("yaw_offset", cfg.yaw_offset, 130.0);
    param("min_move", cfg.min_move, 0.5);
    if (!pn.getParam("root_frame", cfg.frame_id) && !n.getParam("root_frame", cfg.frame_id)) cfg.frame_id = "world";

    ROS_INFO("Reference: lat %f, lon %f, alt %f, yaw offset %f deg", cfg.ref.lat, cfg.ref.lon, cfg.alt_r, cfg.yaw_offset);
    ros::Subscriber sub = n.subscribe<sensor_msgs::NavSatFix>("fix", 100,
        std::bind(odomCallback, std::placeholders::_1, cfg, gps_to_odom));
    ros::spin();
    return 0;
}

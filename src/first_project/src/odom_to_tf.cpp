#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>

void odometryCallback(const nav_msgs::Odometry::ConstPtr& msg, 
                      std::string root_frame, 
                      std::string child_frame) {
    static tf::TransformBroadcaster br;
    tf::Transform transform;
    transform.setOrigin(tf::Vector3(msg->pose.pose.position.x, 
                                    msg->pose.pose.position.y, 
                                    msg->pose.pose.position.z));
    transform.setRotation(tf::Quaternion(msg->pose.pose.orientation.x, 
                                         msg->pose.pose.orientation.y, 
                                         msg->pose.pose.orientation.z, 
                                         msg->pose.pose.orientation.w));
    // Without a configured root frame, use the frame the odometry is expressed in.
    br.sendTransform(tf::StampedTransform(transform, ros::Time::now(),
                                          root_frame.empty() ? msg->header.frame_id : root_frame, child_frame));
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "odom_to_tf");
    ros::NodeHandle nh;
    ros::NodeHandle nh_private("~");

    std::string root_frame, child_frame;
    if (!nh_private.getParam("root_frame", root_frame)) nh.getParam("/root_frame", root_frame);
    if (!nh_private.getParam("child_frame", child_frame)) {
        ROS_FATAL("~child_frame must be set");
        return 1;
    }
    ros::Subscriber sub = nh.subscribe<nav_msgs::Odometry>("input_odom", 10, 
        std::bind(odometryCallback, std::placeholders::_1, root_frame, child_frame));
    ROS_INFO("Root frame: %s, child frame: %s", root_frame.c_str(), child_frame.c_str());
    ros::spin();
    return 0;
}

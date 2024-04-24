#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>

void odomCallback(const nav_msgs::Odometry::ConstPtr& msg, tf::TransformBroadcaster& broadcaster, const std::string& root_frame, const std::string& child_frame) {
    geometry_msgs::TransformStamped odom_trans;
    odom_trans.header.stamp = msg->header.stamp;
    odom_trans.header.frame_id = root_frame;
    odom_trans.child_frame_id = child_frame;
    odom_trans.transform.translation.x = msg->pose.pose.position.x;
    odom_trans.transform.translation.y = msg->pose.pose.position.y;
    odom_trans.transform.translation.z = msg->pose.pose.position.z;
    odom_trans.transform.rotation = msg->pose.pose.orientation;

    // Send the transform
    broadcaster.sendTransform(odom_trans);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "odom_to_tf");
    ros::NodeHandle nh;
    ros::NodeHandle nh_private("~");

    std::string root_frame, child_frame;
    nh_private.param("root_frame", root_frame, std::string("world"));
    nh_private.param("child_frame", child_frame, std::string("odom"));

    tf::TransformBroadcaster broadcaster;
    ros::Subscriber sub = nh.subscribe<nav_msgs::Odometry>("input_odom", 50, boost::bind(odomCallback, _1, boost::ref(broadcaster), root_frame, child_frame));

    ros::spin();
    return 0;
}

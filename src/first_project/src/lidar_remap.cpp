#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <dynamic_reconfigure/server.h>
#include <first_project/lidar_remapConfig.h>

// Global variables
ros::Publisher pcl_pub;
ros::Subscriber pcl_sub;
std::string current_frame_id = "gps_odom";

// Function prototypes
void configCallback(first_project::lidar_remapConfig &config, uint32_t level);
void pointCloudCallback(const sensor_msgs::PointCloud2ConstPtr& cloud);

int main(int argc, char** argv) {
    ros::init(argc, argv, "lidar_remap");
    ros::NodeHandle nh;

    pcl_pub = nh.advertise<sensor_msgs::PointCloud2>("/pointcloud_remapped", 1);
    pcl_sub = nh.subscribe("/os_cloud_node/points", 1, pointCloudCallback);

    // Must be created after ros::init: the server opens a NodeHandle in its constructor.
    dynamic_reconfigure::Server<first_project::lidar_remapConfig> server;
    dynamic_reconfigure::Server<first_project::lidar_remapConfig>::CallbackType f;
    f = boost::bind(&configCallback, _1, _2);
    server.setCallback(f);

    ros::spin();
    return 0;
}

void configCallback(first_project::lidar_remapConfig &config, uint32_t level) {
    current_frame_id = config.frame_id;
}

// The cloud is published in the robot frame selected at runtime (wheel_odom or gps_odom);
// TF places that frame in the world, so the points themselves are not transformed.
void pointCloudCallback(const sensor_msgs::PointCloud2ConstPtr& cloud) {
    sensor_msgs::PointCloud2 output = *cloud;
    output.header.frame_id = current_frame_id;
    pcl_pub.publish(output);
}

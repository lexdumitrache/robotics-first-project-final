#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <dynamic_reconfigure/server.h>
#include <first_project/LidarFrameConfig.h>

class LidarRemapper {
    ros::Publisher pcl_pub;
    ros::Subscriber pcl_sub;
    dynamic_reconfigure::Server<first_project::LidarFrameConfig> server;
    dynamic_reconfigure::Server<first_project::LidarFrameConfig>::CallbackType f;

    std::string current_frame_id;

public:
    LidarRemapper() : current_frame_id("gps_odom") {
        ros::NodeHandle nh;
        pcl_pub = nh.advertise<sensor_msgs::PointCloud2>("/pointcloud_remapped", 1);
        pcl_sub = nh.subscribe("/os_cloud_node/points", 1, &LidarRemapper::pointCloudCallback, this);

        f = boost::bind(&LidarRemapper::configCallback, this, _1, _2);
        server.setCallback(f);
    }

    void configCallback(first_project::LidarFrameConfig &config, uint32_t level) {
        current_frame_id = config.frame_id;
    }

    void pointCloudCallback(const sensor_msgs::PointCloud2ConstPtr& cloud) {
        sensor_msgs::PointCloud2 output = *cloud;
        output.header.frame_id = current_frame_id;
        pcl_pub.publish(output);
    }
};

int main(int argc, char** argv) {
    ros::init(argc, argv, "lidar_remap");
    LidarRemapper remapper;
    ros::spin();
    return 0;
}
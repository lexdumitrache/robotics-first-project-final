#include "ros/ros.h"
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/NavSatFix.h>
#include <cmath>
#include <sstream>
const double a = 6378137; 
const double b = 6356752;
struct Vector { 
    double x, y, z; 
};
struct GPS_point { 
    double lat, lon; 
};
double degreeToRad(double toConvert){
	return toConvert * M_PI / 180;
}
Vector GPSToECEF(GPS_point point, double h){
    double e_2 = 1 - pow(b, 2) / pow(a, 2);
    double n_fi = a / sqrt(1 - e_2 * pow(sin(degreeToRad(point.lat)), 2));
    struct Vector ecef_point;
    ecef_point.x = (n_fi + h) * cos(degreeToRad(point.lat)) * cos(degreeToRad(point.lon));
    ecef_point.y = (n_fi + h) * cos(degreeToRad(point.lat)) * sin(degreeToRad(point.lon));
    ecef_point.z = (n_fi * (1.0 - e_2) + h) * sin(degreeToRad(point.lat));
    return ecef_point;
}

Vector GPSToENU(GPS_point gpsCoordinates, GPS_point ref_position, double alt_r, double h){
	Vector pointsToConvert = GPSToECEF(gpsCoordinates, h);
    Vector enu_points;
    Vector diff;
	Vector ref = GPSToECEF(ref_position, alt_r);
    diff.x = pointsToConvert.x - ref.x;
    diff.y = pointsToConvert.y - ref.y;
    diff.z = pointsToConvert.z - ref.z;
    double conversion_matrix[3][3] = {
        {-sin(degreeToRad(ref_position.lon)), cos(degreeToRad(ref_position.lon)), 0},
        {-sin(degreeToRad(ref_position.lat)) * cos(degreeToRad(ref_position.lon)), -sin(degreeToRad(ref_position.lat)) * sin(degreeToRad(ref_position.lon)), cos(degreeToRad(ref_position.lat))},
        {cos(degreeToRad(ref_position.lat)) * cos(degreeToRad(ref_position.lon)), cos(degreeToRad(ref_position.lat)) * sin(degreeToRad(ref_position.lon)), sin(degreeToRad(ref_position.lat))}
    };

	enu_points.x = conversion_matrix[0][0] * diff.x + conversion_matrix[0][1] * diff.y + conversion_matrix[0][2] * diff.z;
    enu_points.y = conversion_matrix[1][0] * diff.x + conversion_matrix[1][1] * diff.y + conversion_matrix[1][2] * diff.z;
    enu_points.z = conversion_matrix[2][0] * diff.x + conversion_matrix[2][1] * diff.y + conversion_matrix[2][2] * diff.z;
    return enu_points;
}

void odomCallback(const sensor_msgs::NavSatFix::ConstPtr& msg, GPS_point position_r, double alt_r, ros::Publisher& pub ){
	ROS_INFO("lat: %f, lon: %f, alt: %f", msg->latitude, msg->longitude, msg->altitude);
	struct GPS_point point;
	point.lat = msg->latitude;
	point.lon = msg->longitude;
	Vector result = GPSToENU(point, position_r, alt_r, msg->altitude);
	nav_msgs::Odometry odom_msg;
    odom_msg.header.stamp = ros::Time::now();
    odom_msg.header.frame_id = "odom";
    odom_msg.pose.pose.position.x = result.x;
    odom_msg.pose.pose.position.y = result.y;
    odom_msg.pose.pose.position.z = result.z;
    
    pub.publish(odom_msg);
}

int main(int argc, char **argv){

	ros::init(argc, argv, "gps_to_odom");
	ros::NodeHandle n;
	ros::Publisher gps_to_odom = n.advertise<nav_msgs::Odometry>("gps_odom", 100);
	GPS_point position;
	double alt_r;
	n.getParam("/lat_r", position.lat);
	n.getParam("/lon_r", position.lon);
	n.getParam("/alt_r", alt_r);

	ros::Rate loop_rate(10);
	ros::Subscriber sub = n.subscribe<sensor_msgs::NavSatFix>("fix", 100, std::bind(odomCallback, std::placeholders::_1, position, alt_r, gps_to_odom));

  	while (ros::ok()){
    		ros::spinOnce();
    		loop_rate.sleep();
  	}


  	return 0;
}

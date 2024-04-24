#include "ros/ros.h"
#include <nav_msgs/Odometry.h>
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
Vector gpsToECEF(GPS_point point, int h){
    double e_2 = 1 - pow(b, 2) / pow(a, 2);
    double n_fi = a / (1 - e_2 * pow(sin(point.lat), 2));
    struct Vector ecef_point;
    ecef_point.x = (n_fi + h) * cos(point.lat) * cos(point.lon);
    ecef_point.y = (n_fi + h) * cos(point.lat) * sin(point.lon);
    ecef_point.z = (n_fi * (1 - e_2) + h) * sin(point.lat);
    return ecef_point;
}

Vector ECEFToENU(Vector point_to_convert, GPS_point ref_position, int h){
    Vector enu_points;
    Vector right_part;
    right_part.x = point_to_convert.x - ref_position.lat;
    right_part.y = point_to_convert.y - ref_position.lon;
    right_part.z = point_to_convert.z - h;
    double conversion_matrix[3][3] = {
        {-sin(ref_position.lat), -cos(ref_position.lat) * sin(ref_position.lon), cos(ref_position.lat) * cos(ref_position.lon)},
        {0, -sin(ref_position.lon), -cos(ref_position.lon)},
        {-cos(ref_position.lat), -sin(ref_position.lat) * sin(ref_position.lon), sin(ref_position.lat) * cos(ref_position.lon)}
	};

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			switch (j)
			{
			case 0:
				enu_points.x += conversion_matrix[i][j] * right_part.x;
				break;
			case 1:
				enu_points.y += conversion_matrix[i][j] * right_part.y;
				break;
			case 2:
				enu_points.z += conversion_matrix[i][j] * right_part.z;
				break;	
			default:
				break;
			}
		}
		
	}
    return enu_points;
}
int main(int argc, char **argv){

	ros::init(argc, argv, "gps_to_odom");
	ros::NodeHandle n;
	ros::Publisher gps_to_odom = n.advertise<nav_msgs::Odometry>("gps_odom", 1000);
    ros::Subscriber sub = ns.subscribe<nav_msgs::Odometry>("gps_odom", 1000, boost::bind(ECEFToENU, _1, boost::ref(broadcaster), root_frame, child_frame));
	double lat_r;
	double lon_r;
	double alt_r;
	n.getParam("/lat_r", lat_r);
	n.getParam("/lon_r", lon_r);
	n.getParam("/alt_r", alt_r);
	ros::Rate loop_rate(100);

  	while (ros::ok()){


    		ROS_INFO("Running with parameters:");
			ROS_INFO("lat_r: %f, lon_r: %f, alt_r: %f,", lat_r,lon_r,alt_r);
    		ros::spinOnce();

    		loop_rate.sleep();
  	}


  	return 0;
}

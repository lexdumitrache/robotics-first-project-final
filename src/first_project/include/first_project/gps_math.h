#pragma once
#include <cmath>

// WGS84 ellipsoid
const double a = 6378137;
const double b = 6356752;

struct Vector {
    double x, y, z;
};
struct GPS_point {
    double lat, lon;
};

inline double degreeToRad(double toConvert){
    return toConvert * M_PI / 180;
}

inline Vector rotateVector(Vector v, double degrees) {
    double radians = degreeToRad(degrees);
    Vector rotated;
    rotated.x = v.x * cos(radians) - v.y * sin(radians);
    rotated.y = v.x * sin(radians) + v.y * cos(radians);
    rotated.z = v.z;  // Z-axis is unaffected by rotation in the XY-plane
    return rotated;
}

inline Vector GPSToECEF(GPS_point point, double h){
    double e_2 = 1 - pow(b, 2) / pow(a, 2);
    double n_fi = a / sqrt(1 - e_2 * pow(sin(degreeToRad(point.lat)), 2));
    Vector ecef_point;
    ecef_point.x = (n_fi + h) * cos(degreeToRad(point.lat)) * cos(degreeToRad(point.lon));
    ecef_point.y = (n_fi + h) * cos(degreeToRad(point.lat)) * sin(degreeToRad(point.lon));
    ecef_point.z = (n_fi * (1.0 - e_2) + h) * sin(degreeToRad(point.lat));
    return ecef_point;
}

// ENU coordinates of gpsCoordinates relative to ref_position, then rotated by
// yaw_offset degrees about Z to align with the wheel odometry frame.
inline Vector GPSToENU(GPS_point gpsCoordinates, GPS_point ref_position, double alt_r, double h, double yaw_offset){
    Vector pointsToConvert = GPSToECEF(gpsCoordinates, h);
    Vector ref = GPSToECEF(ref_position, alt_r);
    Vector diff;
    diff.x = pointsToConvert.x - ref.x;
    diff.y = pointsToConvert.y - ref.y;
    diff.z = pointsToConvert.z - ref.z;
    double slat = sin(degreeToRad(ref_position.lat)), clat = cos(degreeToRad(ref_position.lat));
    double slon = sin(degreeToRad(ref_position.lon)), clon = cos(degreeToRad(ref_position.lon));
    Vector enu_points;
    enu_points.x = -slon * diff.x + clon * diff.y;
    enu_points.y = -slat * clon * diff.x - slat * slon * diff.y + clat * diff.z;
    enu_points.z = clat * clon * diff.x + clat * slon * diff.y + slat * diff.z;
    return rotateVector(enu_points, yaw_offset);
}

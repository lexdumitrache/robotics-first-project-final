#include <gtest/gtest.h>
#include <first_project/gps_math.h>

const GPS_point ref{45.477669, 9.226740};
const double alt = 169.041;

TEST(GPSToENU, ReferenceIsOrigin) {
    Vector v = GPSToENU(ref, ref, alt, alt, 130.0);
    EXPECT_NEAR(v.x, 0, 1e-6);
    EXPECT_NEAR(v.y, 0, 1e-6);
    EXPECT_NEAR(v.z, 0, 1e-6);
}

TEST(GPSToENU, NorthAndEastWithoutRotation) {
    // 0.001 deg of latitude ~ 111 m north, 0.001 deg of longitude ~ 78 m east at 45.5 N
    Vector north = GPSToENU({ref.lat + 0.001, ref.lon}, ref, alt, alt, 0);
    EXPECT_NEAR(north.x, 0, 0.01);
    EXPECT_NEAR(north.y, 111.1, 0.5);
    Vector east = GPSToENU({ref.lat, ref.lon + 0.001}, ref, alt, alt, 0);
    EXPECT_NEAR(east.x, 78.0, 0.5);
    EXPECT_NEAR(east.y, 0, 0.01);
}

TEST(GPSToENU, YawOffsetRotatesCounterClockwise) {
    Vector v = GPSToENU({ref.lat, ref.lon + 0.001}, ref, alt, alt, 90.0);
    EXPECT_NEAR(v.x, 0, 0.01);
    EXPECT_NEAR(v.y, 78.0, 0.5);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

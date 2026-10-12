# Start from the ROS Noetic perception image
FROM ros:noetic-perception

# Arguments to be passed at build time
ARG USER_ID=1000
ARG GROUP_ID=1000
ARG USER_NAME=ros

ENV DEBIAN_FRONTEND noninteractive

# Install required packages
RUN apt-get update && \
    apt-get install -y nano vim tmux gedit net-tools iproute2 \
        ros-noetic-rqt-common-plugins ros-noetic-rqt-reconfigure ros-noetic-rviz \
        ros-noetic-plotjuggler-ros ros-noetic-foxglove-bridge ros-noetic-turtlesim && \
    rm -rf /var/lib/apt/lists/*

# Create a group and user
RUN groupadd -g ${GROUP_ID} ${USER_NAME} && \
    useradd -m -u ${USER_ID} -g ${GROUP_ID} -s /bin/bash ${USER_NAME}

# Create catkin workspace and add it to .bashrc
RUN mkdir -p /home/${USER_NAME}/catkin_ws/src && \
    chown -R ${USER_NAME}:${GROUP_ID} /home/${USER_NAME}/catkin_ws && \
    echo "source /home/${USER_NAME}/catkin_ws/devel/setup.bash" >> /home/${USER_NAME}/.bashrc
    
# Switch to the new user
USER ${USER_NAME}

# Switch to the directory and initialize the workspace 
RUN /bin/bash -c '. /opt/ros/noetic/setup.bash; cd /home/${USER_NAME}/catkin_ws; catkin_make'


# Set the working directory to the catkin workspace 
WORKDIR /home/${USER_NAME}/catkin_ws

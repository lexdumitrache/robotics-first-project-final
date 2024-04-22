$CONTAINER_NAME = "robotics"

# Check if the container exists
$existingContainer = docker ps -a -q -f "name=^/${CONTAINER_NAME}$"
if ($existingContainer) {
    Write-Host "Container '$CONTAINER_NAME' already running. Removing it ..."
    docker rm -f $CONTAINER_NAME
    Write-Host "Container '$CONTAINER_NAME' has been removed."
} else {
    Write-Host "Container '$CONTAINER_NAME' does not exist."
}

Write-Host "Starting '$CONTAINER_NAME' container."

# Get the current user's username by splitting the WHOAMI output
$currentUserName = whoami
$userName = $currentUserName.Split('\')[1]

# Replace spaces with underscores
$userName = $userName.Replace(' ', '_')

# Replace apostrophes with underscores
$userName = $userName.Replace("'", "_")

$MOUNT_PATH = "/home/$userName/catkin_ws/src"
$SRC_PATH = Join-Path (Get-Location).Path "../src"

# Start the container. Enabled to work with vnc
$dockerRunCommand = "docker run -it " +
    "--net=ros " +
    "--env=`"DISPLAY=novnc:0.0`" " +
    "--volume=`"${SRC_PATH}:${MOUNT_PATH}`" " +
    "--name=$CONTAINER_NAME " +
    "robotics:stable " +
    "/bin/bash"

Invoke-Expression $dockerRunCommand

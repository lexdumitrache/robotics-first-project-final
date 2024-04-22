# Get the current user's username by splitting the WHOAMI output
$currentUserName = whoami
$userName = $currentUserName.Split('\')[1]

# Replace spaces with underscores
$userName = $userName.Replace(' ', '_')

# Replace apostrophes with underscores
$userName = $userName.Replace("'", "_")

# Docker build command with arguments
$userId = '1000'
$groupId = '1000'
$imageTag = 'robotics:stable'
$dockerBuildCommand = "docker build --build-arg USER_ID='$userId' --build-arg GROUP_ID='$groupId' --build-arg USER_NAME='$userName' -t $imageTag ."

# Execute the Docker build command
Invoke-Expression $dockerBuildCommand
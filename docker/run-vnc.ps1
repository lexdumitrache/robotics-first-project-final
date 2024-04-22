# Run Docker command in detached mode, automatically remove the container on exit,
# set network to 'ros', set environment variables, name the container 'novnc',
# map port 8080 on the host to port 8080 on the container, and use the 'theasp/novnc:latest' image
docker run -d --rm `
   --net=ros `
   --env="DISPLAY_WIDTH=1920" --env="DISPLAY_HEIGHT=1080" --env="RUN_XTERM=no" `
   --name=novnc -p=8080:8080 `
   theasp/novnc:latest

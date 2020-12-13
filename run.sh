

# check to see if docker daemon is running windows containers
WIN_DOCKER_COUNT=$(docker version | grep windows/ | wc -l)
if [ $WIN_DOCKER_COUNT -eq 2 ]
then
    echo "Switching to linux containers..."
    /c/Program\ Files/Docker/Docker/DockerCli.exe -SwitchLinuxEngine
fi

#echo $PWD
CWD=C:/web/workstudyplay/thing

# docker run -v $PWD:/arduino-cli -w /arduino-cli arduino/arduino-cli:builder-0.1 goreleaser --rm-dist --snapshot --skip-publish

# docker run -it \
#     -v $CWD:/arduino-cli \
#     arduino/arduino-cli:builder-0.1 goreleaser \
#     --rm-dist --snapshot --skip-publish

# docker run -it \
#     -v $CWD:/arduino-cli \
#     arduino/arduino-cli:builder-0.1 goreleaser \
#     board list





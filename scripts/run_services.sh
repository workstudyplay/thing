#!/bin/bash


# clean your docker: docker system prune -a

#this will only work on windows10
if [[ "$OSTYPE" == "msys" ]]; then

# check to see if docker daemon is running windows containers
WIN_DOCKER_COUNT=$(docker version | grep windows/ | wc -l)
if [ $WIN_DOCKER_COUNT -eq 2 ]
then
    echo "Switching to linux containers..."
    /c/Program\ Files/Docker/Docker/DockerCli.exe -SwitchLinuxEngine
fi
# else
#     echo "Switching to windows containers..."
#     /c/Program\ Files/Docker/Docker/DockerCli.exe -SwitchWindowsEngine

# if [ eval $CMD -eq 0 ]
# then
#     echo "currently running linux containers"
# else
#     echo "currently running windows containers"
# fi

# services are linux
# /c/Program\ Files/Docker/Docker/DockerCli.exe -SwitchLinuxEngine
fi

if [[ "$OSTYPE" == "darwin19" || "$OSTYPE" == "msys" || "$OSTYPE" == "linux-gnu" ]]; then

REDIS_INSTANCE_NAME="workstudyplay-redis"
POSTGRES_INSTANCE_NAME="workstudyplay-postgres"
# MYSQL_INSTANCE_NAME="workstudyplay-mysql"
# JAEGER_INSTANCE_NAME="workstudyplay-jaeger"
# MONGO_INSTANCE_NAME="workstudyplay-mongo"

# FIRESTORE_INSTANCE_NAME="citadel-firestore-emulator"
# FIRESTORE_PROJECT_ID=emulator

if [[ "$1" == "--halt" ]]; then
    echo "halting..."
    docker stop $REDIS_INSTANCE_NAME
    docker stop $POSTGRES_INSTANCE_NAME
    # docker stop $JAEGER_INSTANCE_NAME
    # docker stop $MYSQL_INSTANCE_NAME
    # docker stop $FIRESTORE_INSTANCE_NAME
    # docker stop $MONGO_INSTANCE_NAME
    exit
fi


HAS_REDIS_RUNNING=$(docker ps --filter "name=$REDIS_INSTANCE_NAME" | wc -l)
if [ $HAS_REDIS_RUNNING -eq 1 ]
then
    HAS_REDIS_CONTAINER=$(docker container ls -a --filter "name=$REDIS_INSTANCE_NAME" | wc -l)
    if [ $HAS_REDIS_CONTAINER -eq 1 ]
    then
        echo "CREATE REDIS CONTAINER as $REDIS_INSTANCE_NAME"
        docker run -d -p 6379:6379 --name $REDIS_INSTANCE_NAME redis:latest
    else
        echo "START REDIS CONTAINER as $REDIS_INSTANCE_NAME"
        docker start $REDIS_INSTANCE_NAME
    fi
else
     echo "REDIS IS RUNNING as $REDIS_INSTANCE_NAME"
fi


HAS_POSTGRES_RUNNING=$(docker ps --filter "name=$POSTGRES_INSTANCE_NAME" | wc -l)
if [ $HAS_POSTGRES_RUNNING -eq 1 ]
then
    HAS_POSTGRES_CONTAINER=$(docker container ls -a --filter "name=$POSTGRES_INSTANCE_NAME" | wc -l)
    if [ $HAS_POSTGRES_CONTAINER -eq 1 ]
    then
        echo "CREATE POSTGRES CONTAINER as $POSTGRES_INSTANCE_NAME"
        docker run -d -p 5432:5432 --name $POSTGRES_INSTANCE_NAME -e POSTGRES_PASSWORD=password postgres:12
    else
        echo "START POSTGRES CONTAINER as $POSTGRES_INSTANCE_NAME"
        docker start $POSTGRES_INSTANCE_NAME
    fi
else
     echo "POSTGRES IS RUNNING as $POSTGRES_INSTANCE_NAME"
fi


# HAS_MYSQL_RUNNING=$(docker ps --filter "name=$MYSQL_INSTANCE_NAME" | wc -l)
# if [ $HAS_MYSQL_RUNNING -eq 1 ]
# then
#     HAS_MYSQL_CONTAINER=$(docker container ls -a --filter "name=$MYSQL_INSTANCE_NAME" | wc -l)
#     if [ $HAS_MYSQL_CONTAINER -eq 1 ]
#     then
#         echo "CREATE MYSQL CONTAINER as $MYSQL_INSTANCE_NAME"
#         docker run -d -p 3307:3307 --name $MYSQL_INSTANCE_NAME -e MYSQL_ROOT_PASSWORD= -e MYSQL_ALLOW_EMPTY_PASSWORD=yes -e MYSQL_DATABASE=chat mysql:5.6
#     else
#         echo "START MYSQL CONTAINER as $MYSQL_INSTANCE_NAME"
#         docker start $MYSQL_INSTANCE_NAME
#     fi
# else
#      echo "MYSQL IS RUNNING as $MYSQL_INSTANCE_NAME"
# fi

# HAS_MONGO_RUNNING=$(docker ps --filter "name=$MONGO_INSTANCE_NAME" | wc -l)
# if [ $HAS_MONGO_RUNNING -eq 1 ]
# then
#     HAS_MONGO_CONTAINER=$(docker container ls -a --filter "name=$MONGO_INSTANCE_NAME" | wc -l)
#     if [ $HAS_MONGO_CONTAINER -eq 1 ]
#     then
#         echo "CREATE MONGO CONTAINER as $MONGO_INSTANCE_NAME"
#         docker run -d -p 27017:27017 --name $MONGO_INSTANCE_NAME -e MONGO_PASSWORD=password mongo:latest
#     else
#         echo "START MONGO CONTAINER as $MONGO_INSTANCE_NAME"
#         docker start $MONGO_INSTANCE_NAME
#     fi
# else
#      echo "MONGO IS RUNNING as $MONGO_INSTANCE_NAME"
# fi


# HAS_JAEGER_RUNNING=$(docker ps --filter "name=$JAEGER_INSTANCE_NAME" | wc -l)
# if [ $HAS_JAEGER_RUNNING -eq 1 ]
# then
#     HAS_JAEGER_CONTAINER=$(docker container ls -a --filter "name=$JAEGER_INSTANCE_NAME" | wc -l)
#     if [ $HAS_JAEGER_CONTAINER -eq 1 ]
#     then
#         echo "CREATE JAEGER CONTAINER as $JAEGER_INSTANCE_NAME"
#         docker run -d --name $JAEGER_INSTANCE_NAME \
#             -e COLLECTOR_ZIPKIN_HTTP_PORT=9411 \
#             -p 5775:5775/udp \
#             -p 6831:6831/udp \
#             -p 6832:6832/udp \
#             -p 5778:5778 \
#             -p 16686:16686 \
#             -p 14268:14268 \
#             -p 14250:14250 \
#             -p 9411:9411 \
#             jaegertracing/all-in-one:1.17        
#     else
#         echo "START JAEGER CONTAINER as $JAEGER_INSTANCE_NAME"
#         docker start $JAEGER_INSTANCE_NAME
#     fi
# else
#      echo "JAEGER IS RUNNING as $JAEGER_INSTANCE_NAME"
# fi

# HAS_FIRESTORE_RUNNING=$(docker ps --filter "name=$FIRESTORE_INSTANCE_NAME" | wc -l)
# if [ $HAS_FIRESTORE_RUNNING -eq 1 ]
# then
#     HAS_FIRESTORE_CONTAINER=$(docker container ls -a --filter "name=$FIRESTORE_INSTANCE_NAME" | wc -l)
#     if [ $HAS_FIRESTORE_CONTAINER -eq 1 ]
#     then
#         echo "CREATE FIRESTORE CONTAINER as $FIRESTORE_INSTANCE_NAME with FIRESTORE_PROJECT_ID=$FIRESTORE_PROJECT_ID"
#         docker run \
#             -d \
#             --name $FIRESTORE_INSTANCE_NAME \
#             --env "FIRESTORE_PROJECT_ID=$FIRESTORE_PROJECT_ID" \
#             --env "PORT=8080" \
#             --publish 9090:8080 \
#             mtlynch/firestore-emulator-docker
#     else
#         echo "START FIRESTORE CONTAINER as $FIRESTORE_INSTANCE_NAME"
#         docker start $FIRESTORE_INSTANCE_NAME
#     fi
# else
#      echo "FIRESTORE IS RUNNING as $FIRESTORE_INSTANCE_NAME"
# fi

# HAS_KAFKA_RUNNING=$(docker ps --filter "name=$KAFKA_INSTANCE_NAME" | wc -l)
# if [ $HAS_KAFKA_RUNNING -eq 1 ]
# then
#     HAS_KAFKA_CONTAINER=$(docker ps --filter "name=$KAFKA_INSTANCE_NAME" | wc -l)
#     if [ $HAS_KAFKA_CONTAINER -eq 1 ]
#     then
#         echo "NEED TO START KAFKA"
#         docker start $KAFKA_INSTANCE_NAME
#     else
#         echo "START KAFKA"
# TODO: add kafka start command
#     fi
# else
#      echo "instance $KAFKA_INSTANCE_NAME is running"
# fi


else
    echo "ERROR: Detected operating systemn as: $OSTYPE, not currently supported"
fi

# bash ./scripts/run_ngrok.sh

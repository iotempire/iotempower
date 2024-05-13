#!/usr/bin/env bash

if [[ "$1" == "multi" ]]; then
    # make sure to install docker-buildx, qemu-user-static, and qemu-user-static-binfmt for multi platform
    docker buildx create --name mymultiplatformbuilder --use
    docker buildx inspect --bootstrap
    docker buildx build --platform=amd64,arm64 \
        -t ulno/iotempower:$(cat VERSION) \
        -t ulno/iotempower:latest --push . # dot at end
else
    docker build -t ulno/iotempower:$(cat VERSION) -t ulno/iotempower:latest . # dot at end
fi

# if you forget to tag it as latest, you can run this:
# docker tag ulno/iotempower:$(cat VERSION) ulno/iotempower:latest
#
# to run: 
# obsolete: docker run -d -p 40080:40080 -p 1883:1883 --platform=amd64 --name iotempower-test ulno/iotempower
# now: docker run --name "$IOTEMPOWER_DOCKER_NAME" \
        # -p 40080:40080 \
        # -p 1883:1883 \
        # -it -v "$IOTEMPOWER_DOCKER_SHARE:/iot/data"  ulno/iotempower
#
# to push to docker hub:
# docker push -a ulno/iotempower
#
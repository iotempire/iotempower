# -----------------------------------------------------------------------------
# Dockerfile for IoTempower (https://github.com/iotempire/iotempower)
# Requires Docker buildx: https://docs.docker.com/buildx/working-with-buildx/
# See scripts/build-docker.sh
#
# Builds a multi-arch image for amd64/armv6/armv7/arm64.
# -----------------------------------------------------------------------------

FROM node:buster as builder
LABEL maintainer=ulno

ENV LANG C.UTF-8

# IFDEF PROXY
#! RUN echo 'Acquire::http { Proxy "http://${PROXY}"; };' >> /etc/apt/apt.conf.d/01proxy
# ENDIF

RUN printf "deb http://httpredir.debian.org/debian buster-backports main non-free" > /etc/apt/sources.list.d/backports.list

COPY etc/nginx /etc/nginx

RUN apt-get update -qq && \
    apt-get install -qq --no-install-recommends --yes \
    	build-essential git mosquitto mosquitto-clients \
        haveged virtualenv \
        python2 python2-dev python3 python3-dev \
        python3-setuptools python3-pip python3-venv \
        curl ca-certificates \
        nodejs npm nginx \
        mc micro # makes it easier to navigate (from backports)

RUN npm install node-red
RUN npm install cloudcmd

ENV LANG C.UTF-8


#ARG TARGETARCH
#ARG TARGETVARIANT
#FROM build-$TARGETARCH$TARGETVARIANT as build

WORKDIR /usr/lib/iot

#ENV APP_DIR=/usr/lib/iot
#ENV BUILD_DIR=/build
COPY . .

RUN bash run install quiet

#COPY scripts/install/ ${BUILD_DIR}/scripts/install/


EXPOSE 80
EXPOSE 443
# mosquitto
EXPOSE 1883

ENTRYPOINT ["bash", "/usr/lib/iot/run"]

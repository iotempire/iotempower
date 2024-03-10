FROM debian:bookworm
LABEL authors="Ulrich Norbisrath <devel@mail.ulno.net>, Araz Heydarov"

ENV HOME=/home/iot

# add new iot user with the iot command as shell
RUN useradd -ms $HOME/bin/iot iot

# Use DEBIAN_FRONTEND=noninteractive to suppress debconf warnings.
ENV DEBIAN_FRONTEND=noninteractive 
WORKDIR $HOME

# Use apt-get for a stable CLI interface and suppress warning messages.
USER root
# as we are doing a "desktop" install, we need sudo
# a bit more for debbugging
RUN apt-get update -y && \
    apt-get install -y --no-install-recommends sudo \
        git net-tools psmisc iproute2 less vim \
        nano \
        apt-utils gnupg ca-certificates && \
    update-ca-certificates

# Copy the local repository into the image, excluding what's listed in .dockerignore
COPY . $HOME/iot

# # get iot script into bin - now in installer
# RUN mkdir -p $HOME/bin
# COPY examples/scripts/iot $HOME/bin

# make iot script executable
ENV PATH="$PATH:$HOME/bin"

# Switch to the just cloned iot directory (to /home/iot/iot)
WORKDIR $HOME/iot

# Install everything as root
RUN ./run x bin/iot_install --quiet

# Clean up package lists
RUN apt-get clean && rm -rf /var/lib/apt/lists/*

# Change the ownership of the /home/iot directory recursively to iot user
RUN chown -R iot:iot $HOME

# Switch to the iot user for running the services
USER iot

# Note: The VOLUME command is used to specify the mount point in the container.
VOLUME ["$HOME/iot-docker-persistent"]

# Reset DEBIAN_FRONTEND for the runtime environment
ENV DEBIAN_FRONTEND=

ENTRYPOINT ["iot", "x", "docker_web_starter"]

# switch to vanilla home directory
WORKDIR $HOME

EXPOSE 1883 40080

# TODO: iot-systems template like on pi
# for building run: 
# docker build -t ulno/iotempower:$(cat VERSION) -t ulno/iotempower:latest .
# if you forget to tag it as latest, you can run this:
# docker tag ulno/iotempower:$(cat VERSION) ulno/iotempower:latest
#
# to run: 
# docker run -d -p 40080:40080 -p 1883:1883 --name iotempower-test ulno/iotempower
#
# to push to docker hub:
# docker push ulno/iotempower:$(cat VERSION)
# docker push ulno/iotempower:latest
#
# TODO:
# - fix IP detection for mosquitto
# - integrate home directory as volumes nicely - maybe only share iot-systems?

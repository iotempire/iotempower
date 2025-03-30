FROM debian:bookworm
LABEL authors="Ulrich Norbisrath <devel@mail.ulno.net>, Araz Heydarov"

# home in docker filesystem
ENV HOME=/iot
ENV IOT_DATA=$HOME/data
ENV IOT_REPO=$IOT_DATA/repo

# Use DEBIAN_FRONTEND=noninteractive to suppress debconf warnings.
ENV DEBIAN_FRONTEND=noninteractive 
# pretend to have some formatting capabilities
ENV TERM=xterm

# # add new iot user with the iot command as shell (will complain that it doesn't find shell)
# RUN useradd -ms $HOME/bin/iot iot 2>/dev/null

WORKDIR $HOME

# Use apt-get for a stable CLI interface and suppress warning messages.
USER root

# as we are doing a "desktop" install, we need sudo
# a bit more for debbugging
RUN mkdir -p $HOME && \
    mkdir -p $IOT_DATA && \
    ln -s $IOT_REPO $HOME/iot && \
    mkdir -p $IOT_DATA/iot-systems && \
    ln -s $IOT_DATA/iot-systems $HOME/iot-systems && \
    mkdir -p $IOT_DATA/.node-red && \
    ln -s $IOT_DATA/.node-red $HOME/.node-red && \
    mkdir -p $IOT_DATA/.platformio && \
    ln -s $IOT_DATA/.platformio $HOME/.platformio && \
    apt-get update -y && \
    apt-get install -y --no-install-recommends sudo \
        git net-tools psmisc iproute2 less xz-utils \
        vim nano \
        apt-utils gnupg ca-certificates && \
    update-ca-certificates

# Add autoshutdown script
COPY ./lib/helpers/docker_autoshutdown /usr/local/bin/autoshutdown

# Copy the local repository into the image, excluding what's listed in .dockerignore
COPY . $IOT_REPO
# can this be done more efficiently as we will have to delete it later

# # get iot script into bin - now in installer
# RUN mkdir -p $HOME/bin
# COPY examples/scripts/iot $HOME/bin

# make iot script executable
ENV PATH="$PATH:$HOME/bin"

# Switch to the iot source directory
WORKDIR $IOT_REPO

#RUN ./run x bin/iot_install --system --core --caddy --mqtt --convenience
RUN ./run x bin/iot_install --default && \
    cd .. &&\
    XZ_OPT='-T0' tar caf "$HOME/iot-local-docker.tar.xz" repo/.local .platformio .node-red && \
    rm -rf "$IOT_REPO" && \
    apt-get clean && rm -rf /var/lib/apt/lists/* 
    # ^^ Clean up package lists

# # Docker permissions are so messed up, going back to full root - so run this rootless!
# # Change the ownership of the /home/iot directory recursively to iot user
# RUN chown -R iot:iot $HOME
# 
# # Switch to the iot user for running the services
# USER iot
ENV USER=root
# 
# Note: The VOLUME command is used to specify the mount point in the container.
VOLUME ["$IOT_DATA"]

#ENTRYPOINT ["iot", "x", "docker_web_starter"]

#ENTRYPOINT ["iot"]

ENTRYPOINT ["/usr/local/bin/autoshutdown"]

#ENTRYPOINT ["bash"]

# switch to vanilla home directory
WORKDIR $HOME

EXPOSE 1883 40080

# TODO:
# - integrate home directory as volumes nicely - maybe only share iot-systems? <- in process
#   check examples/script/iot-container

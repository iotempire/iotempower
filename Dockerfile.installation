FROM debian:bookworm-slim
LABEL authors="ARAZ.HEYDAROV"
ENV WORKDIR /root
WORKDIR $WORKDIR
RUN apt-get update -y && \
    apt-get install git -y && \
    git clone https://github.com/iotempire/iotempower.git iot
WORKDIR $WORKDIR/iot
RUN ./run x ./bin/iot_install --quiet
ENTRYPOINT ["./run", "x", "docker_web_starter"]
EXPOSE 40080
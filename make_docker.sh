# from these sources:
# - https://medium.com/nttlabs/buildx-multiarch-2c6c2df00ca2
# - https://medium.com/@srivathsalachary/on-ubuntu-if-you-get-the-below-error-follow-these-steps-7c764837a181
export DOCKER_CLI_EXPERIMENTAL=enabled
# make sure to have all of qemu/kvm installed
# docker run --rm --privileged linuxkit/binfmt # this seems obsoleted
docker run --privileged  --rm tonistiigi/binfmt --install all
#docker run --rm --privileged multiarch/qemu-user-static --reset -p yes i
#docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
docker buildx rm multibuilder
docker buildx create --name multibuilder
docker buildx inspect multibuilder --bootstrap
docker buildx use multibuilder
#docker buildx build --platform linux/arm,linux/arm64,linux/amd64 . --tag IoTempower
# build and load only one
docker buildx build --platform linux/arm64 . --load --tag iotempower

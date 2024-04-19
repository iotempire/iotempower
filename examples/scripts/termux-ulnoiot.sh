#!/data/data/com.termux/files/usr/usr/bin/env bash
# Get dependencies for iotempower in termux
apt update
apt upgrade
apt install python3 git mc nmap mosquitto \
    termux-api play-audio \
    ncurses-utils util-linux coreutils debianutils
pip install --upgrade virtualenv

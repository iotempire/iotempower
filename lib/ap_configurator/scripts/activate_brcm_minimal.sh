#!/bin/sh

minimal_firmware_path="/lib/firmware/cypress/cyfmac43455-sdio-minimal.bin"
default_firmware_path="/lib/firmware/brcm/brcmfmac43455-sdio.bin"


# if [[ ! -e minimal_firmware_path ]]; then
#     echo "The minimal firmware file could not be found!"
#     exit
# fi

# Check if we have the right firmware in use right now already
symlink_check=$(readlink -f "$default_firmware_path")

if [[ $symlink_check == *"minimal"* ]]; then
  # Minimal firmware is already activated, exit
  # TODO: suggest to undo the changes and bring back standard firmware
  echo "The minimal firmware version is already activated! Abandoning operation"
  exit
fi


# First gather some version information
raspberry_pi_version=$(cat /proc/cpuinfo | grep Model | awk -F ': ' '{print $2}')

# Check if the version matches 3A+, 4, or 5
if [[ "$raspberry_pi_version" =~ "Raspberry Pi 3 Model A+" || "$raspberry_pi_version" =~ "Raspberry Pi 4" || "$raspberry_pi_version" =~ "Raspberry Pi Zero 2 W" ]]; then
    echo "This Raspberry Pi board is supported, going on..."
else
    echo "This Raspberry Pi board is not supported! Abandoning operation"
    exit
fi

# Check if the required firmware file exists on this system
if [ -e "$minimal_firmware_path" ]; then
    echo "Minimal firmware file found, going on..."
else
    echo "No correct version of minimal firmware found! Abandoning operation"
    exit
fi

# Everything OK and we can activate the minimal firmware by setting up a syslink to it
echo "Replacing default firmware file with the minimal version. NB! sudo access is required for the following operation!"

# First remove the update-alternatives links for cyfmac, we're taking over
sudo update-alternatives --quiet --remove-all cyfmac43455-sdio.bin

sudo ln -sf "$minimal_firmware_path" "$default_firmware_path"


echo "Reloading the firmware..."
sudo rmmod brcmfmac
sudo modprobe brcmfmac


# Finally, need a reboot to finalize things
echo "Done!"
#echo "A reboot of your device is required to active the new minimal firmware version."
#echo "Please make sure to do so before using the Access Point."

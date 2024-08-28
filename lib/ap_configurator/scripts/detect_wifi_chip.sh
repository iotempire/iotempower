#!/bin/bash


# An attempt to detect any relevant Wi-Fi chip/driver identifiers 
# from the output of Linux system info tools: dmesg, lsusb, lspci, lshw.
# Outputs a single string with any relevant bits combined together.

filter_msg() {
  # Arg $1 - Provided msg

  # Common driver identifiers that we are interested in
  local filter_str="ath9k|ath10k|ath11k|ath12k|broadcom|realtek|intel|mediatek|brcm|bcm|b43|mt76|mt79|rt35|rt53|iwlwifi|rtl|brcmfmac|brcmsmac"
  
  # Filter the given message for any of the keywords,
  # save the ones present and discard duplicates
  echo $1 | grep -iEo $filter_str | sort -u
}


# Kernel logs
dmesg=$(filter_msg "$(dmesg)")

# Gather status and capabilities,
# join with lshw "logical name"
#filter_msg "$(sudo iwconfig)"

# Lookup from known/supported chips/vendors
# from connected USB devices
usb=$(filter_msg "$(lsusb)")

# Lookup from known/supported chips/vendors
pci=$(filter_msg "$(lspci -nn)")

# Find "Wireless interface" and collect 
# product, vendor, bus info, logical name, 
# serial, capabilities and configuration.
lshw_raw=$(lshw -html -C network)
lshw=$(filter_msg "$lshw_raw")

# Parse lshw's HTML output separately
parsed_output=$(echo $lshw_raw | xmllint --html --xpath "
  //div[contains(@class, 'indented') and 
  div/table[@summary='attributes of network']/tbody/tr[td='Wireless interface']]/
  div/table[@summary='attributes of network']/tbody/tr/td[@class='second']
" - 2>/dev/null)

# Extract the initial 6 fields, interested in the first three lines mostly
awk_output=$(echo "$parsed_output" | awk '
  BEGIN { RS="</td>"; FS="<td class=\"second\">"; } 
  NF>1 { print $2 } 
  NR==6 { exit }')

lshw_config=$(lshw -C network | grep -i configuration)
echo ";lshw_config:$lshw_config;"

# Existing AP software detection
hostapd "--version"
hp1=$?

nmcli "-v"
nm1=$?

hp_avail=""
nm_avail=""
if [ $hp1 -eq 0 ]; then
  hp_avail="hostapd_available"
fi

if [ $nm1 -eq 0 ]; then
  nm_avail="netman_available"
fi

# Separate pass over lspci to hopefully extract manufacturer, chip and driver name 
lspci_output=$(lspci -v)

subsystem=$(echo "$lspci_output" | awk '/Network controller/{f=1} f{if($0 ~ /Subsystem:/){print $0;f=0}}' | sed 's/Subsystem: //')
pci=$(echo "$lspci_output" | awk '/Network controller/{f=1} f{if($0 ~ /PCI bridge:/){print $0;f=0}}' | sed 's/PCI bridge: //')
kernel_driver=$(echo "$lspci_output" | awk '/Network controller/{f=1} f{if($0 ~ /Kernel driver in use:/){print $0;f=0}}' | sed 's/Kernel driver in use: //')

echo ";controller_subsystem:$subsystem;"
echo "pci_controller:$pci;"
echo "kernel_driver_in_use:$kernel_driver;"

# CPU info string, works well on RPis
cpuver=$(cat /proc/cpuinfo | grep Model | awk -F ': ' '{print $2}')
echo "cpuver:$cpuver;"

lsmod_driver=$(lsmod | grep cfg80211)

# Concatenate all outputs
combined=$(echo -e "$awk_output,$dmesg,$usb,$pci,$lshw,$hp_avail,$nm_avail,$lsmod_driver" | sort | uniq)
echo $combined

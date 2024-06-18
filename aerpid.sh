#!/bin/bash

#Colors
gold='\033[0;33m'
yellow='\033[1;33m'
green='\033[1;32m'
dark_green='\033[0;32m'
red='\033[1;31m'
dark_red='\033[0;31m'
gray='\033[1;30m'
nc='\033[0m'

case "$1" in
update)
echo -e "${yellow}Updating AerPID Project...${nc}"
git pull origin master
git submodule update --init --recursive
echo -e "${green}Update Complete!${nc}"
;;
upload)
echo -e "${red}Please make sure 'DEBUG' mode is enabled on your device!${nc}"
sleep 3
echo -e "${yellow}Uploading Firmware...${nc}"
pio run -t upload
echo -e "${yellow}Uploading Web Files...${nc}"
pio run -t uploadfs
echo -e "${green}Upload Complete!${nc}"
;;
upgrade)
echo -e "${green}AerPID Project Upgrade${nc}"
echo -e "${dark_green}-------------------------${nc}"
sleep 1
echo -e "${red}WARNING: ${yellow}This will upgrade the firmware on your device!  ${red}There is no going back from this.${nc}"
echo -e "${gray}I hope you know what you are doing...${nc}"
sleep 2
echo -e "${yellow}Press CTRL+C to abort! ${gray}Start in 4 seconds...${nc}"
sleep 1
for i in $(seq 1 3);
do
    t=$((4-$i))
    echo -e "${gray}${t}${nc}"
    sleep 1
done
echo -e "${green}Upgrading AerPID Project...${nc}"
$0 update
echo -e "${gray}...${nc}"
sleep 1
$0 upload
echo -e "${green}AerPID Firmware Upgrade Complete!${nc}"
;;
devices)
echo -e "${gray}Listing Devices:${nc}"
pio device list
echo -e "${green}Device Listing Complete!${nc}"
;;
*)
echo -e "${gold}Command Arguments: ${green}update, upload, upgrade, devices${nc}"
;;
esac

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
make)
$0 build
ver=$2
echo -e "${green}Aggregating Files...${nc}\n"
cd ../
mkdir ./scripts/out/
mkdir ./scripts/build/

echo -e "${yellow}Aggregating 'aerpid' ...${nc}"
mkdir ./scripts/build/aerpid/
cp .pio/build/aerpid/bootloader.bin ./scripts/build/aerpid/bootloader.bin
cp .pio/build/aerpid/partitions.bin ./scripts/build/aerpid/partitions.bin
cp .pio/build/aerpid/spiffs.bin ./scripts/build/aerpid/spiffs.bin
cp .pio/build/aerpid/firmware.bin ./scripts/build/aerpid/firmware.bin
cp ./scripts/deps/boot_app0.bin ./scripts/build/aerpid/boot_app0.bin
cp ./aerpid_spiffs.csv ./scripts/build/aerpid/aerpid_spiffs.csv

cd ./scripts
cd ./build/aerpid/
echo -e "${gold}Creating Tar...${nc}"
tar -czf aerpid_${ver}.tar.gz *

cd ../../../

echo -e "${gray}Moving Tar...${nc}"
cp ./scripts/build/aerpid/aerpid_${ver}.tar.gz ./scripts/out/aerpid_${ver}.tar.gz

echo -e "${yellow}Aggregating 'aerpid-hp' ...${nc}"
mkdir ./scripts/build/aerpid-hp/
cp .pio/build/aerpid-hp/bootloader.bin ./scripts/build/aerpid-hp/bootloader.bin
cp .pio/build/aerpid-hp/partitions.bin ./scripts/build/aerpid-hp/partitions.bin
cp .pio/build/aerpid-hp/spiffs.bin ./scripts/build/aerpid-hp/spiffs.bin
cp .pio/build/aerpid-hp/firmware.bin ./scripts/build/aerpid-hp/firmware.bin
cp ./scripts/deps/boot_app0.bin ./scripts/build/aerpid-hp/boot_app0.bin
cp ./aerpid_spiffs.csv ./scripts/build/aerpid-hp/aerpid_spiffs.csv

cd ./scripts
cd ./build/aerpid-hp/
echo -e "${gold}Creating Tar...${nc}"
tar -czf aerpid-hp_${ver}.tar.gz *

cd ../../../

echo -e "${gray}Moving Tar...${nc}"
cp ./scripts/build/aerpid-hp/aerpid-hp_${ver}.tar.gz ./scripts/out/aerpid-hp_${ver}.tar.gz

echo -e "${green}Archives Created!${nc}"

cd ./scripts
$0 clean
;;

build)
cd ../
echo -e "${yellow}Building env 'aerpid' ...${nc}"
pio run -e aerpid
pio run -e aerpid --target buildfs
echo -e "${yellow}Building env 'aerpid-hp' ...${nc}"
pio run -e aerpid-hp
pio run -e aerpid-hp --target buildfs
echo -e "${green}Build Complete!!${nc}"
sleep 1
;;

clean)
echo -e "${gold}Cleaning Build Files...${nc}"
sleep 1
rm -rf ./build/aerpid
rm -rf ./build/aerpid-hp
rm -rf ./build
echo -e "${green}Cleaned Build Files!${nc}"
;;

*)
echo -e "${gold}Command Arguments: ${green}all, build, make ${dark_green}<version>${nc}${green},clean${nc}"
;;
esac

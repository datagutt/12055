#!/bin/bash

if [ -z $1 ] 
then
  echo 'usage: "./pack_systemimage.sh  projectname(12083or13009or ...else)"'
else
PRJ=OPPO89T_$1
if [ -d out/target/product/${PRJ}/obj/PACKAGING/systemimage_intermediates/ ]
then
	echo  
else
	mkdir -p out/target/product/${PRJ}/obj/PACKAGING/systemimage_intermediates/
fi
#out/host/linux-x86/bin/make_ext4fs -g 16384 -l 399M -a system out/target/product/oppo75_gb2/obj/PACKAGING/systemimage_intermediates/system.img out/target/product/oppo75_gb2/system
out/host/linux-x86/bin/make_ext4fs -s -l 650M -a system out/target/product/${PRJ}/obj/PACKAGING/systemimage_intermediates/system.img out/target/product/${PRJ}/system
out/host/linux-x86/bin/acp -fpt out/target/product/${PRJ}/obj/PACKAGING/systemimage_intermediates/system.img out/target/product/${PRJ}/system.img
echo "pack Systemimg done!"
fi
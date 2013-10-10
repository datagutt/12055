#!/bin/bash
#make to bootimage.sh
if [ -z $1 ] 
then
  echo 'usage: "./pack_bootimage.sh  projectname(12083or12021)"'
else
PRJ=OPPO89W_$1
out/host/linux-x86/bin/acp -uv kernel/out/kernel_${PRJ}.bin out/target/product/${PRJ}/kernel
out/host/linux-x86/bin/mkbootfs out/target/product/${PRJ}/root | out/host/linux-x86/bin/minigzip > out/target/product/${PRJ}/ramdisk.img
mediatek/build/tools/mkimage out/target/product/${PRJ}/ramdisk.img ROOTFS > out/target/product/${PRJ}/ramdisk_android.img
mv out/target/product/${PRJ}/ramdisk.img mediatek/kernel/trace32/${PRJ}_ramdisk.img
mv out/target/product/${PRJ}/ramdisk_android.img out/target/product/${PRJ}/ramdisk.img
out/host/linux-x86/bin/mkbootimg  --kernel out/target/product/${PRJ}/kernel --ramdisk out/target/product/${PRJ}/ramdisk.img --board 1336460062 --output out/target/product/${PRJ}/boot.img
echo "pack Bootimage done!"
cp mediatek/preloader/preloader_OPPO89W_12093.bin out/target/product/${PRJ}/preloader_OPPO_12093.bin
echo "cp preloader.bin done"
cp bootable/bootloader/lk/build-OPPO89W_12093/lk.bin out/target/product/${PRJ}/lk.bin
echo "cp lk.bin done"
fi

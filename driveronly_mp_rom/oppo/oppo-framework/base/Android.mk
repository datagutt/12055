LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

out_dir := out/target/common/obj/JAVA_LIBRARIES/oppo-framework_intermediates
img_dir := out/target/product/$(TARGET_PRODUCT)/system/framework

copy_cls_lib := $(shell mkdir -p $(out_dir) $(img_dir); \
                cp -f $(LOCAL_PATH)/oppo-framework-classes.jar $(out_dir)/classes.jar; \
                cp -f $(LOCAL_PATH)/oppo-framework-javalib.jar $(out_dir)/javalib.jar; \
                cp -f $(LOCAL_PATH)/oppo-framework-javalib.jar $(img_dir)/oppo-framework.jar \
                )
out_dir := 
img_dir := 
copy_cls_lib :=

ifeq (,$(ONE_SHOT_MAKEFILE))
include $(call first-makefiles-under,$(LOCAL_PATH))
endif

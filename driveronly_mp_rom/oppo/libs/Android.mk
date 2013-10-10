LOCAL_PATH := $(call my-dir)
core_mk      := ../../build/target/product/core.mk
product_mk   := oppo_libs.mk
product_mk_bak   := oppo_libs
include_to_core := $(shell cd $(LOCAL_PATH); if [ ! -f $(product_mk) ];then \
        echo -e "include $(LOCAL_PATH)/$(product_mk)" >>$(core_mk); mv $(product_mk_bak) $(product_mk); fi)

#-------------------
include $(LOCAL_PATH)/oppo_include_libs.mk

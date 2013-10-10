
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#LOCAL_NO_STANDARD_LIBRARIES := true
LOCAL_PACKAGE_NAME := oppo-framework-res
LOCAL_CERTIFICATE := platform

# Tell aapt to create "extending (non-application)" resource IDs,
# since these resources will be used by many apps.
LOCAL_AAPT_FLAGS := -x --oppo-package 12 --oppo-public-id 1024

LOCAL_NO_OPPORES := true

LOCAL_MODULE_TAGS := optional

# Install this alongside the libraries.
LOCAL_MODULE_PATH := $(TARGET_OUT_JAVA_LIBRARIES)

# Create package-export.apk, which other packages can use to get
# PRODUCT-agnostic resource data like IDs and type definitions.
LOCAL_EXPORT_PACKAGE_RESOURCES := true




include $(BUILD_PACKAGE)

# Make sure the system .rs files get compiled before building the package-export.apk.
# $(resource_export_package): $(framework_RenderScript_STAMP_FILE)

# define a global intermediate target that other module may depend on.
.PHONY: oppo-framework-res-package-target
oppo-framework-res-package-target: $(LOCAL_BUILT_MODULE)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE	:= kazbase
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/*/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_CFLAGS	:= "-std=c++11"
LOCAL_CPP_FEATURES := rtti exceptions
include $(BUILD_SHARED_LIBRARY)

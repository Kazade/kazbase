LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE	:= kazbase
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/kazbase/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/kazbase/os/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/kazbase/json/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/kazbase/hash/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_CFLAGS	:= "-std=c++11"
LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_LDLIBS += -latomic
LOCAL_SHARED_LIBRARIES := libpcre
include $(BUILD_SHARED_LIBRARY)

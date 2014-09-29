LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE	:= kazbase
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/kazbase/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/kazbase/os/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/kazbase/json/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/kazbase/hash/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_CPPFLAGS 	+= -fexceptions
LOCAL_CPPFLAGS 	+= -frtti
LOCAL_CPPFLAGS  += -std=c++11 -g
LOCAL_LDLIBS += -latomic -llog
LOCAL_SHARED_LIBRARIES := libpcre
include $(BUILD_SHARED_LIBRARY)

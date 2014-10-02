LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES +=  

LOCAL_CFLAGS += -O0 -g

LOCAL_SHARED_LIBRARIES := \
				libgui \
				libbinder \
				libutils \
				libmedia \
				libcamera_client
		
LOCAL_SRC_FILES:= \
				main.cpp \
				MyThread.cpp

LOCAL_MODULE:= mediaplayer_recorder
include $(BUILD_EXECUTABLE)

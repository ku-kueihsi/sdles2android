LOCAL_PATH := $(call my-dir)

#assimp
include $(CLEAR_VARS)
LOCAL_MODULE := assimp
LOCAL_SRC_FILES := $(ANDROID_TOOLCHAIN)/lib/libassimp.a
LOCAL_EXPORT_C_INCLUDES := $(ANDROID_TOOLCHAIN)/include/assimp
include $(PREBUILT_STATIC_LIBRARY)

#libpng
include $(CLEAR_VARS)
LOCAL_MODULE := libpng
LOCAL_SRC_FILES := $(ANDROID_TOOLCHAIN)/lib/libpng.a
LOCAL_EXPORT_C_INCLUDES := $(ANDROID_TOOLCHAIN)/include/
include $(PREBUILT_STATIC_LIBRARY)

#SDL
include $(CLEAR_VARS)
LOCAL_MODULE := SDL
LOCAL_SRC_FILES := $(ANDROID_TOOLCHAIN)/lib/libSDL2.a
LOCAL_EXPORT_C_INCLUDES := $(ANDROID_TOOLCHAIN)/include/SDL2/
include $(PREBUILT_STATIC_LIBRARY)

#main
include $(CLEAR_VARS)

LOCAL_MODULE := main
LOCAL_C_INCLUDES += ../
LOCAL_C_INCLUDES += ../glworld/
LOCAL_CFLAGS    := -Wall -Wextra -O2 -std=c99
LOCAL_CFLAGS    += -mtune=cortex-a9 -march=armv7-a -mhard-float -mfloat-abi=softfp -mfpu=vfpv3-d16 -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -DANDROID -Wa,--noexecstack -gdwarf-2 -marm -fno-omit-frame-pointer -Wall -W -D_REENTRANT -fPIE
LOCAL_CPPFLAGS  += -O2 -std=c++11
LOCAL_CPPFLAGS  += -mtune=cortex-a9 -march=armv7-a -mhard-float -mfloat-abi=softfp -mfpu=vfpv3-d16 -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -DANDROID -Wa,--noexecstack -gdwarf-2 -marm -fno-omit-frame-pointer -Wall -W -D_REENTRANT -fPIE
LOCAL_SRC_FILES := SDL_android_main.c main.cpp 
LOCAL_SRC_FILES += ../glworld/data_utils.cpp ../glworld/gl_tools.cpp ../glworld/image.cpp ../glworld/mesh.cpp ../glworld/virtualworld.cpp

LOCAL_STATIC_LIBRARIES := SDL assimp libpng
LOCAL_LDLIBS 	:= -lz -lstdc++ -llog -lGLESv1_CM -lGLESv2 -lEGL -landroid

include $(BUILD_SHARED_LIBRARY)
#$(call import-module,SDL)LOCAL_PATH := $(call my-dir)

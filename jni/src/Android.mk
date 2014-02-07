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
LOCAL_CFLAGS    := -Wall -Wextra -O2
LOCAL_CFLAGS    += -march=armv7-a -mfloat-abi=softfp -mfpu=vfp -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -DANDROID -Wa,--noexecstack -std=c99 -mtune=cortex-a9 -march=armv7-a -mhard-float -mfloat-abi=softfp -mfpu=vfpv3-d16 -g -g -gdwarf-2 -marm -O0 -fno-omit-frame-pointer -Wall -W -D_REENTRANT -fPIE -DQT_QML_DEBUG -DQT_DECLARATIVE_DEBUG -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
LOCAL_CPPFLAGS  += -std=c++11 -O2
LOCAL_CPPFLAGS  += -march=armv7-a -mfloat-abi=softfp -mfpu=vfp -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -DANDROID -Wa,--noexecstack -std=c++11 -mtune=cortex-a9 -march=armv7-a -mhard-float -mfloat-abi=softfp -mfpu=vfpv3-d16 -g -g -gdwarf-2 -marm -O0 -fno-omit-frame-pointer -Wall -W -D_REENTRANT -fPIE -DQT_QML_DEBUG -DQT_DECLARATIVE_DEBUG -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
LOCAL_SRC_FILES := SDL_android_main.c main.cpp 
LOCAL_SRC_FILES += ../glworld/data_utils.cpp ../glworld/gl_tools.cpp ../glworld/image.cpp ../glworld/mesh.cpp ../glworld/virtualworld.cpp

LOCAL_STATIC_LIBRARIES := SDL assimp libpng
LOCAL_LDLIBS 	:= -lz -lstdc++ -llog -lGLESv1_CM -lGLESv2 -lEGL -landroid

include $(BUILD_SHARED_LIBRARY)
#$(call import-module,SDL)LOCAL_PATH := $(call my-dir)


# Uncomment this if you're using STL in your project
# See CPLUSPLUS-SUPPORT.html in the NDK documentation for more information
# APP_STL := stlport_static 
APP_STL                 := gnustl_static
APP_ABI                                 := armeabi
NDK_TOOLCHAIN_VERSION   := clang
#NDK_TOOLCHAIN_VERSION   := 4.8
APP_CPPFLAGS                    += -std=c++11
APP_PLATFORM                    := android-18
APP_CFLAGS    += -Wall -Wextra -O2 -std=c99 
APP_CFLAGS    += -mtune=cortex-a9 -march=armv7-a -mhard-float -mfloat-abi=softfp -mfpu=vfpv3-d16 -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -DANDROID -Wa,--noexecstack -gdwarf-2 -marm -fno-omit-frame-pointer -Wall -W -D_REENTRANT -fPIE
APP_CPPFLAGS  += -O2 -std=c++11 
APP_CPPFLAGS  += -mtune=cortex-a9 -march=armv7-a -mhard-float -mfloat-abi=softfp -mfpu=vfpv3-d16 -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -DANDROID -Wa,--noexecstack -gdwarf-2 -marm -fno-omit-frame-pointer -Wall -W -D_REENTRANT -fPIE
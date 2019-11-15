set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 21) # API level

set(CMAKE_ANDROID_ARCH arm)
set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
set(CMAKE_ANDROID_NDK /build/android-ndk-r18/)
set(CMAKE_ANDROID_STL_TYPE c++_static)

# ARM specific settings
set(CMAKE_ANDROID_ARM_NEON 1)

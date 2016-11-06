################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../jni/Droplet.cpp \
../jni/DropletDetector.cpp \
../jni/opencvnative.cpp 

OBJS += \
./jni/Droplet.o \
./jni/DropletDetector.o \
./jni/opencvnative.o 

CPP_DEPS += \
./jni/Droplet.d \
./jni/DropletDetector.d \
./jni/opencvnative.d 


# Each subdirectory must supply rules for building sources it contributes
jni/%.o: ../jni/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++11 -I"C:\Users\Public\Android\android-ndk-r12\platforms\android-9\arch-arm\usr\include" -I"C:\Users\Public\Android\android-ndk-r12\sources\cxx-stl\gnu-libstdc++\4.9\libs\armeabi-v7a\include" -I"C:\Users\Public\Android\android-ndk-r12\sources\cxx-stl\gnu-libstdc++\4.9\include" -I"C:\Android\OpenCV-android-sdk\sdk\native\jni\include" -O0 -g3 -Wall -std=c++11 -O2 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



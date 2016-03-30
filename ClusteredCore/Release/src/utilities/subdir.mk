################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/utilities/config.cpp \
../src/utilities/diagnostics.cpp \
../src/utilities/error.cpp \
../src/utilities/fastRandom.cpp \
../src/utilities/timer.cpp \
../src/utilities/utilities.cpp 

OBJS += \
./src/utilities/config.o \
./src/utilities/diagnostics.o \
./src/utilities/error.o \
./src/utilities/fastRandom.o \
./src/utilities/timer.o \
./src/utilities/utilities.o 

CPP_DEPS += \
./src/utilities/config.d \
./src/utilities/diagnostics.d \
./src/utilities/error.d \
./src/utilities/fastRandom.d \
./src/utilities/timer.d \
./src/utilities/utilities.d 


# Each subdirectory must supply rules for building sources it contributes
src/utilities/%.o: ../src/utilities/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"../include" -O3 -Wall -c -fmessage-length=0 -fopenmp -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



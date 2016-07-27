################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/system/AnalysisSystem.cpp \
../src/system/RecoverySystem.cpp \
../src/system/system.cpp \
../src/system/systemHandling.cpp \
../src/system/systemInit.cpp \
../src/system/systemRecovery.cpp 

OBJS += \
./src/system/AnalysisSystem.o \
./src/system/RecoverySystem.o \
./src/system/system.o \
./src/system/systemHandling.o \
./src/system/systemInit.o \
./src/system/systemRecovery.o 

CPP_DEPS += \
./src/system/AnalysisSystem.d \
./src/system/RecoverySystem.d \
./src/system/system.d \
./src/system/systemHandling.d \
./src/system/systemInit.d \
./src/system/systemRecovery.d 


# Each subdirectory must supply rules for building sources it contributes
src/system/%.o: ../src/system/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"../include" -O3 -Wall -c -fmessage-length=0 -fopenmp -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



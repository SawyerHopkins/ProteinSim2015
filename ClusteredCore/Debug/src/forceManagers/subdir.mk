################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/forceManagers/defaultForceManager.cpp 

OBJS += \
./src/forceManagers/defaultForceManager.o 

CPP_DEPS += \
./src/forceManagers/defaultForceManager.d 


# Each subdirectory must supply rules for building sources it contributes
src/forceManagers/%.o: ../src/forceManagers/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"../include" -O0 -g3 -Wall -c -fmessage-length=0 -fopenmp -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



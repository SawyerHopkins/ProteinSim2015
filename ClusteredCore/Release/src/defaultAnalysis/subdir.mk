################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/defaultAnalysis/analysisCluster.cpp \
../src/defaultAnalysis/analysisGeneral.cpp \
../src/defaultAnalysis/analysisManager.cpp \
../src/defaultAnalysis/analysisOutput.cpp \
../src/defaultAnalysis/analysisPost.cpp \
../src/defaultAnalysis/analysisTracker.cpp 

OBJS += \
./src/defaultAnalysis/analysisCluster.o \
./src/defaultAnalysis/analysisGeneral.o \
./src/defaultAnalysis/analysisManager.o \
./src/defaultAnalysis/analysisOutput.o \
./src/defaultAnalysis/analysisPost.o \
./src/defaultAnalysis/analysisTracker.o 

CPP_DEPS += \
./src/defaultAnalysis/analysisCluster.d \
./src/defaultAnalysis/analysisGeneral.d \
./src/defaultAnalysis/analysisManager.d \
./src/defaultAnalysis/analysisOutput.d \
./src/defaultAnalysis/analysisPost.d \
./src/defaultAnalysis/analysisTracker.d 


# Each subdirectory must supply rules for building sources it contributes
src/defaultAnalysis/%.o: ../src/defaultAnalysis/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"../include" -O3 -Wall -c -fmessage-length=0 -fopenmp -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



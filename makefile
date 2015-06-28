## Debug
ProjectName            :=Clustered
ConfigurationName      :=Debug
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Sawyer Hopkins
Date                   :=06/28/15
CodeLitePath           :="/home/sawyer/.codelite"
LinkerName             :=/usr/bin/g++-4.8
SharedObjectLinkerName :=/usr/bin/g++-4.8 -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="Clustered.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -fopenmp
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++-4.8
CC       := /usr/bin/gcc-4.8
CXXFLAGS :=  -g -O3 -fopenmp -std=c++11 -Wall $(Preprocessors)
CFLAGS   :=  -g -O3 -Wall -fopenmp $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/AOPotential.cpp$(ObjectSuffix) $(IntermediateDirectory)/force.cpp$(ObjectSuffix) $(IntermediateDirectory)/brownianIntegrator.cpp$(ObjectSuffix) $(IntermediateDirectory)/particle.cpp$(ObjectSuffix) $(IntermediateDirectory)/system.cpp$(ObjectSuffix) $(IntermediateDirectory)/cell.cpp$(ObjectSuffix) $(IntermediateDirectory)/utilities.cpp$(ObjectSuffix) $(IntermediateDirectory)/error.cpp$(ObjectSuffix) $(IntermediateDirectory)/timer.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/findClusters.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: clean PreBuild all PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

PostBuild:
	@echo Executing Post Build commands ...
	rm ./Debug/*.o
	@echo Done

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "./main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) "main.cpp"

$(IntermediateDirectory)/AOPotential.cpp$(ObjectSuffix): AOPotential.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "./AOPotential.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/AOPotential.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/AOPotential.cpp$(PreprocessSuffix): AOPotential.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/AOPotential.cpp$(PreprocessSuffix) "AOPotential.cpp"

$(IntermediateDirectory)/force.cpp$(ObjectSuffix): force.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "./force.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/force.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/force.cpp$(PreprocessSuffix): force.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/force.cpp$(PreprocessSuffix) "force.cpp"

$(IntermediateDirectory)/brownianIntegrator.cpp$(ObjectSuffix): brownianIntegrator.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "./brownianIntegrator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/brownianIntegrator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/brownianIntegrator.cpp$(PreprocessSuffix): brownianIntegrator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/brownianIntegrator.cpp$(PreprocessSuffix) "brownianIntegrator.cpp"

$(IntermediateDirectory)/particle.cpp$(ObjectSuffix): particle.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "./particle.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/particle.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/particle.cpp$(PreprocessSuffix): particle.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/particle.cpp$(PreprocessSuffix) "particle.cpp"

$(IntermediateDirectory)/system.cpp$(ObjectSuffix): system.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "./system.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/system.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/system.cpp$(PreprocessSuffix): system.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/system.cpp$(PreprocessSuffix) "system.cpp"

$(IntermediateDirectory)/cell.cpp$(ObjectSuffix): cell.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "./cell.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/cell.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cell.cpp$(PreprocessSuffix): cell.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/cell.cpp$(PreprocessSuffix) "cell.cpp"

$(IntermediateDirectory)/utilities.cpp$(ObjectSuffix): utilities.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "./utilities.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/utilities.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/utilities.cpp$(PreprocessSuffix): utilities.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/utilities.cpp$(PreprocessSuffix) "utilities.cpp"

$(IntermediateDirectory)/error.cpp$(ObjectSuffix): error.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "./error.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/error.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/error.cpp$(PreprocessSuffix): error.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/error.cpp$(PreprocessSuffix) "error.cpp"

$(IntermediateDirectory)/timer.cpp$(ObjectSuffix): timer.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "./timer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/timer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/timer.cpp$(PreprocessSuffix): timer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/timer.cpp$(PreprocessSuffix) "timer.cpp"

$(IntermediateDirectory)/findClusters.cpp$(ObjectSuffix): findClusters.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "./findClusters.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/findClusters.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/findClusters.cpp$(PreprocessSuffix): findClusters.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/findClusters.cpp$(PreprocessSuffix) "findClusters.cpp"

##
## Clean
##
clean:
	$(RM) -r ./Debug/


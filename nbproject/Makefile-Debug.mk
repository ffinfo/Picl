#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/src/Config.o \
	${OBJECTDIR}/src/Task.o \
	${OBJECTDIR}/src/TaskManager.o \
	${OBJECTDIR}/src/clustering/CalclulateNoise.o \
	${OBJECTDIR}/src/clustering/ClearMemorySinglereads.o \
	${OBJECTDIR}/src/clustering/Clustering.o \
	${OBJECTDIR}/src/clustering/ClusteringMain.o \
	${OBJECTDIR}/src/clustering/ClusteringTaskManager.o \
	${OBJECTDIR}/src/clustering/CuttoffCalc.o \
	${OBJECTDIR}/src/clustering/Insersize.o \
	${OBJECTDIR}/src/clustering/Pairing.o \
	${OBJECTDIR}/src/clustering/PairingHighLowQuality.o \
	${OBJECTDIR}/src/clustering/PairingLowQuality.o \
	${OBJECTDIR}/src/clustering/PairingTranslocation.o \
	${OBJECTDIR}/src/clustering/ReadNameConverter.o \
	${OBJECTDIR}/src/clustering/ReadPairedReads.o \
	${OBJECTDIR}/src/clustering/ReadSingleReads.o \
	${OBJECTDIR}/src/clustering/ReferenceDataStorage.o \
	${OBJECTDIR}/src/clustering/SortSingleReads.o \
	${OBJECTDIR}/src/clustering/TranslocationDataStorage.o \
	${OBJECTDIR}/src/compare/CompareMain.o \
	${OBJECTDIR}/src/filter/ClusterFile.o \
	${OBJECTDIR}/src/filter/FilterMain.o \
	${OBJECTDIR}/src/generateconfig/GenerateConfigMain.o \
	${OBJECTDIR}/src/multisample/MultisampleMain.o \
	${OBJECTDIR}/src/pairedbammaker/PairedBamMakerMain.o \
	${OBJECTDIR}/src/splitbam/SplitBamMain.o \
	${OBJECTDIR}/src/viewclusters/ViewClustersMain.o \
	${OBJECTDIR}/src/viewcoverage/CoverageFile.o \
	${OBJECTDIR}/src/viewcoverage/ViewCoverage.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../src/bamtools-master/lib -lbamtools-utils -lbamtools -lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/picl

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/picl: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/picl ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/Config.o: src/Config.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Config.o src/Config.cpp

${OBJECTDIR}/src/Task.o: src/Task.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Task.o src/Task.cpp

${OBJECTDIR}/src/TaskManager.o: src/TaskManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/TaskManager.o src/TaskManager.cpp

${OBJECTDIR}/src/clustering/CalclulateNoise.o: src/clustering/CalclulateNoise.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/CalclulateNoise.o src/clustering/CalclulateNoise.cpp

${OBJECTDIR}/src/clustering/ClearMemorySinglereads.o: src/clustering/ClearMemorySinglereads.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/ClearMemorySinglereads.o src/clustering/ClearMemorySinglereads.cpp

${OBJECTDIR}/src/clustering/Clustering.o: src/clustering/Clustering.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/Clustering.o src/clustering/Clustering.cpp

${OBJECTDIR}/src/clustering/ClusteringMain.o: src/clustering/ClusteringMain.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/ClusteringMain.o src/clustering/ClusteringMain.cpp

${OBJECTDIR}/src/clustering/ClusteringTaskManager.o: src/clustering/ClusteringTaskManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/ClusteringTaskManager.o src/clustering/ClusteringTaskManager.cpp

${OBJECTDIR}/src/clustering/CuttoffCalc.o: src/clustering/CuttoffCalc.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/CuttoffCalc.o src/clustering/CuttoffCalc.cpp

${OBJECTDIR}/src/clustering/Insersize.o: src/clustering/Insersize.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/Insersize.o src/clustering/Insersize.cpp

${OBJECTDIR}/src/clustering/Pairing.o: src/clustering/Pairing.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/Pairing.o src/clustering/Pairing.cpp

${OBJECTDIR}/src/clustering/PairingHighLowQuality.o: src/clustering/PairingHighLowQuality.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/PairingHighLowQuality.o src/clustering/PairingHighLowQuality.cpp

${OBJECTDIR}/src/clustering/PairingLowQuality.o: src/clustering/PairingLowQuality.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/PairingLowQuality.o src/clustering/PairingLowQuality.cpp

${OBJECTDIR}/src/clustering/PairingTranslocation.o: src/clustering/PairingTranslocation.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/PairingTranslocation.o src/clustering/PairingTranslocation.cpp

${OBJECTDIR}/src/clustering/ReadNameConverter.o: src/clustering/ReadNameConverter.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/ReadNameConverter.o src/clustering/ReadNameConverter.cpp

${OBJECTDIR}/src/clustering/ReadPairedReads.o: src/clustering/ReadPairedReads.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/ReadPairedReads.o src/clustering/ReadPairedReads.cpp

${OBJECTDIR}/src/clustering/ReadSingleReads.o: src/clustering/ReadSingleReads.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/ReadSingleReads.o src/clustering/ReadSingleReads.cpp

${OBJECTDIR}/src/clustering/ReferenceDataStorage.o: src/clustering/ReferenceDataStorage.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/ReferenceDataStorage.o src/clustering/ReferenceDataStorage.cpp

${OBJECTDIR}/src/clustering/SortSingleReads.o: src/clustering/SortSingleReads.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/SortSingleReads.o src/clustering/SortSingleReads.cpp

${OBJECTDIR}/src/clustering/TranslocationDataStorage.o: src/clustering/TranslocationDataStorage.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/clustering
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/clustering/TranslocationDataStorage.o src/clustering/TranslocationDataStorage.cpp

${OBJECTDIR}/src/compare/CompareMain.o: src/compare/CompareMain.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/compare
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/compare/CompareMain.o src/compare/CompareMain.cpp

${OBJECTDIR}/src/filter/ClusterFile.o: src/filter/ClusterFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/filter
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/filter/ClusterFile.o src/filter/ClusterFile.cpp

${OBJECTDIR}/src/filter/FilterMain.o: src/filter/FilterMain.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/filter
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/filter/FilterMain.o src/filter/FilterMain.cpp

${OBJECTDIR}/src/generateconfig/GenerateConfigMain.o: src/generateconfig/GenerateConfigMain.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/generateconfig
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/generateconfig/GenerateConfigMain.o src/generateconfig/GenerateConfigMain.cpp

${OBJECTDIR}/src/multisample/MultisampleMain.o: src/multisample/MultisampleMain.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/multisample
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/multisample/MultisampleMain.o src/multisample/MultisampleMain.cpp

${OBJECTDIR}/src/pairedbammaker/PairedBamMakerMain.o: src/pairedbammaker/PairedBamMakerMain.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/pairedbammaker
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/pairedbammaker/PairedBamMakerMain.o src/pairedbammaker/PairedBamMakerMain.cpp

${OBJECTDIR}/src/splitbam/SplitBamMain.o: src/splitbam/SplitBamMain.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/splitbam
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/splitbam/SplitBamMain.o src/splitbam/SplitBamMain.cpp

${OBJECTDIR}/src/viewclusters/ViewClustersMain.o: src/viewclusters/ViewClustersMain.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/viewclusters
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/viewclusters/ViewClustersMain.o src/viewclusters/ViewClustersMain.cpp

${OBJECTDIR}/src/viewcoverage/CoverageFile.o: src/viewcoverage/CoverageFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/viewcoverage
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/viewcoverage/CoverageFile.o src/viewcoverage/CoverageFile.cpp

${OBJECTDIR}/src/viewcoverage/ViewCoverage.o: src/viewcoverage/ViewCoverage.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/viewcoverage
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I../../src/bamtools-master/include -std=c++11 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/viewcoverage/ViewCoverage.o src/viewcoverage/ViewCoverage.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/picl

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc

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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1996375920/once_atomic.o \
	${OBJECTDIR}/_ext/1996375920/thread.o \
	${OBJECTDIR}/libs/boost/libs/program_options/src/cmdline.o \
	${OBJECTDIR}/libs/boost/libs/program_options/src/config_file.o \
	${OBJECTDIR}/libs/boost/libs/program_options/src/convert.o \
	${OBJECTDIR}/libs/boost/libs/program_options/src/options_description.o \
	${OBJECTDIR}/libs/boost/libs/program_options/src/parsers.o \
	${OBJECTDIR}/libs/boost/libs/program_options/src/positional_options.o \
	${OBJECTDIR}/libs/boost/libs/program_options/src/split.o \
	${OBJECTDIR}/libs/boost/libs/program_options/src/utf8_codecvt_facet.o \
	${OBJECTDIR}/libs/boost/libs/program_options/src/value_semantic.o \
	${OBJECTDIR}/libs/boost/libs/program_options/src/variables_map.o \
	${OBJECTDIR}/libs/boost/libs/program_options/src/winmain.o \
	${OBJECTDIR}/libs/boost/libs/system/src/error_code.o \
	${OBJECTDIR}/libs/gtest/src/gtest-death-test.o \
	${OBJECTDIR}/libs/gtest/src/gtest-filepath.o \
	${OBJECTDIR}/libs/gtest/src/gtest-port.o \
	${OBJECTDIR}/libs/gtest/src/gtest-printers.o \
	${OBJECTDIR}/libs/gtest/src/gtest-test-part.o \
	${OBJECTDIR}/libs/gtest/src/gtest-typed-test.o \
	${OBJECTDIR}/libs/gtest/src/gtest.o \
	${OBJECTDIR}/src/classes/App.o \
	${OBJECTDIR}/src/classes/BDb.o \
	${OBJECTDIR}/src/classes/BDbResult.o \
	${OBJECTDIR}/src/classes/BaseLogWriter.o \
	${OBJECTDIR}/src/classes/BlackList.o \
	${OBJECTDIR}/src/classes/CalcBase.o \
	${OBJECTDIR}/src/classes/CalcFull.o \
	${OBJECTDIR}/src/classes/CallsSaver.o \
	${OBJECTDIR}/src/classes/Conf.o \
	${OBJECTDIR}/src/classes/Daemon.o \
	${OBJECTDIR}/src/classes/DataLoader.o \
	${OBJECTDIR}/src/classes/Log.o \
	${OBJECTDIR}/src/classes/LogWriterFile.o \
	${OBJECTDIR}/src/classes/LogWriterScreen.o \
	${OBJECTDIR}/src/classes/LogWriterSyslog.o \
	${OBJECTDIR}/src/classes/Logger.o \
	${OBJECTDIR}/src/classes/ThreadPool.o \
	${OBJECTDIR}/src/classes/UdpControlClient.o \
	${OBJECTDIR}/src/common.o \
	${OBJECTDIR}/src/http/file_handler.o \
	${OBJECTDIR}/src/http/mime_types.o \
	${OBJECTDIR}/src/http/reply.o \
	${OBJECTDIR}/src/http/request_parser.o \
	${OBJECTDIR}/src/http/server.o \
	${OBJECTDIR}/src/lists/CallsObjList.o \
	${OBJECTDIR}/src/lists/ClientCounter.o \
	${OBJECTDIR}/src/lists/ClientObjList.o \
	${OBJECTDIR}/src/lists/CurrentCallsObjList.o \
	${OBJECTDIR}/src/lists/DestObjList.o \
	${OBJECTDIR}/src/lists/FminCounter.o \
	${OBJECTDIR}/src/lists/NetworkPrefixObjList.o \
	${OBJECTDIR}/src/lists/ObjList.o \
	${OBJECTDIR}/src/lists/OperatorList.o \
	${OBJECTDIR}/src/lists/PriceObjList.o \
	${OBJECTDIR}/src/lists/PricelistList.o \
	${OBJECTDIR}/src/lists/RuntimeCallsObjList.o \
	${OBJECTDIR}/src/lists/UsageObjList.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/models/obj.o \
	${OBJECTDIR}/src/tasks/Task.o \
	${OBJECTDIR}/src/tasks/TaskRecalc.o \
	${OBJECTDIR}/src/tests/Test.o \
	${OBJECTDIR}/src/tests/TestLibs.o \
	${OBJECTDIR}/src/tests/TestLoader.o \
	${OBJECTDIR}/src/threads/Thread.o \
	${OBJECTDIR}/src/threads/ThreadBillRuntime.o \
	${OBJECTDIR}/src/threads/ThreadBlacklist.o \
	${OBJECTDIR}/src/threads/ThreadCheckStartTable.o \
	${OBJECTDIR}/src/threads/ThreadCurrentCalls.o \
	${OBJECTDIR}/src/threads/ThreadLimitControl.o \
	${OBJECTDIR}/src/threads/ThreadLoader.o \
	${OBJECTDIR}/src/threads/ThreadLog.o \
	${OBJECTDIR}/src/threads/ThreadSaveCounters.o \
	${OBJECTDIR}/src/threads/ThreadSync.o \
	${OBJECTDIR}/src/threads/ThreadTasks.o \
	${OBJECTDIR}/src/threads/ThreadWeb.o \
	${OBJECTDIR}/src/version.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/pgsql-9.1/lib -Llibs -lpq -lrt -lstdc++

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/voip

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/voip: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/voip ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/1996375920/once_atomic.o: ../boost/libs/thread/src/pthread/once_atomic.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1996375920
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1996375920/once_atomic.o ../boost/libs/thread/src/pthread/once_atomic.cpp

${OBJECTDIR}/_ext/1996375920/thread.o: ../boost/libs/thread/src/pthread/thread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1996375920
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1996375920/thread.o ../boost/libs/thread/src/pthread/thread.cpp

${OBJECTDIR}/libs/boost/libs/program_options/src/cmdline.o: libs/boost/libs/program_options/src/cmdline.cpp 
	${MKDIR} -p ${OBJECTDIR}/libs/boost/libs/program_options/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/boost/libs/program_options/src/cmdline.o libs/boost/libs/program_options/src/cmdline.cpp

${OBJECTDIR}/libs/boost/libs/program_options/src/config_file.o: libs/boost/libs/program_options/src/config_file.cpp 
	${MKDIR} -p ${OBJECTDIR}/libs/boost/libs/program_options/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/boost/libs/program_options/src/config_file.o libs/boost/libs/program_options/src/config_file.cpp

${OBJECTDIR}/libs/boost/libs/program_options/src/convert.o: libs/boost/libs/program_options/src/convert.cpp 
	${MKDIR} -p ${OBJECTDIR}/libs/boost/libs/program_options/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/boost/libs/program_options/src/convert.o libs/boost/libs/program_options/src/convert.cpp

${OBJECTDIR}/libs/boost/libs/program_options/src/options_description.o: libs/boost/libs/program_options/src/options_description.cpp 
	${MKDIR} -p ${OBJECTDIR}/libs/boost/libs/program_options/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/boost/libs/program_options/src/options_description.o libs/boost/libs/program_options/src/options_description.cpp

${OBJECTDIR}/libs/boost/libs/program_options/src/parsers.o: libs/boost/libs/program_options/src/parsers.cpp 
	${MKDIR} -p ${OBJECTDIR}/libs/boost/libs/program_options/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/boost/libs/program_options/src/parsers.o libs/boost/libs/program_options/src/parsers.cpp

${OBJECTDIR}/libs/boost/libs/program_options/src/positional_options.o: libs/boost/libs/program_options/src/positional_options.cpp 
	${MKDIR} -p ${OBJECTDIR}/libs/boost/libs/program_options/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/boost/libs/program_options/src/positional_options.o libs/boost/libs/program_options/src/positional_options.cpp

${OBJECTDIR}/libs/boost/libs/program_options/src/split.o: libs/boost/libs/program_options/src/split.cpp 
	${MKDIR} -p ${OBJECTDIR}/libs/boost/libs/program_options/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/boost/libs/program_options/src/split.o libs/boost/libs/program_options/src/split.cpp

${OBJECTDIR}/libs/boost/libs/program_options/src/utf8_codecvt_facet.o: libs/boost/libs/program_options/src/utf8_codecvt_facet.cpp 
	${MKDIR} -p ${OBJECTDIR}/libs/boost/libs/program_options/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/boost/libs/program_options/src/utf8_codecvt_facet.o libs/boost/libs/program_options/src/utf8_codecvt_facet.cpp

${OBJECTDIR}/libs/boost/libs/program_options/src/value_semantic.o: libs/boost/libs/program_options/src/value_semantic.cpp 
	${MKDIR} -p ${OBJECTDIR}/libs/boost/libs/program_options/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/boost/libs/program_options/src/value_semantic.o libs/boost/libs/program_options/src/value_semantic.cpp

${OBJECTDIR}/libs/boost/libs/program_options/src/variables_map.o: libs/boost/libs/program_options/src/variables_map.cpp 
	${MKDIR} -p ${OBJECTDIR}/libs/boost/libs/program_options/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/boost/libs/program_options/src/variables_map.o libs/boost/libs/program_options/src/variables_map.cpp

${OBJECTDIR}/libs/boost/libs/program_options/src/winmain.o: libs/boost/libs/program_options/src/winmain.cpp 
	${MKDIR} -p ${OBJECTDIR}/libs/boost/libs/program_options/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/boost/libs/program_options/src/winmain.o libs/boost/libs/program_options/src/winmain.cpp

${OBJECTDIR}/libs/boost/libs/system/src/error_code.o: libs/boost/libs/system/src/error_code.cpp 
	${MKDIR} -p ${OBJECTDIR}/libs/boost/libs/system/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/boost/libs/system/src/error_code.o libs/boost/libs/system/src/error_code.cpp

${OBJECTDIR}/libs/gtest/src/gtest-death-test.o: libs/gtest/src/gtest-death-test.cc 
	${MKDIR} -p ${OBJECTDIR}/libs/gtest/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/gtest/src/gtest-death-test.o libs/gtest/src/gtest-death-test.cc

${OBJECTDIR}/libs/gtest/src/gtest-filepath.o: libs/gtest/src/gtest-filepath.cc 
	${MKDIR} -p ${OBJECTDIR}/libs/gtest/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/gtest/src/gtest-filepath.o libs/gtest/src/gtest-filepath.cc

${OBJECTDIR}/libs/gtest/src/gtest-port.o: libs/gtest/src/gtest-port.cc 
	${MKDIR} -p ${OBJECTDIR}/libs/gtest/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/gtest/src/gtest-port.o libs/gtest/src/gtest-port.cc

${OBJECTDIR}/libs/gtest/src/gtest-printers.o: libs/gtest/src/gtest-printers.cc 
	${MKDIR} -p ${OBJECTDIR}/libs/gtest/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/gtest/src/gtest-printers.o libs/gtest/src/gtest-printers.cc

${OBJECTDIR}/libs/gtest/src/gtest-test-part.o: libs/gtest/src/gtest-test-part.cc 
	${MKDIR} -p ${OBJECTDIR}/libs/gtest/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/gtest/src/gtest-test-part.o libs/gtest/src/gtest-test-part.cc

${OBJECTDIR}/libs/gtest/src/gtest-typed-test.o: libs/gtest/src/gtest-typed-test.cc 
	${MKDIR} -p ${OBJECTDIR}/libs/gtest/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/gtest/src/gtest-typed-test.o libs/gtest/src/gtest-typed-test.cc

${OBJECTDIR}/libs/gtest/src/gtest.o: libs/gtest/src/gtest.cc 
	${MKDIR} -p ${OBJECTDIR}/libs/gtest/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libs/gtest/src/gtest.o libs/gtest/src/gtest.cc

${OBJECTDIR}/src/classes/App.o: src/classes/App.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/App.o src/classes/App.cpp

${OBJECTDIR}/src/classes/BDb.o: src/classes/BDb.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/BDb.o src/classes/BDb.cpp

${OBJECTDIR}/src/classes/BDbResult.o: src/classes/BDbResult.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/BDbResult.o src/classes/BDbResult.cpp

${OBJECTDIR}/src/classes/BaseLogWriter.o: src/classes/BaseLogWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/BaseLogWriter.o src/classes/BaseLogWriter.cpp

${OBJECTDIR}/src/classes/BlackList.o: src/classes/BlackList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/BlackList.o src/classes/BlackList.cpp

${OBJECTDIR}/src/classes/CalcBase.o: src/classes/CalcBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/CalcBase.o src/classes/CalcBase.cpp

${OBJECTDIR}/src/classes/CalcFull.o: src/classes/CalcFull.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/CalcFull.o src/classes/CalcFull.cpp

${OBJECTDIR}/src/classes/CallsSaver.o: src/classes/CallsSaver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/CallsSaver.o src/classes/CallsSaver.cpp

${OBJECTDIR}/src/classes/Conf.o: src/classes/Conf.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/Conf.o src/classes/Conf.cpp

${OBJECTDIR}/src/classes/Daemon.o: src/classes/Daemon.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/Daemon.o src/classes/Daemon.cpp

${OBJECTDIR}/src/classes/DataLoader.o: src/classes/DataLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/DataLoader.o src/classes/DataLoader.cpp

${OBJECTDIR}/src/classes/Log.o: src/classes/Log.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/Log.o src/classes/Log.cpp

${OBJECTDIR}/src/classes/LogWriterFile.o: src/classes/LogWriterFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/LogWriterFile.o src/classes/LogWriterFile.cpp

${OBJECTDIR}/src/classes/LogWriterScreen.o: src/classes/LogWriterScreen.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/LogWriterScreen.o src/classes/LogWriterScreen.cpp

${OBJECTDIR}/src/classes/LogWriterSyslog.o: src/classes/LogWriterSyslog.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/LogWriterSyslog.o src/classes/LogWriterSyslog.cpp

${OBJECTDIR}/src/classes/Logger.o: src/classes/Logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/Logger.o src/classes/Logger.cpp

${OBJECTDIR}/src/classes/ThreadPool.o: src/classes/ThreadPool.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/ThreadPool.o src/classes/ThreadPool.cpp

${OBJECTDIR}/src/classes/UdpControlClient.o: src/classes/UdpControlClient.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/classes
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/classes/UdpControlClient.o src/classes/UdpControlClient.cpp

${OBJECTDIR}/src/common.o: src/common.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/common.o src/common.cpp

${OBJECTDIR}/src/http/file_handler.o: src/http/file_handler.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/http/file_handler.o src/http/file_handler.cpp

${OBJECTDIR}/src/http/mime_types.o: src/http/mime_types.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/http/mime_types.o src/http/mime_types.cpp

${OBJECTDIR}/src/http/reply.o: src/http/reply.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/http/reply.o src/http/reply.cpp

${OBJECTDIR}/src/http/request_parser.o: src/http/request_parser.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/http/request_parser.o src/http/request_parser.cpp

${OBJECTDIR}/src/http/server.o: src/http/server.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/http/server.o src/http/server.cpp

${OBJECTDIR}/src/lists/CallsObjList.o: src/lists/CallsObjList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/CallsObjList.o src/lists/CallsObjList.cpp

${OBJECTDIR}/src/lists/ClientCounter.o: src/lists/ClientCounter.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/ClientCounter.o src/lists/ClientCounter.cpp

${OBJECTDIR}/src/lists/ClientObjList.o: src/lists/ClientObjList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/ClientObjList.o src/lists/ClientObjList.cpp

${OBJECTDIR}/src/lists/CurrentCallsObjList.o: src/lists/CurrentCallsObjList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/CurrentCallsObjList.o src/lists/CurrentCallsObjList.cpp

${OBJECTDIR}/src/lists/DestObjList.o: src/lists/DestObjList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/DestObjList.o src/lists/DestObjList.cpp

${OBJECTDIR}/src/lists/FminCounter.o: src/lists/FminCounter.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/FminCounter.o src/lists/FminCounter.cpp

${OBJECTDIR}/src/lists/NetworkPrefixObjList.o: src/lists/NetworkPrefixObjList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/NetworkPrefixObjList.o src/lists/NetworkPrefixObjList.cpp

${OBJECTDIR}/src/lists/ObjList.o: src/lists/ObjList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/ObjList.o src/lists/ObjList.cpp

${OBJECTDIR}/src/lists/OperatorList.o: src/lists/OperatorList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/OperatorList.o src/lists/OperatorList.cpp

${OBJECTDIR}/src/lists/PriceObjList.o: src/lists/PriceObjList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/PriceObjList.o src/lists/PriceObjList.cpp

${OBJECTDIR}/src/lists/PricelistList.o: src/lists/PricelistList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/PricelistList.o src/lists/PricelistList.cpp

${OBJECTDIR}/src/lists/RuntimeCallsObjList.o: src/lists/RuntimeCallsObjList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/RuntimeCallsObjList.o src/lists/RuntimeCallsObjList.cpp

${OBJECTDIR}/src/lists/UsageObjList.o: src/lists/UsageObjList.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/lists
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/lists/UsageObjList.o src/lists/UsageObjList.cpp

${OBJECTDIR}/src/main.o: src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/src/models/obj.o: src/models/obj.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/models
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/models/obj.o src/models/obj.cpp

${OBJECTDIR}/src/tasks/Task.o: src/tasks/Task.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/tasks
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/tasks/Task.o src/tasks/Task.cpp

${OBJECTDIR}/src/tasks/TaskRecalc.o: src/tasks/TaskRecalc.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/tasks
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/tasks/TaskRecalc.o src/tasks/TaskRecalc.cpp

${OBJECTDIR}/src/tests/Test.o: src/tests/Test.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/tests/Test.o src/tests/Test.cpp

${OBJECTDIR}/src/tests/TestLibs.o: src/tests/TestLibs.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/tests/TestLibs.o src/tests/TestLibs.cpp

${OBJECTDIR}/src/tests/TestLoader.o: src/tests/TestLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/tests/TestLoader.o src/tests/TestLoader.cpp

${OBJECTDIR}/src/threads/Thread.o: src/threads/Thread.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/threads
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads/Thread.o src/threads/Thread.cpp

${OBJECTDIR}/src/threads/ThreadBillRuntime.o: src/threads/ThreadBillRuntime.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/threads
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads/ThreadBillRuntime.o src/threads/ThreadBillRuntime.cpp

${OBJECTDIR}/src/threads/ThreadBlacklist.o: src/threads/ThreadBlacklist.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/threads
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads/ThreadBlacklist.o src/threads/ThreadBlacklist.cpp

${OBJECTDIR}/src/threads/ThreadCheckStartTable.o: src/threads/ThreadCheckStartTable.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/threads
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads/ThreadCheckStartTable.o src/threads/ThreadCheckStartTable.cpp

${OBJECTDIR}/src/threads/ThreadCurrentCalls.o: src/threads/ThreadCurrentCalls.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/threads
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads/ThreadCurrentCalls.o src/threads/ThreadCurrentCalls.cpp

${OBJECTDIR}/src/threads/ThreadLimitControl.o: src/threads/ThreadLimitControl.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/threads
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads/ThreadLimitControl.o src/threads/ThreadLimitControl.cpp

${OBJECTDIR}/src/threads/ThreadLoader.o: src/threads/ThreadLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/threads
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads/ThreadLoader.o src/threads/ThreadLoader.cpp

${OBJECTDIR}/src/threads/ThreadLog.o: src/threads/ThreadLog.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/threads
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads/ThreadLog.o src/threads/ThreadLog.cpp

${OBJECTDIR}/src/threads/ThreadSaveCounters.o: src/threads/ThreadSaveCounters.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/threads
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads/ThreadSaveCounters.o src/threads/ThreadSaveCounters.cpp

${OBJECTDIR}/src/threads/ThreadSync.o: src/threads/ThreadSync.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/threads
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads/ThreadSync.o src/threads/ThreadSync.cpp

${OBJECTDIR}/src/threads/ThreadTasks.o: src/threads/ThreadTasks.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/threads
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads/ThreadTasks.o src/threads/ThreadTasks.cpp

${OBJECTDIR}/src/threads/ThreadWeb.o: src/threads/ThreadWeb.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/threads
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/threads/ThreadWeb.o src/threads/ThreadWeb.cpp

${OBJECTDIR}/src/version.o: src/version.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ilibs/boost -Ilibs/gtest/include -I/usr/pgsql-9.1/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/version.o src/version.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/voip

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc

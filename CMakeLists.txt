cmake_minimum_required(VERSION 2.8.4)
project(voip)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(libs libs/boost libs/gtest/include /usr/include/postgresql)
link_directories(/usr/pgsql-9.1/lib)

set(COMMON_SOURCES
    src/common.cpp
    src/version.cpp
    src/classes/App.cpp
    src/classes/BDb.cpp
    src/classes/BDbResult.cpp
    src/classes/BDbTransaction.cpp
    src/classes/BaseLogWriter.cpp
    src/classes/Conf.cpp
    src/classes/Daemon.cpp
    src/classes/Logger.cpp
    src/classes/LogWriterFile.cpp
    src/classes/LogWriterScreen.cpp
    src/classes/LogWriterSyslog.cpp
    src/classes/ThreadPool.cpp
    src/lists/ObjList.cpp
    src/tasks/Task.cpp
    src/threads/Thread.cpp
    src/http/mime_types.cpp
    src/http/reply.cpp
    src/http/request_parser.cpp
    src/http/server.cpp
    src/tests/Test.cpp
    src/tests/TestLibs.cpp

    libs/boost/libs/system/src/error_code.cpp
    libs/boost/libs/program_options/src/cmdline.cpp
    libs/boost/libs/program_options/src/config_file.cpp
    libs/boost/libs/program_options/src/convert.cpp
    libs/boost/libs/program_options/src/options_description.cpp
    libs/boost/libs/program_options/src/parsers.cpp
    libs/boost/libs/program_options/src/positional_options.cpp
    libs/boost/libs/program_options/src/split.cpp
    libs/boost/libs/program_options/src/utf8_codecvt_facet.cpp
    libs/boost/libs/program_options/src/value_semantic.cpp
    libs/boost/libs/program_options/src/variables_map.cpp
    libs/boost/libs/program_options/src/winmain.cpp
    libs/boost/libs/thread/src/pthread/once_atomic.cpp
    libs/boost/libs/thread/src/pthread/thread.cpp
    libs/gtest/src/gtest-death-test.cc
    libs/gtest/src/gtest-filepath.cc
    libs/gtest/src/gtest-port.cc
    libs/gtest/src/gtest-printers.cc
    libs/gtest/src/gtest-test-part.cc
    libs/gtest/src/gtest-typed-test.cc
    libs/gtest/src/gtest.cc
)

set(AUTH_SOURCES
    src-auth/classes/Log.cpp
    src-auth/classes/AppAuth.cpp
    src-auth/classes/BillClient.cpp
    src-auth/classes/ConfAuth.cpp
    src-auth/classes/ConfigVersionData.cpp
    src-auth/classes/UdpMessageProcessor.cpp
    src-auth/http/file_handler.cpp
    src-auth/lists/AirpList.cpp
    src-auth/lists/NumberList.cpp
    src-auth/lists/OperatorList.cpp
    src-auth/lists/OperatorRuleList.cpp
    src-auth/lists/OutcomeList.cpp
    src-auth/lists/PrefixlistList.cpp
    src-auth/lists/PrefixlistPrefixList.cpp
    src-auth/lists/ReleaseReasonList.cpp
    src-auth/lists/RouteCaseList.cpp
    src-auth/lists/RouteTableList.cpp
    src-auth/lists/RouteTableRouteList.cpp
    src-auth/lists/RoutingReportDataList.cpp
    src-auth/lists/TrunkGroupList.cpp
    src-auth/lists/TrunkList.cpp
    src-auth/threads/ThreadLoader.cpp
    src-auth/threads/ThreadLog.cpp
    src-auth/threads/ThreadSync.cpp
    src-auth/threads/ThreadUdpServer.cpp
    src-auth/threads/ThreadWeb.cpp
    src-auth/main.cpp
)

set(BILL_SOURCES
    src-bill/classes/AppBill.cpp
    src-bill/classes/BlackList.cpp
    src-bill/classes/CalcBase.cpp
    src-bill/classes/CalcFull.cpp
    src-bill/classes/CallsSaver.cpp
    src-bill/classes/ConfBill.cpp
    src-bill/classes/DataLoader.cpp
    src-bill/classes/Log.cpp
    src-bill/classes/UdpControlClient.cpp
    src-bill/classes/UdpMessageProcessor.cpp
    src-bill/http/file_handler.cpp
    src-bill/lists/CallsObjList.cpp
    src-bill/lists/ClientCounter.cpp
    src-bill/lists/ClientObjList.cpp
    src-bill/lists/CurrentCallsObjList.cpp
    src-bill/lists/DestObjList.cpp
    src-bill/lists/FminCounter.cpp
    src-bill/lists/GlobalCountersObjList.cpp
    src-bill/lists/NetworkPrefixObjList.cpp
    src-bill/lists/OperatorList.cpp
    src-bill/lists/PricelistList.cpp
    src-bill/lists/PriceObjList.cpp
    src-bill/lists/RuntimeCallsObjList.cpp
    src-bill/lists/UsageObjList.cpp
    src-bill/models/obj.cpp
    src-bill/tasks/TaskRecalc.cpp
    src-bill/tests/TestLoader.cpp
    src-bill/threads/ThreadBillRuntime.cpp
    src-bill/threads/ThreadBlacklist.cpp
    src-bill/threads/ThreadCheckStartTable.cpp
    src-bill/threads/ThreadLimitControl.cpp
    src-bill/threads/ThreadLoader.cpp
    src-bill/threads/ThreadLog.cpp
    src-bill/threads/ThreadSelectCurrentCalls.cpp
    src-bill/threads/ThreadSelectGlobalCounters.cpp
    src-bill/threads/ThreadSync.cpp
    src-bill/threads/ThreadSyncCalls.cpp
    src-bill/threads/ThreadSyncCounters.cpp
    src-bill/threads/ThreadTasks.cpp
    src-bill/threads/ThreadUdpServer.cpp
    src-bill/threads/ThreadWeb.cpp
    src-bill/main.cpp
)


add_executable(app_auth ${COMMON_SOURCES} ${AUTH_SOURCES})
target_link_libraries(app_auth stdc++ rt pthread pq)

add_executable(app_bill ${COMMON_SOURCES} ${BILL_SOURCES})
target_link_libraries(app_bill stdc++ rt pthread pq)

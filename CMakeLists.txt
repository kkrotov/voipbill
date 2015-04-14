cmake_minimum_required(VERSION 2.8)
project(voip)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/usr/include/postgresql)
link_directories(/usr/pgsql-9.4/lib)

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
    src/tasks/Task.cpp
    src/threads/Thread.cpp
    src/http/mime_types.cpp
    src/http/reply.cpp
    src/http/request_parser.cpp
    src/http/server.cpp

    libs/boost/system/src/error_code.cpp
    libs/boost/program_options/src/cmdline.cpp
    libs/boost/program_options/src/config_file.cpp
    libs/boost/program_options/src/convert.cpp
    libs/boost/program_options/src/options_description.cpp
    libs/boost/program_options/src/parsers.cpp
    libs/boost/program_options/src/positional_options.cpp
    libs/boost/program_options/src/split.cpp
    libs/boost/program_options/src/utf8_codecvt_facet.cpp
    libs/boost/program_options/src/value_semantic.cpp
    libs/boost/program_options/src/variables_map.cpp
    libs/boost/program_options/src/winmain.cpp
    libs/boost/thread/src/pthread/once_atomic.cpp
    libs/boost/thread/src/pthread/thread.cpp
)

#set(AUTH_SOURCES
#    src-auth/classes/Log.cpp
#    src-auth/classes/AppAuth.cpp
#    src-auth/classes/BillClient.cpp
#    src-auth/classes/ConfAuth.cpp
#    src-auth/classes/ConfigVersionData.cpp
#    src-auth/classes/UdpMessageProcessor.cpp
#    src-auth/http/file_handler.cpp
#    src-auth/lists/AirpList.cpp
#    src-auth/lists/NumberList.cpp
#    src-auth/lists/OperatorList.cpp
#    src-auth/lists/OperatorRuleList.cpp
#    src-auth/lists/OutcomeList.cpp
#    src-auth/lists/PrefixlistList.cpp
#    src-auth/lists/PrefixlistPrefixList.cpp
#    src-auth/lists/ReleaseReasonList.cpp
#    src-auth/lists/RouteCaseList.cpp
#    src-auth/lists/RouteTableList.cpp
#    src-auth/lists/RouteTableRouteList.cpp
#    src-auth/lists/RoutingReportDataList.cpp
#    src-auth/lists/TrunkGroupList.cpp
#    src-auth/lists/TrunkList.cpp
#    src-auth/threads/ThreadLoader.cpp
#    src-auth/threads/ThreadLog.cpp
#    src-auth/threads/ThreadSync.cpp
#    src-auth/threads/ThreadUdpServer.cpp
#    src-auth/threads/ThreadWeb.cpp
#    src-auth/main.cpp
#)

set(BILL_SOURCES
    src-bill/classes/AppBill.cpp
#    src-bill/classes/BlackList.cpp
#    src-bill/classes/CalcBase.cpp
#    src-bill/classes/CalcFull.cpp
#    src-bill/classes/CallsSaver.cpp
    src-bill/classes/Billing.cpp
    src-bill/classes/BillingCall.cpp
    src-bill/classes/ConfBill.cpp
    src-bill/classes/DataLoader.cpp
    src-bill/classes/Log.cpp
#    src-bill/classes/UdpControlClient.cpp
#    src-bill/classes/UdpMessageProcessor.cpp
#    src-bill/classes/CallsList.cpp
    src-bill/http/file_handler.cpp
#    src-bill/tasks/TaskRecalc.cpp
    src-bill/sync/BasePull.cpp
    src-bill/sync/ManagerPull.cpp
    src-bill/threads/ThreadLog.cpp
    src-bill/threads/ThreadSync.cpp
    src-bill/threads/ThreadLoader.cpp
    src-bill/threads/ThreadSelectCurrentCalls.cpp
#    src-bill/threads/ThreadBillRuntime.cpp
#    src-bill/threads/ThreadBlacklist.cpp
#    src-bill/threads/ThreadCheckStartTable.cpp
#    src-bill/threads/ThreadLimitControl.cpp
#    src-bill/threads/ThreadSelectGlobalCounters.cpp
#    src-bill/threads/ThreadTasks.cpp
#    src-bill/threads/ThreadUdpServer.cpp
#    src-bill/threads/ThreadSyncCalls.cpp
#    src-bill/threads/ThreadSyncCounters.cpp
    src-bill/threads/ThreadWeb.cpp
    src-bill/main.cpp
)


#add_executable(app_auth ${COMMON_SOURCES} ${AUTH_SOURCES})
#target_link_libraries(app_auth stdc++ rt pthread pq)

add_executable(app_bill ${COMMON_SOURCES} ${BILL_SOURCES} src-bill/data/BaseData.cpp)
target_link_libraries(app_bill stdc++ rt pthread pq)

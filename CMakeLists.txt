cmake_minimum_required(VERSION 2.8)
project(voip)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")

#include_directories(/usr/include/postgresql libs/jsoncpp-master/include)
include_directories(/usr/pgsql-9.4/include libs/jsoncpp-master/include libs/boost)
link_directories(/usr/pgsql-9.4/lib)

string(TIMESTAMP RUN_DATE "%Y-%m-%d")
execute_process (COMMAND git rev-parse --verify HEAD OUTPUT_VARIABLE CURRENT_HEAD
                 OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND echo ${CURRENT_HEAD})
add_definitions(-D_DATECOMPILED_="${RUN_DATE}" -D_CURRENT_HEAD_="${CURRENT_HEAD}")


set(BOOST_SOURCES
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
)

set(JSON_SOURCES
        libs/jsoncpp-master/src/lib_json/json_value.cpp
        libs/jsoncpp-master/src/lib_json/json_reader.cpp
        libs/jsoncpp-master/src/lib_json/json_writer.cpp
)

set(BILL_SOURCES
    src-bill/common.cpp
    src-bill/version.cpp
    src-bill/classes/daemon/DaemonApp.cpp
    src-bill/classes/daemon/DaemonWorker.cpp
    src-bill/classes/daemon/DaemonMonitor.cpp
    src-bill/classes/App.cpp
    src-bill/classes/AppBill.cpp
    src-bill/classes/BaseLogWriter.cpp
    src-bill/classes/BDb.cpp
    src-bill/classes/BDbResult.cpp
    src-bill/classes/BDbTransaction.cpp
    src-bill/classes/Billing.cpp
    src-bill/classes/Billing.cpp
    src-bill/classes/BillingCall.cpp
        src-bill/classes/BillingCall_nnp.cpp
    src-bill/classes/BlackList.cpp
    src-bill/classes/BlackListLocal.cpp
    src-bill/classes/BlackListGlobal.cpp
    src-bill/classes/BlackListTrunk.cpp
    src-bill/classes/CalcException.cpp
    src-bill/classes/CallsManager.cpp
    src-bill/classes/CdrManager.cpp
    src-bill/classes/CdrParser.cpp
    src-bill/classes/Conf.cpp
    src-bill/classes/ConfBill.cpp
    src-bill/classes/StatsAccountManager.cpp
    src-bill/classes/DbException.cpp
    src-bill/classes/Exception.cpp
    src-bill/classes/FtpClient.cpp
    src-bill/classes/StatsFreeminManager.cpp
    src-bill/classes/Loader.cpp
    src-bill/classes/Log.cpp
    src-bill/classes/Logger.cpp
    src-bill/classes/LogWriterFile.cpp
    src-bill/classes/LogWriterScreen.cpp
    src-bill/classes/LogWriterSyslog.cpp
    src-bill/classes/LogWriterGraylog.cpp
    src-bill/classes/ObjList.cpp
    src-bill/classes/RadiusAuthProcessor.cpp
    src-bill/classes/RadiusAuthServer.cpp
    src-bill/classes/StatsPackageManager.cpp
    src-bill/classes/StatsTrunkSettingsManager.cpp
    src-bill/classes/Repository.cpp
    src-bill/classes/Repository_npp.cpp
    src-bill/classes/Spinlock.cpp
    src-bill/classes/Task.cpp
    src-bill/classes/Thread.cpp
    src-bill/classes/ThreadInterrupted.cpp
    src-bill/classes/ThreadPool.cpp
    src-bill/classes/Timer.cpp
    src-bill/classes/UdpControlClient.cpp
    src-bill/classes/CdrManager.cpp
    src-bill/data/BaseData.cpp
    src-bill/data/ActiveCounterData.cpp
    src-bill/data/AirpData.cpp
    src-bill/data/ClientData.cpp
    src-bill/data/ClientLockData.cpp
    src-bill/data/CurrentCdrData.cpp
    src-bill/data/GeoData.cpp
    src-bill/data/GeoPrefixData.cpp
    src-bill/data/GlobalCountersData.cpp
    src-bill/data/InstanceSettingsData.cpp
    src-bill/data/MobPrefixData.cpp
    src-bill/data/NetworkPrefixData.cpp
    src-bill/data/NumberData.cpp
    src-bill/data/OperatorData.cpp
    src-bill/data/OrganizationData.cpp
    src-bill/data/OutcomeData.cpp
    src-bill/data/PrefixlistData.cpp
    src-bill/data/PrefixlistPrefixData.cpp
    src-bill/data/PricelistData.cpp
    src-bill/data/PricelistPriceData.cpp
    src-bill/data/ReleaseReasonData.cpp
    src-bill/data/RouteCaseData.cpp
    src-bill/data/RouteTableData.cpp
    src-bill/data/RouteTableRouteData.cpp
    src-bill/data/RoutingReportDataData.cpp
    src-bill/data/ServerData.cpp
    src-bill/data/ServiceNumberData.cpp
    src-bill/data/ServiceNumberByTechNumberData.cpp
    src-bill/data/ServiceNumberPackageData.cpp
    src-bill/data/ServiceTrunkData.cpp
    src-bill/data/ServiceTrunkPackageData.cpp
    src-bill/data/ServiceTrunkSettingsData.cpp
    src-bill/data/StatDestinationPrefixlistsData.cpp
    src-bill/data/StatPrefixlistData.cpp
    src-bill/data/TariffChangeLogData.cpp
    src-bill/data/TariffData.cpp
    src-bill/data/TariffPackageData.cpp
    src-bill/data/TrunkByAliasData.cpp
    src-bill/data/TrunkByNameData.cpp
    src-bill/data/TrunkData.cpp
    src-bill/data/TrunkGroupData.cpp
    src-bill/data/TrunkGroupItemData.cpp
    src-bill/data/TrunkNumberPreprocessingData.cpp
    src-bill/data/TrunkPriorityData.cpp
    src-bill/data/TrunkRuleData.cpp
    src-bill/data/TrunkTrunkRuleData.cpp
    src-bill/data/DataContainer.cpp
    src-bill/data/DataBillingContainer.cpp
    src-bill/data/DataCurrentCallsContainer.cpp
    src-bill/data/CurrencyRateData.cpp
    src-bill/lists/ActiveCounter.cpp
    src-bill/lists/AirpList.cpp
    src-bill/lists/ClientList.cpp
    src-bill/lists/ClientLockCounter.cpp
    src-bill/lists/CurrentCdrList.cpp
    src-bill/lists/GeoList.cpp
    src-bill/lists/GeoPrefixList.cpp
    src-bill/lists/GlobalCountersList.cpp
    src-bill/lists/InstanceSettingsList.cpp
    src-bill/lists/MobPrefixList.cpp
    src-bill/lists/NetworkPrefixList.cpp
    src-bill/lists/NumberList.cpp
    src-bill/lists/OperatorList.cpp
    src-bill/lists/OrganizationList.cpp
    src-bill/lists/OutcomeList.cpp
    src-bill/lists/PrefixlistList.cpp
    src-bill/lists/PrefixlistPrefixList.cpp
    src-bill/lists/PricelistList.cpp
    src-bill/lists/PricelistPriceList.cpp
    src-bill/lists/ReleaseReasonList.cpp
    src-bill/lists/RouteCaseList.cpp
    src-bill/lists/RouteTableList.cpp
    src-bill/lists/RouteTableRouteList.cpp
    src-bill/lists/RoutingReportDataList.cpp
    src-bill/lists/ServerList.cpp
    src-bill/lists/ServiceNumberList.cpp
    src-bill/lists/ServiceNumberByTechNumberList.cpp
    src-bill/lists/ServiceNumberPackageList.cpp
    src-bill/lists/ServiceTrunkPackageList.cpp
    src-bill/lists/ServiceTrunkList.cpp
    src-bill/lists/ServiceTrunkSettingsList.cpp
    src-bill/lists/StatDestinationPrefixlistsList.cpp
    src-bill/lists/StatPrefixlistList.cpp
    src-bill/lists/TariffChangeLogList.cpp
    src-bill/lists/TariffList.cpp
    src-bill/lists/TariffPackageList.cpp
    src-bill/lists/TrunkByAliasList.cpp
    src-bill/lists/TrunkByNameList.cpp
    src-bill/lists/TrunkList.cpp
    src-bill/lists/TrunkGroupList.cpp
    src-bill/lists/TrunkGroupItemList.cpp
    src-bill/lists/TrunkNumberPreprocessingList.cpp
    src-bill/lists/TrunkPriorityList.cpp
    src-bill/lists/TrunkRuleList.cpp
    src-bill/lists/TrunkTrunkRuleList.cpp
    src-bill/lists/CurrencyRateList.cpp
    src-bill/models/Airp.cpp
    src-bill/models/Call.cpp
    src-bill/models/CallInfo.cpp
    src-bill/models/Cdr.cpp
    src-bill/models/Client.cpp
    src-bill/models/ClientLockObj.cpp
    src-bill/models/Geo.cpp
    src-bill/models/GeoPrefix.cpp
    src-bill/models/GlobalCounters.cpp
    src-bill/models/InstanceSettings.cpp
    src-bill/models/MobPrefix.cpp
    src-bill/models/NetworkPrefix.cpp
    src-bill/models/Number.cpp
    src-bill/models/Operator.cpp
    src-bill/models/Organization.cpp
    src-bill/models/Outcome.cpp
    src-bill/models/Prefixlist.cpp
    src-bill/models/PrefixlistPrefix.cpp
    src-bill/models/Pricelist.cpp
    src-bill/models/PricelistPrice.cpp
    src-bill/models/ReleaseReason.cpp
    src-bill/models/RouteCase.cpp
    src-bill/models/RouteTable.cpp
    src-bill/models/RouteTableRoute.cpp
    src-bill/models/RoutingReportData.cpp
    src-bill/models/Server.cpp
    src-bill/models/ServiceNumber.cpp
    src-bill/models/ServicePackage.cpp
    src-bill/models/ServiceTrunk.cpp
    src-bill/models/ServiceTrunkSettings.cpp
    src-bill/models/StatDestinationPrefixlists.cpp
    src-bill/models/StatPrefixlist.cpp
    src-bill/models/StatsFreemin.cpp
    src-bill/models/StatsAccount.cpp
    src-bill/models/StatsPackage.cpp
    src-bill/models/StatsTrunkSettings.cpp
    src-bill/models/Tariff.cpp
    src-bill/models/TariffChangeLog.cpp
    src-bill/models/TariffPackage.cpp
    src-bill/models/Trunk.cpp
    src-bill/models/TrunkGroup.cpp
    src-bill/models/TrunkGroupItem.cpp
    src-bill/models/TrunkNumberPreprocessing.cpp
    src-bill/models/TrunkPriority.cpp
    src-bill/models/TrunkRule.cpp
    src-bill/models/TrunkTrunkRule.cpp
    src-bill/models/CurrencyRate.cpp
    src-bill/sync/BasePull.cpp
    src-bill/sync/ManagerPull.cpp
    src-bill/sync/PullAirp.cpp
    src-bill/sync/PullClient.cpp
    src-bill/sync/PullDefs.cpp
    src-bill/sync/PullGeo.cpp
    src-bill/sync/PullGeoPrefix.cpp
    src-bill/sync/PullInstance.cpp
    src-bill/sync/PullMobPrefix.cpp
    src-bill/sync/PullNetworkPrefix.cpp
    src-bill/sync/PullNumber.cpp
    src-bill/sync/PullOperator.cpp
    src-bill/sync/PullOrganization.cpp
    src-bill/sync/PullOutcome.cpp
    src-bill/sync/PullPrefixlist.cpp
    src-bill/sync/PullPrefixlistPrefix.cpp
    src-bill/sync/PullPricelist.cpp
    src-bill/sync/PullReleaseReason.cpp
    src-bill/sync/PullRouteCase.cpp
    src-bill/sync/PullRouteTable.cpp
    src-bill/sync/PullRouteTableRoute.cpp
    src-bill/sync/PullRoutingReportData.cpp
    src-bill/sync/PullCurrencyRate.cpp
    src-bill/sync/PullServer.cpp
    src-bill/sync/PullServiceNumber.cpp
    src-bill/sync/PullServicePackage.cpp
    src-bill/sync/PullServiceTrunk.cpp
    src-bill/sync/PullServiceTrunkSettings.cpp
    src-bill/sync/PullStatDestinationPrefixlists.cpp
    src-bill/sync/PullStatPrefixlist.cpp
    src-bill/sync/PullTariff.cpp
    src-bill/sync/PullTariffLog.cpp
    src-bill/sync/PullTariffPackage.cpp
    src-bill/sync/PullTrunk.cpp
    src-bill/sync/PullTrunkGroup.cpp
    src-bill/sync/PullTrunkGroupItem.cpp
    src-bill/sync/PullTrunkNumberPreprocessing.cpp
    src-bill/sync/PullTrunkPriority.cpp
    src-bill/sync/PullTrunkRule.cpp
    src-bill/sync/PullTrunkTrunkRule.cpp
    src-bill/threads/ThreadLog.cpp
    src-bill/threads/ThreadRadiusAuthServer.cpp
    src-bill/threads/ThreadSync.cpp
    src-bill/threads/ThreadLoader.cpp
    src-bill/threads/ThreadAccountBalanceRecalc.cpp
    src-bill/threads/ThreadRemoteLoader.cpp
    src-bill/threads/ThreadCdrParser.cpp
    src-bill/threads/ThreadCurrentCalls.cpp
    src-bill/threads/ThreadFetchCdr.cpp
    src-bill/threads/ThreadBillRuntime.cpp
    src-bill/threads/ThreadSave.cpp
    src-bill/threads/ThreadClientLock.cpp
    src-bill/threads/ThreadBlacklistFetch.cpp
    src-bill/threads/ThreadBlacklistCalc.cpp
    src-bill/threads/ThreadCheckStartTable.cpp
    src-bill/threads/ThreadLimitControl.cpp
    src-bill/threads/ThreadUpdateActiveClients.cpp
    src-bill/threads/ThreadTasks.cpp
    src-bill/threads/ThreadSyncCalls.cpp
    src-bill/threads/ThreadSyncCdrs.cpp
    src-bill/threads/ThreadSyncCounters.cpp
    src-bill/threads/ThreadSyncLocks.cpp
    src-bill/threads/ThreadWeb.cpp
    src-bill/tasks/TaskRecalc.cpp
    src-bill/web/http/mime_types.cpp
    src-bill/web/http/reply.cpp
    src-bill/web/http/request_parser.cpp
    src-bill/web/http/server.cpp
    src-bill/web/BasePage.cpp
    src-bill/web/HttpHandler.cpp
    src-bill/web/PageBlacklist.cpp
    src-bill/web/PageCalls.cpp
    src-bill/web/PageClient.cpp
    src-bill/web/PageConfig.cpp
    src-bill/web/PageCounters.cpp
    src-bill/web/PageData.cpp
    src-bill/web/PageDataBilling.cpp
    src-bill/web/PageFmins.cpp
    src-bill/web/PagePackage.cpp
    src-bill/web/PageTrunkSettingsCounters.cpp
    src-bill/web/PageNNPPackageMinuteCounters.cpp
    src-bill/web/PageHome.cpp
    src-bill/web/PageLocks.cpp
    src-bill/web/PageLog.cpp
    src-bill/web/PageSync.cpp
    src-bill/web/PageTask.cpp
    src-bill/web/PageTestCalc.cpp
    src-bill/web/PageTestCalcGui.cpp
    src-bill/web/PageTestAuth.cpp
    src-bill/web/PageCdrUnfinished.cpp src-bill/web/PageCdrUnfinished.h 

    src-bill/models/nnp/NNPRegion.cpp
    src-bill/lists/nnp/NNPRegionList.cpp
    src-bill/models/nnp/NNPDestination.cpp
    src-bill/lists/nnp/NNPDestinationList.cpp
    src-bill/models/nnp/NNPOperator.cpp
    src-bill/lists/nnp/NNPOperatorList.cpp
    src-bill/models/nnp/NNPPrefix.cpp
    src-bill/lists/nnp/NNPPrefixList.cpp
    src-bill/models/nnp/NNPNumberRange.cpp
    src-bill/lists/nnp/NNPNumberRangeList.cpp
    src-bill/models/nnp/NNPNumberRangePrefix.cpp
    src-bill/lists/nnp/NNPNumberRangePrefixList.cpp
    src-bill/models/nnp/NNPPrefixDestination.cpp
    src-bill/lists/nnp/NNPPrefixDestinationList.cpp
    src-bill/models/nnp/NNPPackage.cpp
    src-bill/lists/nnp/NNPPackageList.cpp
    src-bill/models/nnp/NNPPackageMinute.cpp
    src-bill/lists/nnp/NNPPackageMinuteList.cpp
    src-bill/models/nnp/NNPPackagePrice.cpp
    src-bill/lists/nnp/NNPPackagePriceList.cpp
    src-bill/models/nnp/NNPPackagePricelist.cpp
    src-bill/lists/nnp/NNPPackagePricelistList.cpp

    src-bill/models/nnp/NNPAccountTariffLight.cpp
    src-bill/lists/nnp/NNPAccountTariffLightList.cpp

    src-bill/data/nnp/NNPDestinationData.cpp
    src-bill/data/nnp/NNPNumberRangeData.cpp
    src-bill/data/nnp/NNPNumberRangePrefixData.cpp
    src-bill/data/nnp/NNPOperatorData.cpp
    src-bill/data/nnp/NNPPackageData.cpp
    src-bill/data/nnp/NNPPackageMinuteData.cpp
    src-bill/data/nnp/NNPPackagePriceData.cpp
    src-bill/data/nnp/NNPPackagePricelistData.cpp
    src-bill/data/nnp/NNPPrefixDestinationData.cpp
    src-bill/data/nnp/NNPPrefixData.cpp
    src-bill/data/nnp/NNPRegionData.cpp

    src-bill/sync/nnp/NNPDestinationPull.cpp 
    src-bill/sync/nnp/NNPNumberRangePull.cpp 
    src-bill/sync/nnp/NNPNumberRangePrefixPull.cpp 
    src-bill/sync/nnp/NNPOperatorPull.cpp 
    src-bill/sync/nnp/NNPPackagePull.cpp 
    src-bill/sync/nnp/NNPPackageMinutePull.cpp 
    src-bill/sync/nnp/NNPPackagePricePull.cpp 
    src-bill/sync/nnp/NNPPackagePricelistPull.cpp 
    src-bill/sync/nnp/NNPPrefixPull.cpp 
    src-bill/sync/nnp/NNPPrefixDestinatioPull.cpp 
    src-bill/sync/nnp/NNPRegionPull.cpp 
    src-bill/data/nnp/NNPAccountTariffLightData.cpp 
    src-bill/sync/nnp/NNPAccountTariffLightPull.cpp 

    src-bill/models/StatsNNPPackageMinute.cpp
    src-bill/classes/StatsNNPPackageMinuteManager.cpp
    src-bill/classes/nnp/NNPResolver.cpp

    src-bill/main.cpp

    src-bill/models/GlobalNNPPackageMinuteCounters.cpp
    src-bill/data/GlobalNNPPackageMinuteCountersData.cpp
    src-bill/lists/GlobalNNPPackageMinuteCountersList.cpp
    src-bill/models/nnp/NNPNumberRangeTreeNode.cpp
    src-bill/models/Hub.cpp
    src-bill/lists/HubList.cpp
    src-bill/data/HubData.cpp
    src-bill/sync/PullHub.cpp
    src-bill/sync/PullHub.h
    )

add_custom_target(invalidate_files ALL
        COMMAND ${CMAKE_COMMAND} -E touch src-bill/version.cpp)

add_definitions(-DHAVE_SYS_IOCTL_H -DHAVE_SOCKLEN_T)
file(GLOB_RECURSE ACE_RADIUS_SOURCES "libs/ace-radius/*.cpp")

add_executable(app_bill ${BILL_SOURCES} ${BOOST_SOURCES} ${JSON_SOURCES} ${ACE_RADIUS_SOURCES})
target_link_libraries(app_bill rt pthread pq curl crypto)

#add_executable(auth-client src-auth-client/client.cpp ${BOOST_SOURCES} ${ACE_RADIUS_SOURCES})

#add_executable(auth-server src-auth-client/server.cpp ${BOOST_SOURCES} ${ACE_RADIUS_SOURCES})


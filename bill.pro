QT  -=  core
QT  -=  gui

TARGET = bill
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += "/usr/pgsql-9.1/include"
LIBS += -L"/usr/pgsql-9.1/lib" -lpq
LIBS += -lrt


INCLUDEPATH += "/home/km/projects/bill/libs/boost"
INCLUDEPATH += "/home/km/projects/bill/libs/gtest/include"

SOURCES += \
    libs/boost/libs/program_options/src/winmain.cpp \
    libs/boost/libs/program_options/src/variables_map.cpp \
    libs/boost/libs/program_options/src/value_semantic.cpp \
    libs/boost/libs/program_options/src/utf8_codecvt_facet.cpp \
    libs/boost/libs/program_options/src/split.cpp \
    libs/boost/libs/program_options/src/positional_options.cpp \
    libs/boost/libs/program_options/src/parsers.cpp \
    libs/boost/libs/program_options/src/options_description.cpp \
    libs/boost/libs/program_options/src/convert.cpp \
    libs/boost/libs/program_options/src/config_file.cpp \
    libs/boost/libs/program_options/src/cmdline.cpp \
    libs/boost/libs/filesystem/src/windows_file_codecvt.cpp \
    libs/boost/libs/filesystem/src/unique_path.cpp \
    libs/boost/libs/filesystem/src/portability.cpp \
    libs/boost/libs/filesystem/src/path_traits.cpp \
    libs/boost/libs/filesystem/src/path.cpp \
    libs/boost/libs/filesystem/src/operations.cpp \
    libs/boost/libs/filesystem/src/codecvt_error_category.cpp \
    libs/boost/libs/system/src/error_code.cpp \
    src/http/server.cpp \
    src/http/request_parser.cpp \
    src/http/reply.cpp \
    src/http/mime_types.cpp \
    src/http/file_handler.cpp \
    libs/boost/libs/thread/src/pthread/thread.cpp \
    libs/boost/libs/thread/src/pthread/once.cpp \
    libs/boost/libs/thread/src/tss_null.cpp \
    libs/boost/libs/thread/src/future.cpp \
    src/tasks/TaskCurrentCalls.cpp \
    src/tasks/TaskAsteriskInfo.cpp \
    src/tasks/Task.cpp \
    src/main.cpp \
    src/common.cpp \
    src/Log.cpp \
    src/BDb.cpp \
    src/tasks/TaskLoader.cpp \
    src/tests/Test.cpp \
    src/tests/TestLibs.cpp \
    libs/gtest/src/gtest-all.cc \
    src/Conf.cpp \
    libs/boost/libs/regex/src/winstances.cpp \
    libs/boost/libs/regex/src/wide_posix_api.cpp \
    libs/boost/libs/regex/src/wc_regex_traits.cpp \
    libs/boost/libs/regex/src/w32_regex_traits.cpp \
    libs/boost/libs/regex/src/usinstances.cpp \
    libs/boost/libs/regex/src/static_mutex.cpp \
    libs/boost/libs/regex/src/regex_traits_defaults.cpp \
    libs/boost/libs/regex/src/regex_raw_buffer.cpp \
    libs/boost/libs/regex/src/regex_debug.cpp \
    libs/boost/libs/regex/src/regex.cpp \
    libs/boost/libs/regex/src/posix_api.cpp \
    libs/boost/libs/regex/src/instances.cpp \
    libs/boost/libs/regex/src/icu.cpp \
    libs/boost/libs/regex/src/fileiter.cpp \
    libs/boost/libs/regex/src/c_regex_traits.cpp \
    libs/boost/libs/regex/src/cregex.cpp \
    libs/boost/libs/regex/src/cpp_regex_traits.cpp \
    src/tasks/TaskYotaQoS.cpp \
    src/tasks/TaskSync.cpp \
    src/App.cpp \
    src/tasks/TaskWeb.cpp \
    src/tests/TestLoader.cpp \
    src/lists/DestObjList.cpp \
    src/lists/ObjList.cpp \
    src/lists/UsageObjList.cpp \
    src/lists/ClientObjList.cpp \
    src/lists/CallsObjList.cpp \
    src/lists/ClientCounter.cpp \
    src/lists/FminCounter.cpp \
    src/lists/CurrentCallsObjList.cpp \
    src/lists/AsteriskNumberObjList.cpp \
    src/DataLoader.cpp \
    src/CalcBase.cpp \
    src/lists/PriceObjList.cpp \
    src/CalcFull.cpp \
    src/CallsSaver.cpp \
    src/BlackList.cpp \
    src/UdpControlClient.cpp \
    src/lists/UsageRawList.cpp \
    src/lists/PhoneList.cpp \
    src/lists/OperatorList.cpp




HEADERS += \
    src/BDb.h \
    src/lists/UsageObjList.h \
    src/lists/PriceObjList.h \
    src/lists/ObjList.h \
    src/lists/Obj.h \
    src/Log.h \
    src/lists/DestObjList.h \
    src/DataLoader.h \
    src/lists/ClientObjList.h \
    src/CallsSaver.h \
    src/lists/CallsObjList.h \
    src/CalcFull.h \
    src/CalcBase.h \
    src/lists/ClientCounter.h \
    src/CounterLoader.h \
    src/lists/FminCounter.h \
    src/KillCalls.h \
    src/UdpControlClient.h \
    libs/boost/libs/filesystem/src/windows_file_codecvt.hpp \
    libs/boost/libs/system/src/local_free_on_destruction.hpp \
    src/Timer.h \
    src/lists/AsteriskNumberObjList.h \
    src/lists/CurrentCallsObjList.h \
    src/common.h \
    src/http/yield.hpp \
    src/http/unyield.hpp \
    src/http/server.hpp \
    src/http/request_parser.hpp \
    src/http/request.hpp \
    src/http/reply.hpp \
    src/http/mime_types.hpp \
    src/http/header.hpp \
    src/http/file_handler.hpp \
    src/http/coroutine.hpp \
    src/App.h \
    src/tasks/TaskYotaQoS.h \
    src/tasks/TaskWeb.h \
    src/tasks/TaskSync.h \
    src/tasks/TaskLoader.h \
    src/tasks/TaskLimitControl.h \
    src/tasks/TaskCurrentCalls.h \
    src/tasks/TaskCheckStartTable.h \
    src/tasks/TaskBillRuntime.h \
    src/tasks/TaskAsteriskInfo.h \
    src/tasks/Task.h \
    src/DbException.h \
    src/tests/Test.h \
    src/Conf.h \
    libs/boost/libs/regex/src/internals.hpp \
    src/Loader.h \
    src/BlackList.h \
    src/tasks/TaskBlacklist.h \
    src/tasks/TaskSaveCounters.h \
    src/lists/UsageRawList.h \
    src/lists/PhoneList.h \
    src/lists/OperatorList.h

OTHER_FILES +=


#include "Test.h"

#include <string>
#include <vector>
using namespace std;


#include <boost/algorithm/string.hpp>
TEST(libs,string) {
    string s = "123456123";
    EXPECT_EQ( s.find("111"), s.npos );
    EXPECT_EQ( s.find("111"), string::npos );
    EXPECT_NE( s.find("456"), string::npos );
    EXPECT_EQ( s.find("456"), (size_t)3 );

    boost::algorithm::replace_all(s,"123","321");
    EXPECT_EQ( s, "321456321" );
}

#include <boost/lexical_cast.hpp>
TEST(libs,lexical_cast) {
    EXPECT_EQ( boost::lexical_cast<string>(9223372036854775807), "9223372036854775807" );
    EXPECT_EQ( boost::lexical_cast<long long int>("9223372036854775807"), 9223372036854775807 );
}

/*
#include <boost/regex.hpp>
TEST(libs,regexp) {
    boost::regex exp(".*\"sfId\":\"([\\w\\d\\.-]+)\".*");
    boost::smatch res;

    string str1 = "{\"qosFeatureData\":{\"sfId\":\"yota.ru.msk.lte.05712857-cd45-4163-a41f-39178ce9cc67\",\"operatorId\":\"yota.ru.msk.lte\",\"qosFeatureId\":\"RtpAudio\",\"qosFeatureProps\":{\"duration\":3600,\"maxUBw\":99,\"maxDBw\":99,\"mediaProps\":[{\"usrIp\":\"89.235.181.6\",\"usrPort\":null,\"srvIp\":\"85.94.32.244\",\"srvPort\":null,\"protocolType\":null}]}}}";
    string str2 = "{\"qosFeatureData\":{\"sfIdXXX\":\"yota.ru.msk.lte.05712857-cd45-4163-a41f-39178ce9cc67\",\"operatorId\":\"yota.ru.msk.lte\",\"qosFeatureId\":\"RtpAudio\",\"qosFeatureProps\":{\"duration\":3600,\"maxUBw\":99,\"maxDBw\":99,\"mediaProps\":[{\"usrIp\":\"89.235.181.6\",\"usrPort\":null,\"srvIp\":\"85.94.32.244\",\"srvPort\":null,\"protocolType\":null}]}}}";

    EXPECT_EQ( boost::regex_match(str1, exp), true );
    EXPECT_EQ( boost::regex_match(str2, exp), false );
    EXPECT_EQ( boost::regex_match(str2, res, exp), false );
    EXPECT_EQ( boost::regex_match(str1, res, exp), true );
    EXPECT_EQ( string(res[1]), "yota.ru.msk.lte.05712857-cd45-4163-a41f-39178ce9cc67" );
}
*/

#include <boost/algorithm/string/split.hpp>
TEST(libs,split) {
    string s = "123,456,789";
    vector<string> ss;
    boost::algorithm::split(ss, s, boost::algorithm::is_any_of(","));
    EXPECT_EQ( ss.size(), (size_t)3 );
}

#include <boost/algorithm/string/join.hpp>
TEST(libs,join) {
    vector<string> ss;
    ss.push_back("123");
    ss.push_back("456");
    ss.push_back("789");
    string s = boost::algorithm::join(ss, ",");
    EXPECT_EQ( s, "123,456,789" );
}

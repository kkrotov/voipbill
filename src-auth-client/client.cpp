#include <iostream>
#include "../libs/ace-radius/RadiusClientStack.h"
#include <boost/program_options.hpp>
namespace po = boost::program_options;
using namespace std;

string host         = "localhost";
uint16_t port       = 1812;
string srcNumber    = "74996850003";
string dstNumber    = "74957757575";
string trunk        = "MCN_Transit_99";
string nas          = "85.94.32.190";
string srcNoa       = "3";
string dstNoa       = "3";
string secret       = "427a7663dbd035c5f7611c9545e9d0";
unsigned char requestId = 0;


bool parse_command_line(int argc, const char * const *argv)
{
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "produce help message")
                ("host", po::value<string>())
                ("port", po::value<uint16_t>())
                ("src", po::value<string>())
                ("dst", po::value<string>())
                ("trunk", po::value<string>())
                ("src-noa", po::value<int>())
                ("dst-noa", po::value<int>())
                ("nas", po::value<string>())
                ("secret", po::value<string>())
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);


        if (vm.count("help")) {
            cout << desc << "\n";
            return false;
        }

        if (vm.count("host")) {
            host = vm["host"].as<string>();
        }

        if (vm.count("port")) {
            port = vm["port"].as<int>();
        }

        if (vm.count("src")) {
            srcNumber = vm["src"].as<string>();
        }

        if (vm.count("dst")) {
            dstNumber = vm["dst"].as<string>();
        }

        if (vm.count("trunk")) {
            trunk = vm["trunk"].as<string>();
        }

        if (vm.count("src_noa")) {
            srcNoa = vm["src_noa"].as<string>();
        }

        if (vm.count("dst_noa")) {
            dstNoa = vm["dst_noa"].as<string>();
        }

        if (vm.count("nas")) {
            nas = vm["nas"].as<string>();
        }

        if (vm.count("secret")) {
            secret = vm["secret"].as<string>();
        }

    } catch (exception& e) {
        cout << "ERROR: Parse cmd line: " << e.what() << endl;
        return false;
    }
}
int main (int argc, const char * const *argv)
{
    if (!parse_command_line(argc, argv)) {
        return 1;
    }

    // create instance of client stack
    RadiusClientStack l_stack;
    if (!l_stack.isValid())
    {
        printf("Can not create RADIUS client stack\n");
        return 1;
    }

    // add connection with some RADIUS server
    l_stack.addServer(host.c_str(), port, RadiusSecret(secret.c_str()));

    // create a new packet
    RadiusPacket l_packet(D_PACKET_ACCESS_REQUEST, l_stack);
    l_packet.setID(++requestId);

    // add some attributes to the packet
    RadiusAttribute l_attr;

    l_packet.addAttribute(D_ATTR_USER_NAME, l_attr);
    l_attr.setString(srcNumber.c_str());

    l_packet.addAttribute(D_ATTR_USER_PASSWORD, l_attr);
    l_attr.setUserPassword("password");

    struct in_addr l_addr;
    inet_aton(nas.c_str(), &l_addr);
    l_packet.addAttribute(D_ATTR_NAS_IP_ADDRESS, l_attr);
    l_attr.setIPAddress(l_addr);

    l_packet.addAttribute(D_ATTR_SERVICE_TYPE, l_attr);
    l_attr.setNumber(8);

    l_packet.addAttribute(D_ATTR_CALLING_STATION_ID, l_attr);
    l_attr.setString(srcNumber.c_str());

    l_packet.addAttribute(D_ATTR_CALLED_STATION_ID, l_attr);
    l_attr.setString(dstNumber.c_str());

    l_packet.addAttribute(D_ATTR_VENDOR_SPECIFIC, l_attr);
    l_attr.setVendorType(1);
    l_attr.setVendorId(9);
    l_attr.setVendorString(("INCOMING-CALLING-ADDRESS-NOA=" + srcNoa).c_str());

    l_packet.addAttribute(D_ATTR_VENDOR_SPECIFIC, l_attr);
    l_attr.setVendorType(1);
    l_attr.setVendorId(9);
    l_attr.setVendorString(("INCOMING-CALLED-ADDRESS-NOA=" + srcNoa).c_str());

    l_packet.addAttribute(D_ATTR_VENDOR_SPECIFIC, l_attr);
    l_attr.setVendorType(1);
    l_attr.setVendorId(9);
    l_attr.setVendorString(("INCOMING-ROUTEID=" + trunk).c_str());


    l_packet.dump();

    // send the packet
    RadiusPacket * l_response = l_stack.sendPacket(l_packet);

    // print response if it was received
    if (l_response)
    {
        printf("---\nPacket was successfully transmitted\n---\n");
        printf("\nResponse packet:\n");
        l_response->dump();
    }
    else
        printf("---\nPacket transmit failure\n---\n");

    return 0;
}

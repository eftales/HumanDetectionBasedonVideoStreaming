#ifndef __GETNICMSG__
#define __GETNICMSG__

#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib") 
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)
using namespace std;



#define ADAPTERNUM 20
#define MIB_IF_TYPE_WiFi 71
#define UeslessIPaddr "0.0.0.0"
int getAllAdapterInfo(char** ipaddr, char **netmask, int NICtype, bool printit, int sequence);
void getAllAdapterInfo();
void GetMsgfromcur(char** ipaddr, char **netmask, PIP_ADAPTER_INFO cur, bool printit);
#endif
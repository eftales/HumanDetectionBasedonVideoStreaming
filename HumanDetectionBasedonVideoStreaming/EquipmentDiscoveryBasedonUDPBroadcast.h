/*
！！！！功能说明
1. 上电后，设备查询当前可用的无线网卡，在无线网卡上工作
2. 找到无线网卡之后，进入设备发现状态，时间为10s
2. 在设备发现状态下每0.5s广播一次自己的ID，同时使用select不间断的监听外来ID
3. 绑定的本地端口为2019


*/
#ifndef __EquipmentDiscoveryBasedonUDPBroadcast__
#define __EquipmentDiscoveryBasedonUDPBroadcast__
#include<stdio.h>
#include<WinSock2.h>
#include <Ws2tcpip.h>
#include<getopt.h>

#include "GetNICMsg.h"

#pragma comment(lib,"getopt.lib")
#pragma comment(lib,"ws2_32.lib")

#pragma warning(disable:4996)
#define RECV_PORT 2019
#define SEND_PORT 2019
#define MAXBUFLEN 1024

#define DONTPRINTIT 0
#define PRINTIT 1

#define FIRST_USING_WIRELESS_NIC 0


#define TOTALWAITTIME 10	// 等待10s 如果没有达到31，则退出程序
#define ECHOINTERVAL 0.5	// 在等待10s的过程中，每0.5s发送一次问候报文，通告自己的消息





struct socket_list{
	SOCKET MainSock;
	int num;
	SOCKET sock_array[64];
};
void make_fdlist(socket_list *list, fd_set *fd_list);
void init_list(socket_list *list);
void insert_list(SOCKET s, socket_list *list);
void delete_list(SOCKET s, socket_list *list);
void raise_message(char *msg);
char * changeIPaddrtoBroadcastIPaddr(char *ipaddr, char *netmask);
int EquipmentDiscovery(SOCKET sock_host, int ID, int  NICnum, int Printit, char **ipaddr, char ** netmask, char *ipaddr_broadcast, sockaddr_in  *addr_remote);
int setUDPBroadcast(SOCKET sock_host);
#endif
/*
������������˵��
1. �ϵ���豸��ѯ��ǰ���õ����������������������Ϲ���
2. �ҵ���������֮�󣬽����豸����״̬��ʱ��Ϊ10s
2. ���豸����״̬��ÿ0.5s�㲥һ���Լ���ID��ͬʱʹ��select����ϵļ�������ID
3. �󶨵ı��ض˿�Ϊ2019


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


#define TOTALWAITTIME 10	// �ȴ�10s ���û�дﵽ31�����˳�����
#define ECHOINTERVAL 0.5	// �ڵȴ�10s�Ĺ����У�ÿ0.5s����һ���ʺ��ģ�ͨ���Լ�����Ϣ





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
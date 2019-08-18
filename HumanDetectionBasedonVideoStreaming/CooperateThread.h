#ifndef __CooperateThread__
#define __CooperateThread__

#define EXITMSG "exit"
#define STOPMSG "stop"
#define STARTMSG "start"

#include<pthread.h>
#include<stdlib.h>
#include "EquipmentDiscoveryBasedonUDPBroadcast.h"

#pragma comment(lib,"pthread.lib")
void CreatThreadVal(pthread_t *pid, pthread_attr_t *attr);
void * Synchronization(void *arg);
int BroadcastExitMsg(SOCKET sock_host, sockaddr_in *addr_remote);
int BroadcastStartMsg(SOCKET sock_host, sockaddr_in *addr_remote);
int BroadcastStopMsg(SOCKET sock_host, sockaddr_in *addr_remote);

typedef struct mypara
{
	SOCKET sock_host;
	sockaddr_in *addr_remote;
	int ID;
}mypara;


#endif
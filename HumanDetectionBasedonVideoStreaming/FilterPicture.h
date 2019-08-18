#ifndef __FilterPicture__
#define __FilterPicture__

#include<stdio.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

#define SENDPICTUREPORT 2020
#define SENDPICTUREIP "127.0.0.1"
#define CONVINCEDPICTURENUM 7

#define IMGNAME "%d_%d.png"
int filter_picture(int ID, int picture_num_start, int picture_num_current, SOCKET sock_host, sockaddr_in *addr_remote_sendpicture);

#endif
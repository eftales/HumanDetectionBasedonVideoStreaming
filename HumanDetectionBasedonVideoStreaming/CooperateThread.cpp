#include"CooperateThread.h"

/**
* @file CooperateThread.cpp
* @brief 定义不同程序之间协作的函数
* @details 包含创建线程的函数、广播退出、广播开始存储图片、广播停止存储图片、和指令分析程序
*/
int SaveFrame = 0; ///< 有3个值，为0表示不存储图片，1和2都要存储图片，为1表示本线程检测到了运动，需要其他进程配合， 此时忽略stop信号，为2表示自己是配合其他进程的，可以收到stop信号，忽略自己内部动作检测
bool StopWork = 0; ///< 有两个值，为0表示不退出程序，为1表示退出程序
extern int SendPicture;///< 为0表示不发送图片，为1表示发送图片

/**
* @brief 创建线程需要的参数
* @details 本工程使用pthread作为线程库，启动线程必定需要pthread_t *pid, pthread_attr_t *attr这两个参数
* @param [inout] pid pthread_t*类型变量
* @param [inout] attr pthread_attr_t*类型变量
* @return void 无返回值
*/

void CreatThreadVal(pthread_t *pid, pthread_attr_t *attr)
{
	pid = (pthread_t *)malloc(sizeof(pthread_t));
	attr = (pthread_attr_t *)malloc(sizeof(pthread_attr_t));

}

/**
* @brief 协作线程运行的主函数
* @details 这个线程为一个阻塞式UDP服务器，接受来自其他进程的指令
* @param [in] arg void*类型变量 传入的是一个指针，需要根据事先准备好的协议进行强制类型转换

* @return void* 不采用返回值
	
* @see 关于强制类型转换的目的类型参考 mypara 

* @ref CooperateThread.h

*/


void * Synchronization(void *arg)
{

	mypara *pstru;
	pstru = (struct mypara *) arg;
	SOCKET sock_host = pstru->sock_host;
	sockaddr_in *addr_remote = pstru->addr_remote;
	int ID = pstru->ID;

	char buff[1024];
	int retval;
	while (true)
	{
		printf("UDP服务器开启...\n");
		retval = recvfrom(sock_host, buff, 1024, 0, NULL, NULL);
		printf("UDP服务器收到数据...\n");
		if (retval <= 0)
		{
			retval = WSAGetLastError();
			printf("服务端socket异常\n code:%d", retval);
			return NULL;
		}
		printf(buff);
		if (strcmp(buff, "start") == 0)
		{
			// 为了避免死锁进行如下设计
			/*
			ID = 1 的设备在自己的SaveFrame = 1的情况下收到"start"信号，自己的SaveFrame仍然为1
			其他设备收到"start"信号直接改为2
			*/
			if (ID == 1 && SaveFrame == 1)
				SaveFrame = 1;
			else
			{
				SendPicture = 0;
				SaveFrame = 2;
			}
				

		}
		if (strcmp(buff, "stop") == 0)
		{
			SaveFrame = 3;

		}
		if (strcmp(buff, "exit") == 0)
		{
			StopWork = 1;
			SaveFrame = 0;
			
			break;
		}
	}



	return 0;

}
/**
* @brief 广播程序退出消息
* @details 系统运行过程中，如果一个进程请求退出程序，则所有程序都要配合退出程序
* @param [in] sock_host 在sock_host 上发送广播报文
* @param [in] addr_remote 报文的目的地址
* @return 返回值表示程序运行情况
*	@retval 0 成功运行
*	@retval -1 发送失败

*/
int BroadcastExitMsg(SOCKET sock_host, sockaddr_in *addr_remote)
{
	int headlen = sizeof(sockaddr);
	int retval = sendto(sock_host, EXITMSG, strlen(EXITMSG), 0, (sockaddr *)addr_remote, headlen);
	if (retval < 0)
	{
		printf("exit消息发送失败 error code: %d\n", WSAGetLastError());
		return -1;
	}
	return 0;
}

/**
* @brief 广播停止存储图片消息
* @details 系统运行过程中，本进程检测到了有效帧即将结束，则广播停止存储的消息
* @param [in] sock_host 在sock_host 上发送广播报文
* @param [in] addr_remote 报文的目的地址
* @return 返回值表示程序运行情况
*	@retval 0 成功运行
*	@retval -1 发送失败
*/

int BroadcastStopMsg(SOCKET sock_host, sockaddr_in *addr_remote)
{
	int headlen = sizeof(sockaddr);
	int retval = sendto(sock_host, STOPMSG, strlen(STOPMSG), 0, (sockaddr *)addr_remote, headlen);
	if (retval < 0)
	{
		printf("exit消息发送失败 error code: %d\n", WSAGetLastError());
		return -1;
	}
	return 0;
}

/**
* @brief 广播开始存储图片消息
* @details 系统运行过程中，本进程检测到了有效帧即将开始，则广播开始存储图片的消息
* @param [in] sock_host 在sock_host 上发送广播报文
* @param [in] addr_remote 报文的目的地址
* @return 返回值表示程序运行情况
*	@retval 0 成功运行
*	@retval -1 发送失败
*/


int BroadcastStartMsg(SOCKET sock_host, sockaddr_in *addr_remote)
{
	int headlen = sizeof(sockaddr);
	int retval = sendto(sock_host, STARTMSG, strlen(STARTMSG), 0, (sockaddr *)addr_remote, headlen);
	if (retval < 0)
	{
		printf("exit消息发送失败 error code: %d\n", WSAGetLastError());
		return -1;
	}
	return 0;
}
#include"CooperateThread.h"

/**
* @file CooperateThread.cpp
* @brief ���岻ͬ����֮��Э���ĺ���
* @details ���������̵߳ĺ������㲥�˳����㲥��ʼ�洢ͼƬ���㲥ֹͣ�洢ͼƬ����ָ���������
*/
int SaveFrame = 0; ///< ��3��ֵ��Ϊ0��ʾ���洢ͼƬ��1��2��Ҫ�洢ͼƬ��Ϊ1��ʾ���̼߳�⵽���˶�����Ҫ����������ϣ� ��ʱ����stop�źţ�Ϊ2��ʾ�Լ�������������̵ģ������յ�stop�źţ������Լ��ڲ��������
bool StopWork = 0; ///< ������ֵ��Ϊ0��ʾ���˳�����Ϊ1��ʾ�˳�����
extern int SendPicture;///< Ϊ0��ʾ������ͼƬ��Ϊ1��ʾ����ͼƬ

/**
* @brief �����߳���Ҫ�Ĳ���
* @details ������ʹ��pthread��Ϊ�߳̿⣬�����̱߳ض���Ҫpthread_t *pid, pthread_attr_t *attr����������
* @param [inout] pid pthread_t*���ͱ���
* @param [inout] attr pthread_attr_t*���ͱ���
* @return void �޷���ֵ
*/

void CreatThreadVal(pthread_t *pid, pthread_attr_t *attr)
{
	pid = (pthread_t *)malloc(sizeof(pthread_t));
	attr = (pthread_attr_t *)malloc(sizeof(pthread_attr_t));

}

/**
* @brief Э���߳����е�������
* @details ����߳�Ϊһ������ʽUDP�����������������������̵�ָ��
* @param [in] arg void*���ͱ��� �������һ��ָ�룬��Ҫ��������׼���õ�Э�����ǿ������ת��

* @return void* �����÷���ֵ
	
* @see ����ǿ������ת����Ŀ�����Ͳο� mypara 

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
		printf("UDP����������...\n");
		retval = recvfrom(sock_host, buff, 1024, 0, NULL, NULL);
		printf("UDP�������յ�����...\n");
		if (retval <= 0)
		{
			retval = WSAGetLastError();
			printf("�����socket�쳣\n code:%d", retval);
			return NULL;
		}
		printf(buff);
		if (strcmp(buff, "start") == 0)
		{
			// Ϊ�˱������������������
			/*
			ID = 1 ���豸���Լ���SaveFrame = 1��������յ�"start"�źţ��Լ���SaveFrame��ȻΪ1
			�����豸�յ�"start"�ź�ֱ�Ӹ�Ϊ2
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
* @brief �㲥�����˳���Ϣ
* @details ϵͳ���й����У����һ�����������˳����������г���Ҫ����˳�����
* @param [in] sock_host ��sock_host �Ϸ��͹㲥����
* @param [in] addr_remote ���ĵ�Ŀ�ĵ�ַ
* @return ����ֵ��ʾ�����������
*	@retval 0 �ɹ�����
*	@retval -1 ����ʧ��

*/
int BroadcastExitMsg(SOCKET sock_host, sockaddr_in *addr_remote)
{
	int headlen = sizeof(sockaddr);
	int retval = sendto(sock_host, EXITMSG, strlen(EXITMSG), 0, (sockaddr *)addr_remote, headlen);
	if (retval < 0)
	{
		printf("exit��Ϣ����ʧ�� error code: %d\n", WSAGetLastError());
		return -1;
	}
	return 0;
}

/**
* @brief �㲥ֹͣ�洢ͼƬ��Ϣ
* @details ϵͳ���й����У������̼�⵽����Ч֡������������㲥ֹͣ�洢����Ϣ
* @param [in] sock_host ��sock_host �Ϸ��͹㲥����
* @param [in] addr_remote ���ĵ�Ŀ�ĵ�ַ
* @return ����ֵ��ʾ�����������
*	@retval 0 �ɹ�����
*	@retval -1 ����ʧ��
*/

int BroadcastStopMsg(SOCKET sock_host, sockaddr_in *addr_remote)
{
	int headlen = sizeof(sockaddr);
	int retval = sendto(sock_host, STOPMSG, strlen(STOPMSG), 0, (sockaddr *)addr_remote, headlen);
	if (retval < 0)
	{
		printf("exit��Ϣ����ʧ�� error code: %d\n", WSAGetLastError());
		return -1;
	}
	return 0;
}

/**
* @brief �㲥��ʼ�洢ͼƬ��Ϣ
* @details ϵͳ���й����У������̼�⵽����Ч֡������ʼ����㲥��ʼ�洢ͼƬ����Ϣ
* @param [in] sock_host ��sock_host �Ϸ��͹㲥����
* @param [in] addr_remote ���ĵ�Ŀ�ĵ�ַ
* @return ����ֵ��ʾ�����������
*	@retval 0 �ɹ�����
*	@retval -1 ����ʧ��
*/


int BroadcastStartMsg(SOCKET sock_host, sockaddr_in *addr_remote)
{
	int headlen = sizeof(sockaddr);
	int retval = sendto(sock_host, STARTMSG, strlen(STARTMSG), 0, (sockaddr *)addr_remote, headlen);
	if (retval < 0)
	{
		printf("exit��Ϣ����ʧ�� error code: %d\n", WSAGetLastError());
		return -1;
	}
	return 0;
}
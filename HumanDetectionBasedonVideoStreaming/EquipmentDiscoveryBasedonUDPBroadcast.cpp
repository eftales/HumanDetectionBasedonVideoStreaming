#include "EquipmentDiscoveryBasedonUDPBroadcast.h"
/**
* @file EquipmentDiscoveryBasedonUDPBroadcast.cpp
* @brief �����豸���֡�UDP�㲥��ַ���õȺ���

*/

int EQUIPMENTNUM = 2;	///< �豸���������ļ�EquipmentDiscoveryBasedonUDPBroadcast.cpp�ж���
int ALLALIVE = 3;			///< ���豸���������Ƶ�����ֵ�����ļ�EquipmentDiscoveryBasedonUDPBroadcast.cpp�ж���,�����豸����Ϊ3 ��ô�豸��ŷֱ�Ϊ001 010 100 ����ALLALIVE��ֵΪ111=7

/**
* @brief ʵ���豸���ֵ��߼�������UDP�㲥
* 
* @param [in] sock_host ���뱾��ʹ�õġ����Թ㲥��UDP�׽���������
* @param [in] ID ���뱾����ID��IDΪUDP�㲥ʱ����غ�
* @param [in] NICnum ����ϣ������һ�����������Ϸ���
* @param [in] Printit �����Ƿ�ϣ����ӡ������Ϣ
* @param [out] ipaddr ��IP��ַ���ַ������ͣ��������
* @param [out] netmask ѡ����������IP�㲥��ַ���ַ������ͣ��������
* @param [inout] ipaddr_broadcast ѡ����������IP�㲥��ַ���ַ������ͣ��������ΪNULL��NICnum������Ч����NICnum��Ӧ��IP�㲥��ַ���룻�����ΪNULL����ֱ��ʹ�ø�ֵ��Ϊ�㲥��Ŀ�ĵ�ַ
* @param [out] addr_remote Զ�˵�ַ��sock_addr_in ���ͣ������������ipaddr_broadcast����NICnum����õ�
* @return �����Ƿ�ɹ�����
*	@retval 0 ����ִ�гɹ�
*	@retval else ����ִ��ʧ��
*/

int EquipmentDiscovery(SOCKET sock_host, int ID, int  NICnum, int Printit, char **ipaddr, char ** netmask, char *ipaddr_broadcast, sockaddr_in  *addr_remote)
{
	int all_member = 0;

	all_member = all_member | ID;
	if (all_member == 0)
	{
		raise_message("û�������豸��ID\n���������в����� -i X X��ʾ1 - 5��������\n");
		return -1;
	}
		
	if (ipaddr_broadcast == NULL)
	{
		
		if (getAllAdapterInfo(ipaddr, netmask, MIB_IF_TYPE_WiFi, DONTPRINTIT, FIRST_USING_WIRELESS_NIC) != 0) {
			raise_message("�Ҳ���һ�����õ���������\n");
			closesocket(sock_host);
			return -1;
		}

		// ������������Ĺ㲥��ַ
		changeIPaddrtoBroadcastIPaddr(*ipaddr,*netmask);

	}
	else
	{
		*ipaddr = ipaddr_broadcast;
	}
		

	// ���÷��͵�ַ
	addr_remote->sin_addr.S_un.S_addr = inet_addr(*ipaddr);
	addr_remote->sin_port = htons(SEND_PORT);
	addr_remote->sin_family = AF_INET;




	// ����select
	int endtime = TOTALWAITTIME / ECHOINTERVAL;
	int others_ID_int;
	struct socket_list sock_list;

	init_list(&sock_list);

	fd_set readfds, writefds, exceptfds;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
	sock_list.MainSock = sock_host;
	timeval timeout;
	int CostTime;
	int *ID_list = (int *)malloc(sizeof(int)*EQUIPMENTNUM);
	for (int i = 1; i <= EQUIPMENTNUM; ++i)
	{
		ID_list[i - 1] = i;

	}

	timeout.tv_sec = 0;
	timeout.tv_usec = 1000000 * ECHOINTERVAL;




	SYSTEMTIME sys;
	int Milliseconds;



	int retval, headlen = sizeof(sockaddr);




	while (endtime){
		retval = sendto(sock_host, (char *)&ID, sizeof(ID), 0, (sockaddr *)addr_remote, headlen);
		if (retval < 0)
		{
			printf("sendtoʧ�� error code: %d\n", WSAGetLastError());
			return -1;
		}

		


		while (true)
		{
			GetLocalTime(&sys);
			Milliseconds = sys.wMilliseconds;
			make_fdlist(&sock_list, &readfds);
			retval = select(0, &readfds, NULL, NULL, &timeout);
			GetLocalTime(&sys);
			CostTime = (sys.wMilliseconds > Milliseconds) ? (sys.wMilliseconds - Milliseconds) : (1000 - Milliseconds + sys.wMilliseconds);

			if (retval == SOCKET_ERROR)
			{
				printf("selectʧ�� error code: %d\n", WSAGetLastError());
				return -1;
			}
			if (FD_ISSET(sock_host, &readfds))
			{
				// �ɶ���ʾ���ݴﵽ
				retval = recvfrom(sock_host, (char *)&others_ID_int, sizeof(others_ID_int), 0, NULL, NULL);
				if (retval == SOCKET_ERROR)
				{
					printf("recvfromʧ�� error code: %d\n", WSAGetLastError());
					return -1;
				}
				others_ID_int = ntohs(others_ID_int);
				all_member = all_member | others_ID_int;

			}
			if (CostTime >= 500)
			{
				break;
			}
			else
			{
				timeout.tv_usec = 500 - CostTime;
			}

		}
		timeout.tv_usec = 1000000 * ECHOINTERVAL;

		FD_ZERO(&readfds);
		endtime--;


	}
	if (all_member != ALLALIVE)
	{
		raise_message("------------------\n���豸δ����\n");
		for (int i = 0; i < EQUIPMENTNUM; ++i)
		{
			if (ID_list[i] & all_member == ID_list[i])
				printf("�豸id = %d δ����\n", ID_list[i]);
		}
		return -1;

	}

	raise_message("�����豸���Ѿ�����\n");
	
	return 0;

}

/**
* @brief ���ַ������͵�IP��ַת��Ϊ�ַ������͵�IP�㲥��ַ
* 
* @param [in] ipaddr �ַ�ָ��������洢ѡ��������IP��ַ
* @param [in] netmask �ַ�ָ��������洢ѡ�����������������ַ
* @return ����ָ������IP������Ĺ㲥��ַ���ַ�������
*/
char * changeIPaddrtoBroadcastIPaddr(char *ipaddr, char *netmask)
{
	unsigned long int ipaddr_int = inet_addr(ipaddr);
	unsigned long int netmask_int = inet_addr(netmask);
	unsigned long int netmask_low = inet_addr("255.255.255.255");
	netmask_low = netmask_low^netmask_int;
	ipaddr_int = ipaddr_int | netmask_low;
	char *ipbroadcast;
	in_addr ipaddr_in;
	ipaddr_in.S_un.S_addr = ipaddr_int;
	ipbroadcast = inet_ntoa(ipaddr_in);// inet_ntoa�������Լ�mallocһ��ط����ַ���
	return ipbroadcast;

}

/**
* @brief ��ʾ��Ϣ
* �����ַ�������ӡ
*/

void raise_message(char *msg)
{
	printf(msg);
}

/**
* @brief select������������ʼ��socket list
*/
void init_list(socket_list *list)
{
	int i;
	list->MainSock = 0;
	list->num = 0;
	for (i = 0; i < 64; i++){
		list->sock_array[i] = 0;
	}
}
/**
* @brief select��������������socket list
*/
void insert_list(SOCKET s, socket_list *list)
{
	int i;
	for (i = 0; i < 64; i++){
		if (list->sock_array[i] == 0){
			list->sock_array[i] = s;
			list->num += 1;
			break;
		}
	}
}
/**
* @brief select����������ɾ��socket list
*/
void delete_list(SOCKET s, socket_list *list)
{
	int i;
	for (i = 0; i < 64; i++){
		if (list->sock_array[i] == s){
			list->sock_array[i] = 0;
			list->num -= 1;
			break;
		}
	}
}

/**
* @brief select��������������socket list
*/

void make_fdlist(socket_list *list, fd_set *fd_list)
{
	int i;
	FD_SET(list->MainSock, fd_list);
	for (i = 0; i < 64; i++){
		if (list->sock_array[i] > 0){
			FD_SET(list->sock_array[i], fd_list);
		}
	}
}

/**
* @brief ����UDP�׽�����������ʹ����Թ㲥
* @param [in] sock_host ��Ҫ����Ϊ�㲥���׽���������
* @return ����ִ��״̬
*	@retval 0 ִ�гɹ�
*	@retval else ִ��ʧ��
*/
int setUDPBroadcast(SOCKET sock_host)
{
	
	BOOL bBoardcast = TRUE;
	if (SOCKET_ERROR == setsockopt(sock_host, SOL_SOCKET, SO_BROADCAST, (char*)&bBoardcast, sizeof(bBoardcast)))
	{
		printf("setsockopt����udp�㲥ʧ�� error code: %d\n", WSAGetLastError());
		return -1;
	}
	else
	{
		sockaddr_in addr_local ;
		addr_local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		addr_local.sin_port = htons(RECV_PORT);
		addr_local.sin_family = AF_INET;
		if (bind(sock_host, (sockaddr *)&addr_local, sizeof(addr_local)) != 0)
		{
			printf("bind����udp�㲥ʧ�� error code: %d\n", WSAGetLastError());
			return -1;
		}
	}
		return 0;
}
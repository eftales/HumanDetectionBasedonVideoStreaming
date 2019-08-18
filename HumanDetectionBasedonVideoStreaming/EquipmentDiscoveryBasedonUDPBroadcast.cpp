#include "EquipmentDiscoveryBasedonUDPBroadcast.h"
/**
* @file EquipmentDiscoveryBasedonUDPBroadcast.cpp
* @brief 定义设备发现、UDP广播地址设置等函数

*/

int EQUIPMENTNUM = 2;	///< 设备数量，在文件EquipmentDiscoveryBasedonUDPBroadcast.cpp中定义
int ALLALIVE = 3;			///< 由设备数量可以推导到的值，在文件EquipmentDiscoveryBasedonUDPBroadcast.cpp中定义,比如设备数量为3 那么设备编号分别为001 010 100 所以ALLALIVE的值为111=7

/**
* @brief 实现设备发现的逻辑，基于UDP广播
* 
* @param [in] sock_host 输入本机使用的、可以广播的UDP套接字描述符
* @param [in] ID 输入本机的ID，ID为UDP广播时候的载荷
* @param [in] NICnum 输入希望在哪一个无线网卡上发包
* @param [in] Printit 输入是否希望打印网卡信息
* @param [out] ipaddr 是IP地址，字符串类型，输出参数
* @param [out] netmask 选定的网卡的IP广播地址，字符串类型，输出参数
* @param [inout] ipaddr_broadcast 选定的网卡的IP广播地址，字符串类型，如果输入为NULL则NICnum必须有效，将NICnum对应的IP广播地址填入；如果不为NULL，则直接使用该值作为广播的目的地址
* @param [out] addr_remote 远端地址，sock_addr_in 类型，输出参数，由ipaddr_broadcast或者NICnum计算得到
* @return 程序是否成功返回
*	@retval 0 程序执行成功
*	@retval else 程序执行失败
*/

int EquipmentDiscovery(SOCKET sock_host, int ID, int  NICnum, int Printit, char **ipaddr, char ** netmask, char *ipaddr_broadcast, sockaddr_in  *addr_remote)
{
	int all_member = 0;

	all_member = all_member | ID;
	if (all_member == 0)
	{
		raise_message("没有设置设备的ID\n请在命令行参数用 -i X X表示1 - 5进行设置\n");
		return -1;
	}
		
	if (ipaddr_broadcast == NULL)
	{
		
		if (getAllAdapterInfo(ipaddr, netmask, MIB_IF_TYPE_WiFi, DONTPRINTIT, FIRST_USING_WIRELESS_NIC) != 0) {
			raise_message("找不到一个可用的无线网卡\n");
			closesocket(sock_host);
			return -1;
		}

		// 获得无线网卡的广播地址
		changeIPaddrtoBroadcastIPaddr(*ipaddr,*netmask);

	}
	else
	{
		*ipaddr = ipaddr_broadcast;
	}
		

	// 设置发送地址
	addr_remote->sin_addr.S_un.S_addr = inet_addr(*ipaddr);
	addr_remote->sin_port = htons(SEND_PORT);
	addr_remote->sin_family = AF_INET;




	// 设置select
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
			printf("sendto失败 error code: %d\n", WSAGetLastError());
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
				printf("select失败 error code: %d\n", WSAGetLastError());
				return -1;
			}
			if (FD_ISSET(sock_host, &readfds))
			{
				// 可读表示数据达到
				retval = recvfrom(sock_host, (char *)&others_ID_int, sizeof(others_ID_int), 0, NULL, NULL);
				if (retval == SOCKET_ERROR)
				{
					printf("recvfrom失败 error code: %d\n", WSAGetLastError());
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
		raise_message("------------------\n有设备未启动\n");
		for (int i = 0; i < EQUIPMENTNUM; ++i)
		{
			if (ID_list[i] & all_member == ID_list[i])
				printf("设备id = %d 未启动\n", ID_list[i]);
		}
		return -1;

	}

	raise_message("所有设备均已经在线\n");
	
	return 0;

}

/**
* @brief 将字符串类型的IP地址转换为字符串类型的IP广播地址
* 
* @param [in] ipaddr 字符指针变量，存储选定网卡的IP地址
* @param [in] netmask 字符指针变量，存储选定网卡的网络掩码地址
* @return 返回指定子网IP和掩码的广播地址，字符串类型
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
	ipbroadcast = inet_ntoa(ipaddr_in);// inet_ntoa函数会自己malloc一块地方给字符串
	return ipbroadcast;

}

/**
* @brief 提示信息
* 输入字符串，打印
*/

void raise_message(char *msg)
{
	printf(msg);
}

/**
* @brief select辅助函数，初始化socket list
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
* @brief select辅助函数，插入socket list
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
* @brief select辅助函数，删除socket list
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
* @brief select辅助函数，创建socket list
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
* @brief 设置UDP套接字描述符，使其可以广播
* @param [in] sock_host 需要设置为广播的套接字描述符
* @return 函数执行状态
*	@retval 0 执行成功
*	@retval else 执行失败
*/
int setUDPBroadcast(SOCKET sock_host)
{
	
	BOOL bBoardcast = TRUE;
	if (SOCKET_ERROR == setsockopt(sock_host, SOL_SOCKET, SO_BROADCAST, (char*)&bBoardcast, sizeof(bBoardcast)))
	{
		printf("setsockopt设置udp广播失败 error code: %d\n", WSAGetLastError());
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
			printf("bind设置udp广播失败 error code: %d\n", WSAGetLastError());
			return -1;
		}
	}
		return 0;
}
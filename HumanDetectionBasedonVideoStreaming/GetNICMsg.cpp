#include "GetNICMsg.h"

void GetMsgfromcur(char** ipaddr, char **netmask, PIP_ADAPTER_INFO cur, bool printit)
{
	IP_ADDR_STRING *pIpAddrString = &(cur->IpAddressList);
	if (printit)
	{

		cout << "IP:" << pIpAddrString->IpAddress.String << endl;
		cout << "��������:" << pIpAddrString->IpMask.String << endl;
		cout << "Context:" << pIpAddrString->Context << endl;

		char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

		// mac ��ַһ��6���ֽ� 
		// mac ������ת16�����ַ���
		char macStr[18] = { 0 };//12+5+1
		int k = 0;
		for (int j = 0; j < cur->AddressLength; j++){

			macStr[k++] = hex[(cur->Address[j] & 0xf0) >> 4];
			macStr[k++] = hex[cur->Address[j] & 0x0f];
			macStr[k++] = '-';
		}
		macStr[k - 1] = 0;

		cout << "MAC:" << macStr << endl; // mac��ַ 16�����ַ�����ʾ 
		cout << "--------------------------------------------------" << endl;

	}
	*ipaddr = (char *)malloc(sizeof(IP_ADDRESS_STRING));
	strcpy(*ipaddr, pIpAddrString->IpAddress.String);
	*netmask = (char *)malloc(sizeof(IP_ADDRESS_STRING));
	strcpy(*netmask, pIpAddrString->IpMask.String);


}

int getAllAdapterInfo(char** ipaddr, char **netmask, int NICtype, bool printit, int sequence)
{
	if (sequence == 0)
		sequence = 1;
	int CurrentSequence = 1;

	//char *ipaddr = (char *)malloc(sizeof(IP_ADDRESS_STRING));


	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO[ADAPTERNUM];// 20�������ռ� �㹻�� 
	unsigned long stSize = sizeof(IP_ADAPTER_INFO) * ADAPTERNUM;
	// ��ȡ����������Ϣ��������Ϊ����������� 
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	// �ռ䲻��
	if (ERROR_BUFFER_OVERFLOW == nRel) {
		// �ͷſռ�
		if (pIpAdapterInfo != NULL)
			delete[] pIpAdapterInfo;
		return -1;
	}

	PIP_ADAPTER_INFO cur = pIpAdapterInfo;
	// ������� ͨ��������ʽ���������� 
	while (cur){

		if (cur->Type == NICtype)
		{
			switch (cur->Type) {
			case MIB_IF_TYPE_OTHER:
				break;
			case MIB_IF_TYPE_ETHERNET:
				GetMsgfromcur(ipaddr, netmask, cur, printit);
				if (strcmp(UeslessIPaddr, *ipaddr) != 0)
				{
					if (CurrentSequence == sequence)
						return 0;
					else
						++CurrentSequence;
				}

				break;
			case MIB_IF_TYPE_TOKENRING:
				break;
			case MIB_IF_TYPE_FDDI:
				break;
			case MIB_IF_TYPE_PPP:
				break;
			case MIB_IF_TYPE_LOOPBACK:
				break;
			case MIB_IF_TYPE_SLIP:
				break;
			case MIB_IF_TYPE_WiFi:
				GetMsgfromcur(ipaddr, netmask, cur, printit);

				if (strcmp(UeslessIPaddr, *ipaddr) != 0)
				{
					if (CurrentSequence == sequence)
						return 0;
					else
						++CurrentSequence;
				}

				break;
			default://��������,Unknown type

				break;
			}
		}


		cur = cur->Next;

	}

	// �ͷſռ�
	if (pIpAdapterInfo != NULL)
		delete[] pIpAdapterInfo;
}

void getAllAdapterInfo()
{

	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO[ADAPTERNUM];// 20�������ռ� �㹻�� 
	unsigned long stSize = sizeof(IP_ADAPTER_INFO) * ADAPTERNUM;
	// ��ȡ����������Ϣ��������Ϊ����������� 
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	// �ռ䲻��
	if (ERROR_BUFFER_OVERFLOW == nRel) {
		// �ͷſռ�
		if (pIpAdapterInfo != NULL)
			delete[] pIpAdapterInfo;
		return;
	}

	PIP_ADAPTER_INFO cur = pIpAdapterInfo;
	// ������� ͨ��������ʽ���������� 
	while (cur){
		cout << "����������" << cur->Description << endl;
		switch (cur->Type) {
		case MIB_IF_TYPE_OTHER:
			break;
		case MIB_IF_TYPE_ETHERNET:
		{
			IP_ADDR_STRING *pIpAddrString = &(cur->IpAddressList);
			cout << "IP:" << pIpAddrString->IpAddress.String << endl;
			cout << "��������:" << pIpAddrString->IpMask.String << endl;
			cout << "Context:" << pIpAddrString->Context << endl;
		}
			break;
		case MIB_IF_TYPE_TOKENRING:
			break;
		case MIB_IF_TYPE_FDDI:
			break;
		case MIB_IF_TYPE_PPP:
			break;
		case MIB_IF_TYPE_LOOPBACK:
			break;
		case MIB_IF_TYPE_SLIP:
			break;

		default://��������,Unknown type
		{
			IP_ADDR_STRING *pIpAddrString = &(cur->IpAddressList);
			cout << "IP:" << pIpAddrString->IpAddress.String << endl;
			cout << "��������:" << pIpAddrString->IpMask.String << endl;

		}
			break;
		}
		char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

		// mac ��ַһ��6���ֽ� 
		// mac ������ת16�����ַ���
		char macStr[18] = { 0 };//12+5+1
		int k = 0;
		for (int j = 0; j < cur->AddressLength; j++){

			macStr[k++] = hex[(cur->Address[j] & 0xf0) >> 4];
			macStr[k++] = hex[cur->Address[j] & 0x0f];
			macStr[k++] = '-';
		}
		macStr[k - 1] = 0;

		cout << "MAC:" << macStr << endl; // mac��ַ 16�����ַ�����ʾ 
		cur = cur->Next;
		cout << "--------------------------------------------------" << endl;
	}

	// �ͷſռ�
	if (pIpAdapterInfo != NULL)
		delete[] pIpAdapterInfo;
}
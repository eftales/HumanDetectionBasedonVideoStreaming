#include"FilterPicture.h"
#pragma warning(disable:4996)



int filter_picture(int ID, int picture_num_start, int picture_num_current, SOCKET sock_host, sockaddr_in *addr_remote_sendpicture)
{
	

	char imgname[20] = "%d_%d.png";


	int start_num = (picture_num_current + picture_num_start) / 2 - 3, retval, headlen = sizeof(sockaddr);
	for (int i = 0; i < 7; ++i)
	{
		sprintf(imgname, IMGNAME, ID, start_num + i);
		retval = sendto(sock_host, imgname, strlen(imgname), 0, (sockaddr *)addr_remote_sendpicture, headlen);
		if (retval < 0)
		{
			printf("sendtoÊ§°Ü error code: %d\n", WSAGetLastError());
			return -1;
		}

	}
	return 0;




}
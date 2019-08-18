/*!
* @mainpage EquipmentDiscoveryBasedonUDPBroadcast工程概述
* @author 张嘉墀 1479803084@qq.com
* @date 2019-8-12

*/

/**
* @file main.cpp
* @brief 程序的主逻辑
* @details 

*/
#include"EquipmentDiscoveryBasedonUDPBroadcast.h"
#include"GetNICMsg.h"
#include"CameraHumanDetect.h"
#include"CooperateThread.h"
#include"FilterPicture.h"
#include "read_cfg.h"


extern char *optarg;   ///< 无法使用...
extern int optind;     ///< argv的当前索引值

extern int SaveFrame;	// 有3个值，为0表示不存储图片，1和2都要存储图片，为1表示本线程检测到了运动，需要其他进程配合， 此时忽略stop信号，为2表示自己是配合其他进程的，可以收到stop信号，忽略自己内部动作检测
extern bool StopWork; // 有两个值，为0表示不退出程序，为1表示退出程序

int SendPicture = 0;	// 为0表示不发送图片，为1表示发送图片

extern int EQUIPMENTNUM;	///< 设备数量，在文件EquipmentDiscoveryBasedonUDPBroadcast.cpp中定义
extern int ALLALIVE;			///< 由设备数量可以推导到的值，在文件EquipmentDiscoveryBasedonUDPBroadcast.cpp中定义,比如设备数量为3 那么设备编号分别为001 010 100 所以ALLALIVE的值为111=7

#define IMGNAME "%d_%d.png"	///< 图片名字模板


#define USELESS_ID 0	///< 默认本设备的ID，如果为0，则程序主动退出




int main(int argc,char**argv)
{
	

	// 设备发现阶段参数
	int Printit = DONTPRINTIT;	///< 设备发现阶段参数 Printit,是否要打印所有网卡信息，默认为不打印
	int NICnum = FIRST_USING_WIRELESS_NIC;	///< 设备发现阶段参数 NICnum，需要在第几个网卡上运行，默认为第一个可用的无线网卡
	char *ipaddr_broadcast = NULL;///< 设备发现阶段参数 ipaddr_broadcast，广播地址，广播地址一般会由程序自动生成，不过也可以手动输入
	// 人体检测阶段参数
	int whichCamera = DEFAULT_CAMERA;	///< 人体检测阶段参数 whichCamera，需要使用笔记本上的哪个摄像头

	// 本设备ID
	int ID = USELESS_ID;	///< 本设备ID ID

	// 摄像头分辨率
	int frame_width = 640;	///< 摄像头分辨率的宽度，默认为640
	int frame_height = 480;///< 摄像头分辨率的高度，默认为480
	// 视频分割参数
	int critical_to_move = 5;		  ///< 可以检测到人的帧数目要达到critical_to_move = 5（默认）才认为有动作
	int critical_to_stop = 1;		  ///< 不可以检测到人的帧数目要达到critical_to_move = 1（默认）才认为这是一段有效的图片组
	int critical_stoped = 5;		  ///< 可以检测到人的帧数目要达到critical_to_move = 5（默认）才认为有动作真正结束

	my_struct_detect_result my_struct_undetect = { 0 }, my_struct_detect = { 0 };


	// 有效区域:人处在这个区域才算有效
	int inner_area = 0.2;	// 视频分割参数 inner_area，有效区域:人处在这个区域才算有效

	if (argc < 2)// 读取配置文件
	{
		Config cfg;

		// Read the file. If there is an error, report it and exit.
		try
		{
			cfg.readFile("HumanDetection.cfg");
		}
		catch (const FileIOException &fioex)
		{
			std::cerr << "I/O error while reading file." << std::endl;
			return(EXIT_FAILURE);
		}
		catch (const ParseException &pex)
		{
			std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
				<< " - " << pex.getError() << std::endl;
			return(EXIT_FAILURE);
		}
		// 设备发现阶段参数
		cfg.lookupValue("Parameter.EquipmentDiscovery.Printit", Printit);
		cfg.lookupValue("Parameter.EquipmentDiscovery.NICnum", NICnum);
		cfg.lookupValue("Parameter.EquipmentDiscovery.EQUIPMENTNUM", EQUIPMENTNUM);
		cfg.lookupValue("Parameter.EquipmentDiscovery.ALLALIVE", ALLALIVE);
		string ip;
		cfg.lookupValue("Parameter.EquipmentDiscovery.ipaddr_broadcast", ip);
		ipaddr_broadcast = (char *)malloc(sizeof(char)*ip.length() + 1);
		strcpy(ipaddr_broadcast, ip.c_str());
		// 人体检测阶段参数
		cfg.lookupValue("Parameter.whichCamera", whichCamera);// cfg.lookupValue如果cfg文件中没有这个path，那么也不会更改传入的值
		// 本设备ID
		cfg.lookupValue("Parameter.ID", ID);
		// 摄像头分辨率
		cfg.lookupValue("Parameter.CameraResolution.frame_width", frame_width);
		cfg.lookupValue("Parameter.CameraResolution.frame_height", frame_height);
		// 视频分割参数
		cfg.lookupValue("Parameter.ConfidenceRegion.critical_to_move", critical_to_move);// 可以检测到人的帧数目要达到critical_to_move = 5（默认）才认为有动作
		cfg.lookupValue("Parameter.ConfidenceRegion.critical_to_stop", critical_to_stop);// 不可以检测到人的帧数目要达到critical_to_move = 1（默认）才认为这是一段有效的图片组
		cfg.lookupValue("Parameter.ConfidenceRegion.critical_stoped", critical_stoped);// 可以检测到人的帧数目要达到critical_to_move = 5（默认）才认为有动作真正结束
		// 有效区域:人处在这个区域才算有效
		cfg.lookupValue("Parameter.ConfidenceRegion.inner_area", inner_area);
	}
	else
	{
		int opt = 0;
		while ((opt = getopt(argc, argv, "pn::a::i::c::")) != -1)
		{
			switch (opt)
			{
			case 'p':
				Printit = 1;
				break;
			case 'n':
				NICnum = atoi(argv[optind]);
				break;
			case 'a':
				ipaddr_broadcast = argv[optind];
				break;
			case 'c':
				whichCamera = atoi(argv[optind]);
				break;
			case 'i':
				switch (atoi(argv[optind]))
				{
				case 1:
					ID = 1;
					break;
				case 2:
					ID = 2;
					break;
				case 3:
					ID = 4;
					break;
				case 4:
					ID = 8;
					break;
				case 5:
					ID = 16;
					break;
				}

			}
		}
	}


	// 照片名称设置
	char imgname[20] = "%d_%d.png";
	int picture_num_current = 1, picture_num_start = 1;


	// UDP设置
	WSADATA wsa;
	WSAStartup(0x201, &wsa);
	SOCKET sock_host; // 用于设备发现，
	sock_host = socket(AF_INET, SOCK_DGRAM, 0);
	// 设置广播参数，并与端口2019绑定
	if (setUDPBroadcast(sock_host) == -1)
	{
		raise_message("UDP广播设置失败\n");
		return -1;
	}
	// 各类IP地址
	char *ipaddr = NULL, *netmask = NULL;
	// 发送地址
	sockaddr_in  *addr_remote = new sockaddr_in;
	sockaddr_in *addr_remote_sendpicture = new sockaddr_in;
	addr_remote_sendpicture->sin_addr.S_un.S_addr = inet_addr(SENDPICTUREIP);
	addr_remote_sendpicture->sin_port = htons(SENDPICTUREPORT);
	addr_remote_sendpicture->sin_family = AF_INET;

	// 创建人体检测分类器
	cv::HOGDescriptor *myHOG;
	myHOG = GetmyHOG();

	// 打开摄像头
	cv::VideoCapture cap;
	cap.open(whichCamera); //打开摄像头
	if (!cap.isOpened())
	{
		raise_message("摄像头打开失败\n");
		return -1;
	}
	// 设置摄像头分辨率
	cap.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
	cv::Mat frame;

	// 创建协作线程
	pthread_t *ThreadPidUDPServer = new pthread_t;
	pthread_attr_t *ThreadAttrUDPServer = new pthread_attr_t;
	// 线程参数
	mypara pstru;
	pstru.sock_host = sock_host;
	pstru.ID = ID;

	CreatThreadVal(ThreadPidUDPServer, ThreadAttrUDPServer);
	pthread_attr_init(ThreadAttrUDPServer);
	pthread_attr_setscope(ThreadAttrUDPServer, PTHREAD_SCOPE_PROCESS);
	pthread_attr_setdetachstate(ThreadAttrUDPServer, PTHREAD_CREATE_DETACHED);
	

	// 发现设备过程
	if (EquipmentDiscovery(sock_host, ID, NICnum, Printit, &ipaddr, &netmask, ipaddr_broadcast, addr_remote) != 0)
	{
		raise_message("设备发现失败\n");
		return -1;
	}


	// 设备发现完成之后，创建协作线程
	pstru.addr_remote = addr_remote;
	pthread_create(ThreadPidUDPServer, ThreadAttrUDPServer, Synchronization, &pstru);

	
	while (1)
	{
		if (StopWork)
			break;

		cap.read(frame);//获得图片;
		//Canny(frame, frame, 30, 100);//canny边缘检测，去掉这一行就是纯粹的读取摄像头了 可以考虑加一点处理
		//cvtColor(frame, frame, CV_BGR2HSV);//BGR空间转为HSV颜色空间，注意，两者不能同时对同一张图片（在此为frame）进行处理，否则报错
		if (frame.empty())
			break;
		
		if (SaveFrame != 0)
		{
			sprintf(imgname, IMGNAME, ID, picture_num_current);
			cv::imwrite(imgname, frame);
			
			++picture_num_current;

		}
		
		//// 测试
		//SendPicture = 1;
		if (SendPicture)
		{
			//// 测试
			//picture_num_current = 8;
			//picture_num_start = 1;
			//ID = 1;
			if ((picture_num_current - picture_num_start) >= CONVINCEDPICTURENUM)
			{
				filter_picture(ID, picture_num_start, picture_num_current, sock_host, addr_remote_sendpicture);

			}
			else
			{
				printf("图片数量不够，无法送检\n");
			}
			SendPicture = 0;
			picture_num_start = picture_num_current;

		}
	


		std::vector<cv::Rect> found;
		myHOG->detectMultiScale(frame, found);

		int human_detected = 0;
		for (int i = 0; i < found.size(); i++)
		{
			cv::Rect r = found[i];
			// 认为人体应该在视频中间
			if ((r.tl().x >= int(inner_area*frame_width)) && (r.br().x <= int((1 - inner_area)*frame_width)))
				if ((r.tl().y >= int(inner_area*frame_height)) && (r.br().y <= int((1 - inner_area)*frame_height)))
				{
					cv::rectangle(frame, r.tl(), r.br(), cv::Scalar(0, 0, 255), 3);
					human_detected = 1;
				}
					

		}
		
		switch (SaveFrame)
		{
		case 0:// 判定动作是否开始
			// 判断动作是否开始的条件：连续n = critical_to_move帧可以检测到人体的情况下，突然检测不到人体
			if (human_detected)
			{
				++my_struct_detect.num;
			}
			else
			{
				if (my_struct_detect.num >= critical_to_move)//认为动作开始
				{
					BroadcastStartMsg(sock_host, addr_remote);
					SaveFrame = 1;
					my_struct_detect.num = 0;// 情况检测到的帧数，为下一步腾出空间
				}
				else
				{
					my_struct_detect.num = 0;//重新开始计数
				}
			}


			break;
		case 1:// 判定动作是否结束和判定动作是否有效等价
			//当连续10帧检测到人体之后，认为动作结束
			if (human_detected)
			{
				if (my_struct_undetect.num != 0)
				{
					// 如果不是第一次检测到人体
					++my_struct_detect.num;


				}
				else
				{
					//如果是第一次检测到人体需要判断 没有检测到人体的帧是否够约定好的有效帧数
					if (my_struct_undetect.num >= critical_to_stop)
					{
						++my_struct_detect.num;
					}
					else
					{
						++my_struct_undetect.num;
					}
				}
				// 判读动作是否满足结束要求
				if (my_struct_detect.num >= critical_stoped)
				{
					//动作结束
					BroadcastStopMsg(sock_host, addr_remote);
					SendPicture = 1;
					SaveFrame = 0;
					my_struct_detect.num = 0;
					my_struct_undetect.num = 0;

				}
			}
			else
			{
				++my_struct_undetect.num;

			}


			break;
		case 2:// 本程序作为协作程序，不需要检测何时结束
			// 这里需要修改参数，不过是在第一次收到请求协作的信号之后就改过了
			break;
		case 3:
			SaveFrame = 0;
			SendPicture = 1;
			break;

		}


		imshow("行为检测", frame);
		if (cv::waitKey(1) >= 0)//按下任意键退出摄像头
		{
			BroadcastExitMsg(sock_host, addr_remote);
			break;
		}
	}
	cap.release();
	cv::destroyAllWindows();//关闭所有窗口


	WSACleanup();
	return 0;
}
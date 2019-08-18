/*!
* @mainpage EquipmentDiscoveryBasedonUDPBroadcast���̸���
* @author �ż�ܯ 1479803084@qq.com
* @date 2019-8-12

*/

/**
* @file main.cpp
* @brief ��������߼�
* @details 

*/
#include"EquipmentDiscoveryBasedonUDPBroadcast.h"
#include"GetNICMsg.h"
#include"CameraHumanDetect.h"
#include"CooperateThread.h"
#include"FilterPicture.h"
#include "read_cfg.h"


extern char *optarg;   ///< �޷�ʹ��...
extern int optind;     ///< argv�ĵ�ǰ����ֵ

extern int SaveFrame;	// ��3��ֵ��Ϊ0��ʾ���洢ͼƬ��1��2��Ҫ�洢ͼƬ��Ϊ1��ʾ���̼߳�⵽���˶�����Ҫ����������ϣ� ��ʱ����stop�źţ�Ϊ2��ʾ�Լ�������������̵ģ������յ�stop�źţ������Լ��ڲ��������
extern bool StopWork; // ������ֵ��Ϊ0��ʾ���˳�����Ϊ1��ʾ�˳�����

int SendPicture = 0;	// Ϊ0��ʾ������ͼƬ��Ϊ1��ʾ����ͼƬ

extern int EQUIPMENTNUM;	///< �豸���������ļ�EquipmentDiscoveryBasedonUDPBroadcast.cpp�ж���
extern int ALLALIVE;			///< ���豸���������Ƶ�����ֵ�����ļ�EquipmentDiscoveryBasedonUDPBroadcast.cpp�ж���,�����豸����Ϊ3 ��ô�豸��ŷֱ�Ϊ001 010 100 ����ALLALIVE��ֵΪ111=7

#define IMGNAME "%d_%d.png"	///< ͼƬ����ģ��


#define USELESS_ID 0	///< Ĭ�ϱ��豸��ID�����Ϊ0������������˳�




int main(int argc,char**argv)
{
	

	// �豸���ֽ׶β���
	int Printit = DONTPRINTIT;	///< �豸���ֽ׶β��� Printit,�Ƿ�Ҫ��ӡ����������Ϣ��Ĭ��Ϊ����ӡ
	int NICnum = FIRST_USING_WIRELESS_NIC;	///< �豸���ֽ׶β��� NICnum����Ҫ�ڵڼ������������У�Ĭ��Ϊ��һ�����õ���������
	char *ipaddr_broadcast = NULL;///< �豸���ֽ׶β��� ipaddr_broadcast���㲥��ַ���㲥��ַһ����ɳ����Զ����ɣ�����Ҳ�����ֶ�����
	// ������׶β���
	int whichCamera = DEFAULT_CAMERA;	///< ������׶β��� whichCamera����Ҫʹ�ñʼǱ��ϵ��ĸ�����ͷ

	// ���豸ID
	int ID = USELESS_ID;	///< ���豸ID ID

	// ����ͷ�ֱ���
	int frame_width = 640;	///< ����ͷ�ֱ��ʵĿ�ȣ�Ĭ��Ϊ640
	int frame_height = 480;///< ����ͷ�ֱ��ʵĸ߶ȣ�Ĭ��Ϊ480
	// ��Ƶ�ָ����
	int critical_to_move = 5;		  ///< ���Լ�⵽�˵�֡��ĿҪ�ﵽcritical_to_move = 5��Ĭ�ϣ�����Ϊ�ж���
	int critical_to_stop = 1;		  ///< �����Լ�⵽�˵�֡��ĿҪ�ﵽcritical_to_move = 1��Ĭ�ϣ�����Ϊ����һ����Ч��ͼƬ��
	int critical_stoped = 5;		  ///< ���Լ�⵽�˵�֡��ĿҪ�ﵽcritical_to_move = 5��Ĭ�ϣ�����Ϊ�ж�����������

	my_struct_detect_result my_struct_undetect = { 0 }, my_struct_detect = { 0 };


	// ��Ч����:�˴���������������Ч
	int inner_area = 0.2;	// ��Ƶ�ָ���� inner_area����Ч����:�˴���������������Ч

	if (argc < 2)// ��ȡ�����ļ�
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
		// �豸���ֽ׶β���
		cfg.lookupValue("Parameter.EquipmentDiscovery.Printit", Printit);
		cfg.lookupValue("Parameter.EquipmentDiscovery.NICnum", NICnum);
		cfg.lookupValue("Parameter.EquipmentDiscovery.EQUIPMENTNUM", EQUIPMENTNUM);
		cfg.lookupValue("Parameter.EquipmentDiscovery.ALLALIVE", ALLALIVE);
		string ip;
		cfg.lookupValue("Parameter.EquipmentDiscovery.ipaddr_broadcast", ip);
		ipaddr_broadcast = (char *)malloc(sizeof(char)*ip.length() + 1);
		strcpy(ipaddr_broadcast, ip.c_str());
		// ������׶β���
		cfg.lookupValue("Parameter.whichCamera", whichCamera);// cfg.lookupValue���cfg�ļ���û�����path����ôҲ������Ĵ����ֵ
		// ���豸ID
		cfg.lookupValue("Parameter.ID", ID);
		// ����ͷ�ֱ���
		cfg.lookupValue("Parameter.CameraResolution.frame_width", frame_width);
		cfg.lookupValue("Parameter.CameraResolution.frame_height", frame_height);
		// ��Ƶ�ָ����
		cfg.lookupValue("Parameter.ConfidenceRegion.critical_to_move", critical_to_move);// ���Լ�⵽�˵�֡��ĿҪ�ﵽcritical_to_move = 5��Ĭ�ϣ�����Ϊ�ж���
		cfg.lookupValue("Parameter.ConfidenceRegion.critical_to_stop", critical_to_stop);// �����Լ�⵽�˵�֡��ĿҪ�ﵽcritical_to_move = 1��Ĭ�ϣ�����Ϊ����һ����Ч��ͼƬ��
		cfg.lookupValue("Parameter.ConfidenceRegion.critical_stoped", critical_stoped);// ���Լ�⵽�˵�֡��ĿҪ�ﵽcritical_to_move = 5��Ĭ�ϣ�����Ϊ�ж�����������
		// ��Ч����:�˴���������������Ч
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


	// ��Ƭ��������
	char imgname[20] = "%d_%d.png";
	int picture_num_current = 1, picture_num_start = 1;


	// UDP����
	WSADATA wsa;
	WSAStartup(0x201, &wsa);
	SOCKET sock_host; // �����豸���֣�
	sock_host = socket(AF_INET, SOCK_DGRAM, 0);
	// ���ù㲥����������˿�2019��
	if (setUDPBroadcast(sock_host) == -1)
	{
		raise_message("UDP�㲥����ʧ��\n");
		return -1;
	}
	// ����IP��ַ
	char *ipaddr = NULL, *netmask = NULL;
	// ���͵�ַ
	sockaddr_in  *addr_remote = new sockaddr_in;
	sockaddr_in *addr_remote_sendpicture = new sockaddr_in;
	addr_remote_sendpicture->sin_addr.S_un.S_addr = inet_addr(SENDPICTUREIP);
	addr_remote_sendpicture->sin_port = htons(SENDPICTUREPORT);
	addr_remote_sendpicture->sin_family = AF_INET;

	// ���������������
	cv::HOGDescriptor *myHOG;
	myHOG = GetmyHOG();

	// ������ͷ
	cv::VideoCapture cap;
	cap.open(whichCamera); //������ͷ
	if (!cap.isOpened())
	{
		raise_message("����ͷ��ʧ��\n");
		return -1;
	}
	// ��������ͷ�ֱ���
	cap.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
	cv::Mat frame;

	// ����Э���߳�
	pthread_t *ThreadPidUDPServer = new pthread_t;
	pthread_attr_t *ThreadAttrUDPServer = new pthread_attr_t;
	// �̲߳���
	mypara pstru;
	pstru.sock_host = sock_host;
	pstru.ID = ID;

	CreatThreadVal(ThreadPidUDPServer, ThreadAttrUDPServer);
	pthread_attr_init(ThreadAttrUDPServer);
	pthread_attr_setscope(ThreadAttrUDPServer, PTHREAD_SCOPE_PROCESS);
	pthread_attr_setdetachstate(ThreadAttrUDPServer, PTHREAD_CREATE_DETACHED);
	

	// �����豸����
	if (EquipmentDiscovery(sock_host, ID, NICnum, Printit, &ipaddr, &netmask, ipaddr_broadcast, addr_remote) != 0)
	{
		raise_message("�豸����ʧ��\n");
		return -1;
	}


	// �豸�������֮�󣬴���Э���߳�
	pstru.addr_remote = addr_remote;
	pthread_create(ThreadPidUDPServer, ThreadAttrUDPServer, Synchronization, &pstru);

	
	while (1)
	{
		if (StopWork)
			break;

		cap.read(frame);//���ͼƬ;
		//Canny(frame, frame, 30, 100);//canny��Ե��⣬ȥ����һ�о��Ǵ���Ķ�ȡ����ͷ�� ���Կ��Ǽ�һ�㴦��
		//cvtColor(frame, frame, CV_BGR2HSV);//BGR�ռ�תΪHSV��ɫ�ռ䣬ע�⣬���߲���ͬʱ��ͬһ��ͼƬ���ڴ�Ϊframe�����д������򱨴�
		if (frame.empty())
			break;
		
		if (SaveFrame != 0)
		{
			sprintf(imgname, IMGNAME, ID, picture_num_current);
			cv::imwrite(imgname, frame);
			
			++picture_num_current;

		}
		
		//// ����
		//SendPicture = 1;
		if (SendPicture)
		{
			//// ����
			//picture_num_current = 8;
			//picture_num_start = 1;
			//ID = 1;
			if ((picture_num_current - picture_num_start) >= CONVINCEDPICTURENUM)
			{
				filter_picture(ID, picture_num_start, picture_num_current, sock_host, addr_remote_sendpicture);

			}
			else
			{
				printf("ͼƬ�����������޷��ͼ�\n");
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
			// ��Ϊ����Ӧ������Ƶ�м�
			if ((r.tl().x >= int(inner_area*frame_width)) && (r.br().x <= int((1 - inner_area)*frame_width)))
				if ((r.tl().y >= int(inner_area*frame_height)) && (r.br().y <= int((1 - inner_area)*frame_height)))
				{
					cv::rectangle(frame, r.tl(), r.br(), cv::Scalar(0, 0, 255), 3);
					human_detected = 1;
				}
					

		}
		
		switch (SaveFrame)
		{
		case 0:// �ж������Ƿ�ʼ
			// �ж϶����Ƿ�ʼ������������n = critical_to_move֡���Լ�⵽���������£�ͻȻ��ⲻ������
			if (human_detected)
			{
				++my_struct_detect.num;
			}
			else
			{
				if (my_struct_detect.num >= critical_to_move)//��Ϊ������ʼ
				{
					BroadcastStartMsg(sock_host, addr_remote);
					SaveFrame = 1;
					my_struct_detect.num = 0;// �����⵽��֡����Ϊ��һ���ڳ��ռ�
				}
				else
				{
					my_struct_detect.num = 0;//���¿�ʼ����
				}
			}


			break;
		case 1:// �ж������Ƿ�������ж������Ƿ���Ч�ȼ�
			//������10֡��⵽����֮����Ϊ��������
			if (human_detected)
			{
				if (my_struct_undetect.num != 0)
				{
					// ������ǵ�һ�μ�⵽����
					++my_struct_detect.num;


				}
				else
				{
					//����ǵ�һ�μ�⵽������Ҫ�ж� û�м�⵽�����֡�Ƿ�Լ���õ���Ч֡��
					if (my_struct_undetect.num >= critical_to_stop)
					{
						++my_struct_detect.num;
					}
					else
					{
						++my_struct_undetect.num;
					}
				}
				// �ж������Ƿ��������Ҫ��
				if (my_struct_detect.num >= critical_stoped)
				{
					//��������
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
		case 2:// ��������ΪЭ�����򣬲���Ҫ����ʱ����
			// ������Ҫ�޸Ĳ������������ڵ�һ���յ�����Э�����ź�֮��͸Ĺ���
			break;
		case 3:
			SaveFrame = 0;
			SendPicture = 1;
			break;

		}


		imshow("��Ϊ���", frame);
		if (cv::waitKey(1) >= 0)//����������˳�����ͷ
		{
			BroadcastExitMsg(sock_host, addr_remote);
			break;
		}
	}
	cap.release();
	cv::destroyAllWindows();//�ر����д���


	WSACleanup();
	return 0;
}
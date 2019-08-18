#include "CameraHumanDetect.h"

/**
* @file CameraHumanDetect.cpp
* @brief ��������������HOG���ӵĺ���
*/

/**
* @brief �������룬����cv::HOGDescriptor��ָ��

* @return ����cv::HOGDescriptor��ָ��
*/

cv::HOGDescriptor* GetmyHOG()
{
	int DescriptorDim;//HOG�����ӵ�ά������ͼƬ��С����ⴰ�ڴ�С�����С��ϸ����Ԫ��ֱ��ͼbin��������
	MySVM svm;//SVM������

	svm.load("SVM_HOG.xml");

	DescriptorDim = svm.get_var_count();//����������ά������HOG�����ӵ�ά��
	int supportVectorNum = svm.get_support_vector_count();//֧�������ĸ���

	cv::Mat alphaMat = cv::Mat::zeros(1, supportVectorNum, CV_32FC1);//alpha���������ȵ���֧����������
	cv::Mat supportVectorMat = cv::Mat::zeros(supportVectorNum, DescriptorDim, CV_32FC1);//֧����������
	cv::Mat resultMat = cv::Mat::zeros(1, DescriptorDim, CV_32FC1);//alpha��������֧����������Ľ��

	//��֧�����������ݸ��Ƶ�supportVectorMat������
	for (int i = 0; i<supportVectorNum; i++)
	{
		const float * pSVData = svm.get_support_vector(i);//���ص�i��֧������������ָ��
		for (int j = 0; j<DescriptorDim; j++)
		{
			//std::cout<<pData[j]<<" ";
			supportVectorMat.at<float>(i, j) = pSVData[j];
		}
	}

	//��alpha���������ݸ��Ƶ�alphaMat��
	double * pAlphaData = svm.get_alpha_vector();//����SVM�ľ��ߺ����е�alpha����
	for (int i = 0; i<supportVectorNum; i++)
	{
		alphaMat.at<float>(0, i) = pAlphaData[i];
	}

	//����-(alphaMat * supportVectorMat),����ŵ�resultMat��
	//gemm(alphaMat, supportVectorMat, -1, 0, 1, resultMat);//��֪��Ϊʲô�Ӹ��ţ�
	resultMat = -1 * alphaMat * supportVectorMat;

	//�õ����յ�setSVMDetector(const vector<float>& detector)�����п��õļ����
	std::vector<float> myDetector;
	//��resultMat�е����ݸ��Ƶ�����myDetector��
	for (int i = 0; i<DescriptorDim; i++)
	{
		myDetector.push_back(resultMat.at<float>(0, i));
	}
	//������ƫ����rho���õ������
	myDetector.push_back(svm.get_rho());
	//����HOGDescriptor�ļ����
	cv::HOGDescriptor *myHOG = new cv::HOGDescriptor;
	myHOG->setSVMDetector(myDetector);
	return myHOG;
}



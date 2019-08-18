#include "CameraHumanDetect.h"

/**
* @file CameraHumanDetect.cpp
* @brief 定义生成人体检测HOG算子的函数
*/

/**
* @brief 无需输入，返回cv::HOGDescriptor的指针

* @return 返回cv::HOGDescriptor的指针
*/

cv::HOGDescriptor* GetmyHOG()
{
	int DescriptorDim;//HOG描述子的维数，由图片大小、检测窗口大小、块大小、细胞单元中直方图bin个数决定
	MySVM svm;//SVM分类器

	svm.load("SVM_HOG.xml");

	DescriptorDim = svm.get_var_count();//特征向量的维数，即HOG描述子的维数
	int supportVectorNum = svm.get_support_vector_count();//支持向量的个数

	cv::Mat alphaMat = cv::Mat::zeros(1, supportVectorNum, CV_32FC1);//alpha向量，长度等于支持向量个数
	cv::Mat supportVectorMat = cv::Mat::zeros(supportVectorNum, DescriptorDim, CV_32FC1);//支持向量矩阵
	cv::Mat resultMat = cv::Mat::zeros(1, DescriptorDim, CV_32FC1);//alpha向量乘以支持向量矩阵的结果

	//将支持向量的数据复制到supportVectorMat矩阵中
	for (int i = 0; i<supportVectorNum; i++)
	{
		const float * pSVData = svm.get_support_vector(i);//返回第i个支持向量的数据指针
		for (int j = 0; j<DescriptorDim; j++)
		{
			//std::cout<<pData[j]<<" ";
			supportVectorMat.at<float>(i, j) = pSVData[j];
		}
	}

	//将alpha向量的数据复制到alphaMat中
	double * pAlphaData = svm.get_alpha_vector();//返回SVM的决策函数中的alpha向量
	for (int i = 0; i<supportVectorNum; i++)
	{
		alphaMat.at<float>(0, i) = pAlphaData[i];
	}

	//计算-(alphaMat * supportVectorMat),结果放到resultMat中
	//gemm(alphaMat, supportVectorMat, -1, 0, 1, resultMat);//不知道为什么加负号？
	resultMat = -1 * alphaMat * supportVectorMat;

	//得到最终的setSVMDetector(const vector<float>& detector)参数中可用的检测子
	std::vector<float> myDetector;
	//将resultMat中的数据复制到数组myDetector中
	for (int i = 0; i<DescriptorDim; i++)
	{
		myDetector.push_back(resultMat.at<float>(0, i));
	}
	//最后添加偏移量rho，得到检测子
	myDetector.push_back(svm.get_rho());
	//设置HOGDescriptor的检测子
	cv::HOGDescriptor *myHOG = new cv::HOGDescriptor;
	myHOG->setSVMDetector(myDetector);
	return myHOG;
}



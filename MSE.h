#pragma once
#include <opencv2/opencv.hpp>

class ImageData;

class MSE
{
private:
	double mseValue;
public:
	MSE():mseValue(0.0){}
	MSE(double mseValue):mseValue(mseValue){}

	double getMSE() { return this->mseValue; }
	void calculateMSE(cv::Mat sourceImage, std::vector<ImageData> imagesInfo);
	double computeMSE(cv::Mat imageA, cv::Mat imageB);
	void printMSE();
};


#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

class TamuraFeatures
{
private:
	double coarseness, contrast, directionality, linelikeness, regularity, roughness;
public:
	TamuraFeatures() : coarseness(0.0), contrast(0.0), directionality(0.0), linelikeness(0.0),
		regularity(0.0),roughness(0.0) {}
	TamuraFeatures(double coarsenessValue, double contrastValue, double directionalityValue, double linelikenessValue,
		double regularityValue, double roughnessValue) : coarseness(coarsenessValue), contrast(contrastValue), 
		directionality(directionalityValue), linelikeness(linelikenessValue), regularity(regularityValue), roughness(roughnessValue) {}

	double getCoarseness() { return this->coarseness; }
	double getContrast() { return this->contrast; }
	double getDirectionality() { return this->directionality; }
	double getLinelikeness() { return this->linelikeness; }
	double getRegularity() { return this->regularity; }
	double getRoughness() { return this->roughness; }

	void calculateCoarseness(cv::Mat image);
	void calculateContrast(cv::Mat image);
	void calculateDirectionality(cv::Mat image);
    void calculateLinelikeness(cv::Mat image, int n, int w);//n - количество направлений, w - радиус
	void calculateRegularity(cv::Mat image);
	void calculateRoughness(cv::Mat image);

	void printFeatures();

	//промежуточные для грубости
	cv::Mat morphConnectingEdges(cv::Mat edges);//соединение ребер по морфологическому правилу, где соединяется пиксель, если есть два несвязанных ненулевых соседа
	int diffTexels3x3(cv::Mat texelsImage, int x, int y);//различные тексели на области 3 на 3
	double calculateCoarsenessFromFormula(cv::Mat texelsImage);//расчет грубости по формуле
	
	//промежуточные для контраста
	double calculateContrastInLocation(cv::Mat location);//расчет контраста для окрестности пикселя
	//промежуточные для линейности
	double calculatePixelLineLikeness(cv::Mat image,int x, int y, int n, int w);//линейность для каждого пикселя
	//промежуточные для регулярности
	double calculateRegularityInLocation(cv::Mat location);//расчет регулярности для окрестности пикселя
};


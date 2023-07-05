#pragma once
#include <opencv2/opencv.hpp>
#include "TamuraFeatures.h"
#include "MSE.h"

class ImageData
{
private:
    std::string imageName;
    cv::Mat pixelMatrix;
    TamuraFeatures tamuraFeatures;
    MSE mse;
    double targetFunction;

    int rank;//ранг особи после недоминирующей сортировки

public:
    ImageData() : targetFunction(0.0), rank(0) {}
    ImageData(const cv::Size& matrixSize)
        : pixelMatrix(matrixSize, CV_8UC1), targetFunction(0.0), rank(0) {}
    ImageData(std::string filename, const cv::Mat& matrix, const TamuraFeatures& features, const MSE& mseValue)
        : imageName(filename), pixelMatrix(matrix), tamuraFeatures(features), mse(mseValue), targetFunction(0.0), rank(0){}
    ImageData(const std::string& filename, const cv::Mat& matrix)
        : imageName(filename), pixelMatrix(matrix), targetFunction(0.0), rank(0) {}

    void setPixelMatrix(const cv::Mat& matrix);
    void calculateTamuraFeatures();
    void calculateMSE(std::vector<ImageData> imageDataSet);

    void evaluateTargetFunction(std::vector<double> weights);
    double getTargetFunction() { return targetFunction; }
    cv::Mat getPixelMatrix() { return pixelMatrix; }
    TamuraFeatures getTamuraFeatures() { return tamuraFeatures; }
    MSE getMSE() { return mse; }
    std::string getImageName() { return imageName; }
    int getRank() { return rank; }
    void setRank(int rank);
};


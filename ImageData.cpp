#include "ImageData.h"

void ImageData::setPixelMatrix(const cv::Mat& matrix)
{
	pixelMatrix = matrix;
}

void ImageData::calculateTamuraFeatures()
{
    TamuraFeatures features;

    features.calculateCoarseness(this->pixelMatrix);
    features.calculateContrast(this->pixelMatrix);
    features.calculateDirectionality(this->pixelMatrix);
    features.calculateLinelikeness(this->pixelMatrix, 8, 3);
    features.calculateRegularity(this->pixelMatrix);
    features.calculateRoughness(this->pixelMatrix);

    this->tamuraFeatures = features;
    
}

void ImageData::calculateMSE(std::vector<ImageData> imageDataSet)
{
    MSE mseFunc;

    mseFunc.calculateMSE(this->pixelMatrix, imageDataSet);

    this->mse = mseFunc;
}

void ImageData::evaluateTargetFunction(std::vector<double> weights)
{
    // Вычисление целевой функции
    double targetFunction = 0.0;
    targetFunction += weights[0] * tamuraFeatures.getCoarseness();
    targetFunction += weights[1] * tamuraFeatures.getContrast();
    targetFunction += weights[2] * tamuraFeatures.getDirectionality();
    targetFunction += weights[3] * tamuraFeatures.getLinelikeness();
    targetFunction += weights[4] * tamuraFeatures.getRegularity();
    targetFunction += weights[5] * tamuraFeatures.getRoughness();
    targetFunction += weights[6] * mse.getMSE();

    // Сохранение результата в поле targetFunction
    this->targetFunction = targetFunction;
}

void ImageData::setRank(int rank)
{
    this->rank = rank;
}

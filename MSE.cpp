#include "MSE.h"
#include "ImageData.h"
#include <omp.h>

void MSE::calculateMSE(cv::Mat sourceImage, std::vector<ImageData> imagesInfo)
{
    double totalMSE = 0.0;
    double minMSE = std::numeric_limits<double>::max();
    double maxMSE = std::numeric_limits<double>::min();
    size_t numImages = imagesInfo.size();

    #pragma omp parallel for reduction(+:totalMSE) reduction(min:minMSE) reduction(max:maxMSE)
    for (size_t i = 0; i < numImages; ++i)
    {
        cv::Mat image = imagesInfo[i].getPixelMatrix();

        double mse = computeMSE(sourceImage, image);

        totalMSE += mse;

        if (mse < minMSE)
            minMSE = mse;

        if (mse > maxMSE)
            maxMSE = mse;
    }

    // MSE
    totalMSE /= (numImages - 1);

    // Нормализация итогового значения MSE
    this->mseValue = totalMSE / (maxMSE - minMSE);
}

double MSE::computeMSE(cv::Mat imageA, cv::Mat imageB)
{
    cv::Mat diff;
    cv::absdiff(imageA, imageB, diff);
    cv::Mat squaredDiff = diff.mul(diff);

    double mse = cv::sum(squaredDiff)[0] / (imageA.rows * imageA.cols);

    return mse;
}

void MSE::printMSE()
{
    std::cout << this->mseValue << std::endl;
}

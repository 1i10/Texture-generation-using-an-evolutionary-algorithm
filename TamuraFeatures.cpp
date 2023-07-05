#include "TamuraFeatures.h"
#include <omp.h>

void TamuraFeatures::calculateCoarseness(cv::Mat image)
{
    //1
    //Применение фильтра Лапласа Гауссиана (LoG)
    cv::Mat edges;
    Laplacian(image, edges, CV_32F, 3);

    //2
    cv::Mat connectedEdges;
    //преобразования изображения в двоичное изображение, где значения пикселей либо равны 0, либо равны 1
    cv::threshold(edges, connectedEdges, 0, 255, cv::THRESH_BINARY);

    // соединяем ребра морфологическим преобразованием (два и более несвязанных ненулевых соседа)
    cv::Mat connectedEdgesProcessed = morphConnectingEdges(connectedEdges);
    
    //3
    // Вычисляем образ "текселей" L как отрицательное преобразование связанного изображения ребер E'
    cv::Mat texelsImage = 1 - connectedEdgesProcessed;

    // Маркируем "тексели" в изображении L
    cv::Mat labelMat;
    cv::connectedComponents(texelsImage, labelMat);

    //грубость
	this->coarseness = calculateCoarsenessFromFormula(labelMat);
}

void TamuraFeatures::calculateContrast(cv::Mat image)
{
    double sumContrast = 0.0;
    int numPixels = image.rows * image.cols;

    //вычисляем контраст для каждого пикселя
    #pragma omp parallel for reduction(+:sumContrast)
    for (int i = 0; i < image.rows; i++) 
    {
        for (int j = 0; j < image.cols; j++) 
        {
            cv::Mat neighborhoodsLocation = image(cv::Range(std::max(0, i - 1), std::min(image.rows, i + 2)),
                cv::Range(std::max(0, j - 1), std::min(image.cols, j + 2))); // Извлекаем окрестность 3x3 (или меньше, если граничные)
            
            double localContrast = calculateContrastInLocation(neighborhoodsLocation); // Вычисляем контраст для окрестности
            
            sumContrast += localContrast;
        }
    }
    
    // Вычисляем среднее значение контраста для всего изображения
	this->contrast = sumContrast/numPixels;
}

void TamuraFeatures::calculateDirectionality(cv::Mat image)
{
    int rows = image.rows;
    int cols = image.cols;

    double sumDirectionality = 0.0;
    int numPixels = rows * cols;

    //оператор Собеля для горизонтальной производной
    cv::Mat sobelX;
    Sobel(image, sobelX, CV_32F, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);

    //оператор Собеля для горизонтальной производной
    cv::Mat sobelY;
    Sobel(image, sobelY, CV_32F, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);

    // Смещение
    const int offsetX[] = { 0, 0, 1, 0, -1, 1, -1, 1, -1 };
    const int offsetY[] = { 0, 1, 0, -1, 0, 1, -1, -1, 1 };
    const int localMapSize = 9;

    std::vector<double> threadSumDirectionality(omp_get_max_threads(), 0.0);

    #pragma omp parallel
    {
        int threadID = omp_get_thread_num();
        // Массивы для хранения вычисленных значений магнитуды и направления
        std::vector<std::vector<double>> magnitudes(rows, std::vector<double>(cols, 0.0));
        std::vector<std::vector<double>> directions(rows, std::vector<double>(cols, 0.0));

        // Вычисление магнитуды и направления для каждого пикселя
        #pragma omp for
        for (int x = 0; x < rows; x++)
        {
            for (int y = 0; y < cols; y++)
            {
                double sumSin = 0.0;
                double sumCos = 0.0;

                for (int k = 0; k < localMapSize; k++)
                {
                    int tmpX = x + offsetX[k];
                    int tmpY = y + offsetY[k];

                    if (tmpX >= 0 && tmpX < image.rows &&
                        tmpY >= 0 && tmpY < image.cols)
                    {
                        double magnitude = 0.0;
                        double direction = 0.0;
                        // Проверка наличия вычисленных значений в массивах
                        if (magnitudes[tmpX][tmpY] == 0 && directions[tmpX][tmpY] == 0)
                        {
                            // Вычисление магнитуды и направления для каждого пикселя в окрестности
                            double dx = sobelX.at<float>(tmpX, tmpY);
                            double dy = sobelY.at<float>(tmpX, tmpY);
                            magnitude = sqrt(dx * dx + dy * dy);
                            direction = atan2(dy, dx) + CV_PI / 2.0;
                            // Сохранение вычисленных значений в массивах
                            magnitudes[tmpX][tmpY] = magnitude;
                            directions[tmpX][tmpY] = direction;
                        }
                        else
                        {
                            magnitude = magnitudes[tmpX][tmpY];
                            direction = directions[tmpX][tmpY];
                        }
                        // Промежуточные значения для формулы направленности
                        sumSin += magnitude * sin(direction);
                        sumCos += magnitude * cos(direction);
                    }
                }
                // Вычисление направленности
                double directionality = atan2(sumSin, sumCos);
                threadSumDirectionality[threadID] += directionality;
            }
        }
    }

    for (double threadDirectionality : threadSumDirectionality)
    {
        sumDirectionality += threadDirectionality;
    }

    // Вычисление средней направленности
    this->directionality = sumDirectionality / numPixels;
}

void TamuraFeatures::calculateLinelikeness(cv::Mat image, int n, int w)
{
    double totalLineLikeness = 0.0;
    int countPixels = image.rows * image.cols;

    // Вычиляем линейность для каждого пикселя
    #pragma omp parallel for reduction(+:totalLineLikeness)
    for (int x = 0; x < image.rows; ++x) 
    {
        for (int y = 0; y < image.cols; ++y) 
        {
            double lineLikeness = calculatePixelLineLikeness(image, x, y, n, w);
            totalLineLikeness += lineLikeness;
        }
    }

    // Линейность
    this->linelikeness = totalLineLikeness / countPixels;
}

void TamuraFeatures::calculateRegularity(cv::Mat image)
{
    double sumRegularity = 0.0;
    //вычисляем регулярность для каждого пикселя
    #pragma omp parallel for reduction(+:sumRegularity)
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            cv::Mat neighborhoodsLocation = image(cv::Range(std::max(0, i - 1), std::min(image.rows, i + 2)),
                cv::Range(std::max(0, j - 1), std::min(image.cols, j + 2))); // Извлекаем окрестность 3x3 (или меньше, если граничные)

            double localRegularity = calculateRegularityInLocation(neighborhoodsLocation); // Вычисляем регулярность для окрестности

            sumRegularity += localRegularity;
        }
    }

    // Вычисляем среднее значение контраста для всего изображения
    this->regularity = sumRegularity / (image.rows * image.cols);
}

void TamuraFeatures::calculateRoughness(cv::Mat image)
{
    double sumRoughness = 0.0;
    
    //Применение фильтра Лапласа Гауссиана (LoG)
    cv::Mat edges;
    Laplacian(image, edges, CV_64F, 3);

    // Применяем бинаризацию
    threshold(edges, edges, 0, 1, cv::THRESH_BINARY);
    
    //вычисляем шероховатость для каждого пикселя
    #pragma omp parallel for reduction(+:sumRoughness)
    for (int i = 0; i < edges.rows; i++)
    {
        for (int j = 0; j < edges.cols; j++)
        {
            cv::Mat neighborhoodsLocation = edges(cv::Range(std::max(0, i - 1), std::min(edges.rows, i + 2)),
                cv::Range(std::max(0, j - 1), std::min(edges.cols, j + 2))); // Извлекаем окрестность 3x3 (или меньше, если граничные)

            double roughnessPixel = cv::sum(neighborhoodsLocation)[0] / (neighborhoodsLocation.rows * neighborhoodsLocation.cols);
            sumRoughness += roughnessPixel;
        }
    }

	this->roughness = sumRoughness/(edges.rows * edges.cols);
}

void TamuraFeatures::printFeatures()
{
	std::cout << "coarseness: " << this->coarseness << "\tcontrast: " << this->contrast
		<< "\tdirectionality: " << this->directionality << "\tlinelikeness: " << this->linelikeness
		<< "\tregularity: " << this->regularity << "\troughness: " << this->roughness << '\n';
}

cv::Mat TamuraFeatures::morphConnectingEdges(cv::Mat edges)
{
    // Создаем новую матрицу для обработанных ребер
    cv::Mat connectedEdgesProcessed = cv::Mat::zeros(edges.size(), CV_8UC1);

    //смещение до соседних пикселей
    const int dx[] = { 0,1,0,-1,1,-1,1,-1 };
    const int dy[] = { 1,0,-1,0,1,-1,-1,1 };
    const int connectivity = 8;

    // соединяем ребра морфологическим преобразованием (два и более несвязанных ненулевых соседа)
    for (int x = 0; x < edges.rows; ++x)
    {
        for (int y = 0; y < edges.cols; ++y)
        {
            if (edges.at<int>(x, y) == 0)
            {
                // Ищем количество соседей
                int neighbors = 0;
                for (int k = 0; k < connectivity; ++k)
                {
                    int tmpX = x + dx[k];
                    int tmpY = y + dy[k];

                    if (tmpX >= 0 && tmpX < edges.rows &&
                        tmpY >= 0 && tmpY < edges.cols &&
                        edges.at<int>(tmpX, tmpY) != 0)
                    {
                        neighbors++;
                    }
                }

                if (neighbors >= 2)
                {
                    connectedEdgesProcessed.at<uchar>(x, y) = 255;
                }
                else
                {
                    connectedEdgesProcessed.at<uchar>(x, y) = edges.at<int>(x, y);
                }
            }
            else
            {
                connectedEdgesProcessed.at<uchar>(x, y) = 255;
            }
        }
    }

    return connectedEdgesProcessed;
}

int TamuraFeatures::diffTexels3x3(cv::Mat texelsImage, int x, int y)
{
    std::set<int> uniqueTexels;

    //смещение
    const int dx[] = { 0,0,1,0,-1,1,-1,1,-1 };
    const int dy[] = { 0,1,0,-1,0,1,-1,-1,1 };
    const int localMapSize = 9;

    for (int k = 0; k < localMapSize; ++k)
    {
        int tmpX = x + dx[k];
        int tmpY = y + dy[k];

        if (tmpX >= 0 && tmpX < texelsImage.rows &&
            tmpY >= 0 && tmpY < texelsImage.cols)
        {
            int texel = texelsImage.at<int>(tmpX, tmpY);
            if (texel != 0)
            {
                uniqueTexels.insert(texel);
            }
        }
    }

    int countUniqTexels = uniqueTexels.size();

    return countUniqTexels;
}

double TamuraFeatures::calculateCoarsenessFromFormula(cv::Mat texelsImage)
{
    double totalCoarseness = 0.0;
    //окрестность
    int M = 3;
    int N = 3;

    #pragma omp parallel for reduction(+:totalCoarseness)
    for (int x = 0; x < texelsImage.rows; ++x)
    {
        for (int y = 0; y < texelsImage.cols; ++y)
        {
            if (texelsImage.at<int>(x, y) != 0)
            {
                int numUniqueTexels = diffTexels3x3(texelsImage, x, y);

                double localCoarseness = numUniqueTexels/ (M * N * 1.0f);
                totalCoarseness += localCoarseness;
            }
        }
    }

    int numPixels = texelsImage.rows * texelsImage.cols;
    
    return totalCoarseness / numPixels;
}

double TamuraFeatures::calculateContrastInLocation(cv::Mat location)
{
    double localContrast = 0.0;

    double Imin, Imax;
    cv::minMaxLoc(location, &Imin, &Imax); // Находим минимальное и максимальное значение интенсивности пикселей в окрестности

    double r = (Imax - Imin) / (Imax + Imin); // Вычисляем динамический диапазон серого

    //гистограмма для окрестности location
    cv::Mat histogram;
    int histSize = 256; // Размер гистограммы
    float range[] = { 0, 256 }; // Диапазон интенсивностей пикселей
    const float* histRange = { range };
    // Вычисляем гистограмму для окрестности
    calcHist(&location, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange); 

    // Нормализуем гистограмму
    normalize(histogram, histogram, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    // Вычисляем разность между соседними значениями гистограммы
    cv::Mat histDerivative;
    absdiff(histogram.rowRange(0, histSize - 1), histogram.rowRange(1, histSize), histDerivative);
    // Суммируем абсолютные значения разностей
    cv::Mat histDerivativeAbs;
    reduce(histDerivative, histDerivativeAbs, 0, cv::REDUCE_SUM, CV_32F);

    double skewness = sum(histDerivativeAbs)(0);
    double kurtosis = sum(histogram.mul(histogram))(0);

    double p = kurtosis / (1 + skewness);

    localContrast = r / p; // Вычисляем значение контраста

    return localContrast;
}

double TamuraFeatures::calculatePixelLineLikeness(cv::Mat image, int x, int y, int n, int w)
{
    double sum = 0.0;
    double meanDirection = 0.0;
    int count = 0;

    // Среднее значение направления в окне
    for (int i = x - w; i <= x + w; ++i) 
    {
        for (int j = y - w; j <= y + w; ++j) 
        {
            if (i >= 0 && i < image.rows && j >= 0 && j < image.cols) 
            {
                double direction = static_cast<double>(image.at<uchar>(i, j));
                direction = direction * (n / 256.0);  // Quantization
                meanDirection += direction;
                count++;
            }
        }
    }
    meanDirection /= count;
    meanDirection *= (CV_PI / n);

    // Линейность для каждого пикселя
    for (int i = x - w; i <= x + w; ++i) 
    {
        for (int j = y - w; j <= y + w; ++j) 
        {
            if (i >= 0 && i < image.rows && j >= 0 && j < image.cols) 
            {
                double direction = static_cast<double>(image.at<uchar>(i, j));
                direction = direction * (n / 256.0);  // Quantization
                direction *= (CV_PI / n);
                sum += (direction - meanDirection) * (direction - meanDirection);
            }
        }
    }

    double lineLikeness = 1.0 - (sum / ((2 * w + 1) * (2 * w + 1)));

    return lineLikeness;
}

double TamuraFeatures::calculateRegularityInLocation(cv::Mat location)
{
    //вычисление автокорреляции
    cv::Mat correlation;
    cv::matchTemplate(location, location, correlation, cv::TM_CCORR_NORMED);
   
    //максимальное значение автокорреляции
    double maxCorrelation;
    cv::minMaxLoc(correlation, nullptr, &maxCorrelation);

    cv::Scalar sumCorrelation = cv::sum(correlation);
    // сумма значений всех локальных пиков автокорреляции, исключая максимальное значение
    double sumPeakCorrelation = sumCorrelation[0] - maxCorrelation;

    //регулярность
    double regularity = sumPeakCorrelation != 0 ? 1.0 - maxCorrelation / sumPeakCorrelation : 0.0;

    return regularity;
}

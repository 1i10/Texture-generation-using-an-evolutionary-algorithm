#include "EvolutionaryAlgorithm.h"
#include <random>
#include "Random.h"

ImageData EvolutionaryAlgorithm::getBestIndivid() const
{
    return bestIndivid;
}

void EvolutionaryAlgorithm::calculateRandomWeights()
{
    // Создание объекта генератора случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());

    // Определение диапазона случайных чисел
    double minNumber = 0.0;
    double maxNumber = 1.0;
    std::uniform_real_distribution<double> dist(minNumber, maxNumber);

    // Генерация случайных чисел
    std::vector<double> randomNumbers(this->weightsNum);
    double sumRandomNumbers = 0.0;
    for (int j = 0; j < this->weightsNum; j++)
    {
        randomNumbers[j] = dist(gen);
        sumRandomNumbers += randomNumbers[j];
    }

    // Вычисление весов Тамуры
    double sumWeights = 0.0; // Сумма весов
    for (int i = 0; i < this->weightsNum - 1; i++)
    {
        this->weights.push_back(randomNumbers[i] / sumRandomNumbers);
        sumWeights += this->weights[i];
    }

    this->weights.push_back(1.0 - sumWeights); // Вес для компоненты MSE
}

void EvolutionaryAlgorithm::crossover(std::vector<ImageData>& parentPopulation, std::vector<ImageData>& offspringPopulation)
{
    // Генерация случайной перестановки индексов для турнирного отбора
    std::vector<int> index1(parentPopulation.size());
    std::vector<int> index2(parentPopulation.size());
    randomPermutation(index1.data(), static_cast<int>(parentPopulation.size()));
    randomPermutation(index2.data(), static_cast<int>(parentPopulation.size()));

    for (int i = 0; i < static_cast<int>(parentPopulation.size()) / 2; ++i)
    {
        ImageData parent1 = selectionTournamentByRank(parentPopulation[index1[2 * i]], parentPopulation[index1[2 * i + 1]]);
        ImageData parent2 = selectionTournamentByRank(parentPopulation[index2[2 * i]], parentPopulation[index2[2 * i + 1]]);
        ImageData offspring1(parent1.getPixelMatrix().size());
        ImageData offspring2(parent1.getPixelMatrix().size());
        sbx(parent1, parent2, offspring1, offspring2);

        offspringPopulation.push_back(offspring1);
        offspringPopulation.push_back(offspring2);
    }
}

ImageData EvolutionaryAlgorithm::selectionTournamentByRank(ImageData individ1, ImageData individ2)
{
    if (individ1.getRank() < individ2.getRank())
    {
        return individ1;
    }
    else if (individ2.getRank() < individ1.getRank())
    {
        return individ1;
    }
    else
    {
        if (randomperc() <= 0.5)
            return (individ1);
        else
            return (individ1);
    }
}

ImageData EvolutionaryAlgorithm::selectionTournamentByFitness(ImageData individ1, ImageData individ2, bool isGreaterFitness)
{
    if (individ1.getTargetFunction() < individ2.getTargetFunction())
    {
        return isGreaterFitness ? individ2 : individ1;
    }
    else if (individ2.getTargetFunction() < individ1.getTargetFunction())
    {
        return isGreaterFitness ? individ1 : individ2;
    }
    else
    {
        if (randomperc() <= 0.5)
            return (individ1);
        else
            return (individ2);
    }
}

void EvolutionaryAlgorithm::sbx(ImageData& parent1, ImageData& parent2, ImageData& offspring1, ImageData& offspring2)
{
    double rand;
    double y1, y2;
    double yl = 0; // Нижняя граница значения пикселя (0 для серого изображения)
    double yu = 255; // Верхняя граница значения пикселя (255 для серого изображения)
    double c1, c2;
    double alpha, beta, betaq;
    double etaC = 20.0;

    cv::Mat pixelsMat1 = parent1.getPixelMatrix();
    cv::Mat pixelsMat2 = parent2.getPixelMatrix();
    int width = pixelsMat1.cols;
    int height = pixelsMat1.rows;

    if (randomperc() <= this->crossoverProbability)
    {
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (randomperc() <= 0.5)
                {
                    uchar pixel1 = pixelsMat1.at<uchar>(i, j);
                    uchar pixel2 = pixelsMat2.at<uchar>(i, j);

                    y1 = pixel1;
                    y2 = pixel2;

                    if (fabs(y1 - y2) > 1e-9)
                    {
                        if (y1 < y2)
                        {
                            y1 = pixel1;
                            y2 = pixel2;
                        }
                        else
                        {
                            y1 = pixel2;
                            y2 = pixel1;
                        }

                        rand = randomperc();
                        beta = 1.0 + (2.0 * (y1 - yl) / (y2 - y1));
                        alpha = 2.0 - pow(beta, -(etaC + 1.0));
                        if (rand <= (1.0 / alpha))
                        {
                            betaq = pow((rand * alpha), (1.0 / (etaC + 1.0)));
                        }
                        else
                        {
                            betaq = pow((1.0 / (2.0 - rand * alpha)), (1.0 / (etaC + 1.0)));
                        }
                        c1 = 0.5 * ((y1 + y2) - betaq * (y2 - y1));

                        beta = 1.0 + (2.0 * (yu - y2) / (y2 - y1));
                        alpha = 2.0 - pow(beta, -(etaC + 1.0));
                        if (rand <= (1.0 / alpha))
                        {
                            betaq = pow((rand * alpha), (1.0 / (etaC + 1.0)));
                        }
                        else
                        {
                            betaq = pow((1.0 / (2.0 - rand * alpha)), (1.0 / (etaC + 1.0)));
                        }
                        c2 = 0.5 * ((y1 + y2) + betaq * (y2 - y1));

                        // Проверка границ значений пикселя
                        if (c1 < yl)
                            c1 = yl;
                        if (c2 < yl)
                            c2 = yl;
                        if (c1 > yu)
                            c1 = yu;
                        if (c2 > yu)
                            c2 = yu;

                        if (randomperc() <= 0.5)
                        {
                            offspring1.getPixelMatrix().at<uchar>(i, j) = static_cast<uchar>(c2);
                            offspring2.getPixelMatrix().at<uchar>(i, j) = static_cast<uchar>(c1);
                        }
                        else
                        {
                            offspring1.getPixelMatrix().at<uchar>(i, j) = static_cast<uchar>(c1);
                            offspring2.getPixelMatrix().at<uchar>(i, j) = static_cast<uchar>(c2);
                        }
                    }
                    else
                    {
                        offspring1.getPixelMatrix().at<uchar>(i, j) = pixel1;
                        offspring2.getPixelMatrix().at<uchar>(i, j) = pixel2;
                    }
                }
                else
                {
                    offspring1.getPixelMatrix().at<uchar>(i, j) = pixelsMat1.at<uchar>(i, j);
                    offspring2.getPixelMatrix().at<uchar>(i, j) = pixelsMat2.at<uchar>(i, j);
                }
            }
        }
    }
    else
    {
        // Если случайное число больше вероятности кроссовера, то просто копируем значения родителей в потомков
        offspring1.setPixelMatrix(pixelsMat1);
        offspring2.setPixelMatrix(pixelsMat2);
    }
}

void EvolutionaryAlgorithm::polynomialMutation(std::vector<ImageData>& population, int populationSize)
{
    double yl = 0;
    double yu = 255;
    double etaM = 20.0;

    for (int i = 0; i < populationSize; ++i)
    {
        cv::Mat image = population[i].getPixelMatrix();

        int rows = image.rows;
        int cols = image.cols;

        double rnd, delta1, delta2, mutPow, deltaq;
        double y, val, xy;

        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                y = image.at<uchar>(row, col);

                rnd = randomperc();
                if (rnd <= this->mutationProbability)
                {
                    delta1 = (y - yl) / (yu - yl);
                    delta2 = (yu - y) / (yu - yl);
                    rnd = randomperc();
                    mutPow = 1.0 / (etaM + 1.0);

                    if (rnd <= 0.5)
                    {
                        xy = 1.0 - delta1;
                        val = 2.0 * rnd + (1.0 - 2.0 * rnd) * (pow(xy, (etaM + 1.0)));
                        deltaq = pow(val, mutPow) - 1.0;
                    }
                    else
                    {
                        xy = 1.0 - delta2;
                        val = 2.0 * (1.0 - rnd) + 2.0 * (rnd - 0.5) * (pow(xy, (etaM + 1.0)));
                        deltaq = 1.0 - (pow(val, mutPow));
                    }

                    y = y + deltaq * (yu - yl);
                }

                if (y < yl)
                    y = yl;
                else if (y > yu)
                    y = yu;

                image.at<uchar>(row, col) = static_cast<uchar>(y);
            }
        }

        image.copyTo(population[i].getPixelMatrix());
    }
}

void EvolutionaryAlgorithm::updateBestIndivid(ImageData individ, bool isGreaterFitness)
{
    if (individ.getTargetFunction() < bestIndivid.getTargetFunction())
    {
        if (!isGreaterFitness)//минимизация, иначе лучший индвид не меняется
        {
            this->bestIndivid = individ;
        }
    }
    else
    {
        if (isGreaterFitness)
        {
            this->bestIndivid = individ;
        }
    }
}

void EvolutionaryAlgorithm::mergePopulation(std::vector<ImageData>& population, int populationSize, std::vector<ImageData>& offspringPopulation, int ofsPopulationSize, std::vector<ImageData>& mixedPopulation)
{
    mixedPopulation.reserve(populationSize + ofsPopulationSize); // Оптимизация для предотвращения нескольких перевыделений памяти

    // Добавляем элементы из исходной популяции
    for (int i = 0; i < populationSize; ++i)
    {
        mixedPopulation.push_back(population[i]);
    }

    // Добавляем элементы из популяции потомков
    for (int i = 0; i < ofsPopulationSize; ++i)
    {
        mixedPopulation.push_back(offspringPopulation[i]);
    }
}

void EvolutionaryAlgorithm::nonDominatedSort(std::vector<ImageData>& population)
{
    int popSize = population.size();

    std::vector<int> numDominatingPoints(popSize, 0); // сохранить количество точек, которые доминируют над i-м решением
    std::vector<std::vector<int>> solutionIndices(popSize, std::vector<int>(popSize, 0)); // сохранить индекс решения, i-е решение которого доминирует
    std::vector<int> zeroDominatingPoints(popSize, 0); // сохранить решение, которое numDominatedPoints равно 0
    std::vector<int> dominateNum(popSize, 0); // сохранить количество доминирующих точек i-го решения

    for (int i = 0; i < popSize; i++)
    {
        ImageData& individTempA = population[i];
        int index = 0;
        for (int j = 0; j < popSize; j++)
        {
            if (i == j)
            {
                continue;
            }

            ImageData& individTempB = population[j];
            int dominateRelation = checkDominance(individTempA, individTempB);
            if (dominateRelation == 1)
            {
                solutionIndices[i][index++] = j;
            }
            else if (dominateRelation == -1)
            {
                numDominatingPoints[i]++;
            }
        }
        dominateNum[i] = index;
    }

    int unrankNum = popSize;
    int currentRank = 0;
    while (unrankNum > 0)
    {
        int index = 0;
        for (int i = 0; i < popSize; i++)
        {
            if (numDominatingPoints[i] == 0)
            {
                population[i].setRank(currentRank);
                zeroDominatingPoints[index++] = i;
                unrankNum--;
                numDominatingPoints[i] = -1;
            }
        }
        currentRank++;
        for (int i = 0; i < index; i++)
        {
            for (int j = 0; j < dominateNum[zeroDominatingPoints[i]]; j++)
            {
                numDominatingPoints[solutionIndices[zeroDominatingPoints[i]][j]]--;
            }
        }
    }
}

int EvolutionaryAlgorithm::checkDominance(ImageData individ1, ImageData individ2)
{
    int flag1 = 0, flag2 = 0;

    TamuraFeatures tIndivid1 = individ1.getTamuraFeatures();
    TamuraFeatures tIndivid2 = individ2.getTamuraFeatures();
    MSE mIndivid1 = individ1.getMSE();
    MSE mIndivid2 = individ2.getMSE();

    if (tIndivid1.getCoarseness() < tIndivid2.getCoarseness())
        flag1 = 1;
    else if (tIndivid1.getCoarseness() > tIndivid2.getCoarseness())
        flag2 = 1;

    if (tIndivid1.getContrast() < tIndivid2.getContrast())
        flag1 = 1;
    else if (tIndivid1.getContrast() > tIndivid2.getContrast())
        flag2 = 1;

    if (tIndivid1.getDirectionality() < tIndivid2.getDirectionality())
        flag1 = 1;
    else if (tIndivid1.getDirectionality() > tIndivid2.getDirectionality())
        flag2 = 1;

    if (tIndivid1.getLinelikeness() < tIndivid2.getLinelikeness())
        flag1 = 1;
    else if (tIndivid1.getLinelikeness() > tIndivid2.getLinelikeness())
        flag2 = 1;

    if (tIndivid1.getRegularity() < tIndivid2.getRegularity())
        flag1 = 1;
    else if (tIndivid1.getRegularity() > tIndivid2.getRegularity())
        flag2 = 1;

    if (tIndivid1.getRoughness() < tIndivid2.getRoughness())
        flag1 = 1;
    else if (tIndivid1.getRoughness() > tIndivid2.getRoughness())
        flag2 = 1;

    if (mIndivid1.getMSE() < mIndivid2.getMSE())
        flag1 = 1;
    else if (mIndivid1.getMSE() > mIndivid2.getMSE())
        flag2 = 1;

    if (flag1 == 1 && flag2 == 0)
        return 1;
    else if (flag1 == 0 && flag2 == 1)
        return -1;
    else
        return 0;
}


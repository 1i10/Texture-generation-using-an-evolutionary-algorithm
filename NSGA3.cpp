#include "NSGA3.h"
#include <numeric>

void NSGA3::run()
{
    init();

    std::vector<ImageData> offspringPopulation;

    // 3. Основной цикл по поколениям
    for (int generation = 0; generation < numGenerations; ++generation)
    {
        // 4. Генерация потомков
        offspringPopulation.clear();
        this->crossover(this->population, offspringPopulation);
        this->polynomialMutation(offspringPopulation, offspringPopulation.size());

        // 5. Оценка потомков
        for (ImageData& offspring : offspringPopulation)
        {
            offspring.calculateTamuraFeatures();
            offspring.calculateMSE(offspringPopulation);
            offspring.evaluateTargetFunction(this->weights);
            this->updateBestIndivid(offspring, true);
        }

        // 6. Слияние популяций
        std::vector<ImageData> mixedPopulation;
        this->mergePopulation(this->population, this->population.size(), offspringPopulation, offspringPopulation.size(), mixedPopulation);

        // 7. Выбор следующего поколения
        environmentalSelection(this->population, mixedPopulation);

        emit bestIndividChanged(this->bestIndivid, generation + 1);
    }

    emit finished();
}

void NSGA3::init()
{
    // 1. Инициализация начальной популяции
    //    population уже содержит начальную популяцию, переданную через конструктор
    // 2. Вычисление целевой функции для каждого индивида в начальной популяции

    //установим лучшего индивида
    bestIndivid = this->population[0];

    //определим веса
    calculateRandomWeights();

    for (ImageData& individual : this->population)
    {
        individual.evaluateTargetFunction(this->weights);
        this->updateBestIndivid(individual, true);
    }
    emit bestIndividChanged(this->bestIndivid, 0);
}

void NSGA3::environmentalSelection(std::vector<ImageData>& population, std::vector<ImageData>& mixedPopulation)
{
    int mixpopSize = mixedPopulation.size();
    int ndpopSize = 0;

    // получаем недоминирующую популяцию
    getNonDominatedPopulation(mixedPopulation, mixpopSize, nonDominatedPopulation, ndpopSize);

    if (ndpopSize == static_cast<int>(population.size()))
    {
        //полное копирование недоминируемой популяции в следующее поколение
        population = nonDominatedPopulation;
        return;
    }

    // Сортируем недоминирующую популяцию по приближению к оптимальному фронту
    crowdingDistanceSorting(nonDominatedPopulation, ndpopSize);

    // Выбираем особи на основе их приближенности к оптимальному фронту
    for (int i = 0; i < static_cast<int>(population.size()); i++)
    {
        population[i] = nonDominatedPopulation[i];
    }
}

void NSGA3::getNonDominatedPopulation(std::vector<ImageData>& mixedPopulation, int mixpopSize, std::vector<ImageData>& nonDominatedPopulation, int& ndpopSize)
{
    // Выполняем недоминирующую сортировку
    nonDominatedSort(mixedPopulation);

    // Определяем количество особей, необходимое для следующего поколения
    int currentPopNum = 0;
    int tempNumber = 0;
    int rankIndex = 0;

    while (1)
    {
        tempNumber = 0;
        for (int i = 0; i < mixpopSize; i++)
        {
            if (mixedPopulation[i].getRank() == rankIndex)
            {
                tempNumber++;
            }
        }
        if (currentPopNum + tempNumber < weightsNum)
        {
            currentPopNum += tempNumber;
            rankIndex++;
        }
        else
            break;
    }

    ndpopSize = currentPopNum + tempNumber;

    nonDominatedPopulation.resize(ndpopSize);
    int index = 0;
    for (int i = 0; i < mixpopSize; i++)
    {
        if (mixedPopulation[i].getRank() <= rankIndex)
        {
            nonDominatedPopulation[index++] = mixedPopulation[i];
        }
    }
}

void NSGA3::crowdingDistanceSorting(std::vector<ImageData>& population, int popSize)
{
    int numObjectives = 7; // Количество целевых функций

    std::vector<std::vector<double>> objectiveValues(popSize, std::vector<double>(numObjectives));

    // Заполняем значения приближенности для каждой особи
    for (int i = 0; i < popSize; i++)
    {
        TamuraFeatures tamuraFeatures = population[i].getTamuraFeatures();
        MSE mse = population[i].getMSE();

        // Значения целевых функций TamuraFeatures
        objectiveValues[i][0] = tamuraFeatures.getCoarseness();
        objectiveValues[i][1] = tamuraFeatures.getContrast();
        objectiveValues[i][2] = tamuraFeatures.getDirectionality();
        objectiveValues[i][3] = tamuraFeatures.getLinelikeness();
        objectiveValues[i][4] = tamuraFeatures.getRegularity();
        objectiveValues[i][5] = tamuraFeatures.getRoughness();
        // Значение целевой функции MSE
        objectiveValues[i][6] = mse.getMSE();
    }

    // Создаем вектор индексов для сортировки
    std::vector<int> indices(popSize);
    std::iota(indices.begin(), indices.end(), 0);

    // Сортируем популяцию на основе совокупного расстояния
    std::sort(indices.begin(), indices.end(), [&objectiveValues, numObjectives](int a, int b) {
        double crowdingDistanceA = 0.0;
        double crowdingDistanceB = 0.0;

        for (int obj = 0; obj < numObjectives; obj++) 
        {
            crowdingDistanceA += objectiveValues[a][obj];
            crowdingDistanceB += objectiveValues[b][obj];
        }

        return crowdingDistanceA > crowdingDistanceB;
        });

    // Создаем временный вектор для хранения отсортированной популяции
    std::vector<ImageData> sortedPopulation(popSize);

    // Копируем особи из отсортированного порядка во временный вектор
    for (int i = 0; i < popSize; ++i) 
    {
        sortedPopulation[i] = population[indices[i]];
    }

    // Копируем отсортированные особи обратно в исходный вектор популяции
    population = sortedPopulation;
}

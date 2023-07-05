#pragma once
#include <vector>
#include "ImageData.h"
#include <QObject>

class EvolutionaryAlgorithm : public QObject
{
    Q_OBJECT
protected:
    std::vector<ImageData> population;
    int numGenerations;
    double mutationProbability;
    double crossoverProbability;
    ImageData bestIndivid;
    std::vector<double> weights;
    int weightsNum;

public:
    EvolutionaryAlgorithm(const std::vector<ImageData>& initialPopulation, int generations, double mutationProb, double crossoverProb, int weightsNum)
        : population(initialPopulation), numGenerations(generations), mutationProbability(mutationProb), crossoverProbability(crossoverProb),
        weightsNum(weightsNum){}


    virtual void run() = 0;

    void calculateRandomWeights();

    void crossover(std::vector<ImageData>& parentPopulation, std::vector<ImageData>& offspringPopulation);
    ImageData selectionTournamentByRank(ImageData individ1, ImageData individ2);
    ImageData selectionTournamentByFitness(ImageData individ1, ImageData individ2, bool isGreaterFitness);//isGreaterFitness = 1 - целевая максимизация
    void sbx(ImageData& parent1, ImageData& parent2, ImageData& offspring1, ImageData& offspring2);

    void polynomialMutation(std::vector<ImageData>& population, int populationSize);

    void updateBestIndivid(ImageData individ, bool isGreaterFitness);//isGreaterFitness = 1 - целевая максимизация

    void mergePopulation(std::vector<ImageData>& population, int populationSize, std::vector<ImageData>& offspringPopulation, int ofsPopulationSize, std::vector<ImageData>& mixedPopulation);

    void nonDominatedSort(std::vector<ImageData>& population);//недоминирующая сортировка и результаты ранга сохраняются в ImageData переменной rank
    int checkDominance(ImageData individ1, ImageData individ2); //определить доминирующую особь

    ImageData getBestIndivid() const;

signals:
    void bestIndividChanged(ImageData individ, int currentGeneration);
};


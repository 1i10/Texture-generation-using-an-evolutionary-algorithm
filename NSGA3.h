#pragma once
#include "EvolutionaryAlgorithm.h"
class NSGA3 : public EvolutionaryAlgorithm
{
    Q_OBJECT
private:
    std::vector<ImageData> nonDominatedPopulation;//недоминирующие точки
public:
    NSGA3(const std::vector<ImageData>& initialPopulation, int generations, double mutationProb, double crossoverProb, int weightsNum)
        : EvolutionaryAlgorithm(initialPopulation, generations, mutationProb, crossoverProb, weightsNum) {}

    void run() override;
    void init();
    void environmentalSelection(std::vector<ImageData>& population, std::vector<ImageData>& mixedPopulation);
    void getNonDominatedPopulation(std::vector<ImageData>& mixedPopulation, int mixpopSize, std::vector<ImageData>& nonDominatedPopulation, int& ndpopSize);
    void crowdingDistanceSorting(std::vector<ImageData>& population, int popSize);

signals:
    void finished();
};


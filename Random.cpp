#include "Random.h"
#include <random>
#include <algorithm>

static double oldrand[55];

void randomPermutation(int* perm, int size)
{
    int* index = new int[size];
    int* flag = new int[size];

    for (int i = 0; i < size; i++)
    {
        index[i] = i;
        flag[i] = 1;
    }

    int num = 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    while (num < size)
    {
        int start = std::uniform_int_distribution<int>(0, size - 1)(gen);
        while (true)
        {
            if (flag[start])
            {
                perm[num] = index[start];
                flag[start] = 0;
                num++;
                break;
            }
            if (start == (size - 1))
                start = 0;
            else
                start++;
        }
    }

    delete[] index;
    delete[] flag;
}

void advanceRandom()
{
    int j1;
    double new_random;
    for (j1 = 0; j1 < 24; j1++)
    {
        new_random = oldrand[j1] - oldrand[j1 + 31];
        if (new_random < 0.0)
        {
            new_random = new_random + 1.0;
        }
        oldrand[j1] = new_random;
    }
    for (j1 = 24; j1 < 55; j1++)
    {
        new_random = oldrand[j1] - oldrand[j1 - 24];
        if (new_random < 0.0)
        {
            new_random = new_random + 1.0;
        }
        oldrand[j1] = new_random;
    }
}

double randomperc()
{
    std::random_device rd;
    std::mt19937 rng(rd());

    std::uniform_real_distribution<double> unif(0, 1);

    return unif(rng);
}

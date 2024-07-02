#include "sigma.h"

// using namespace std;

double Sigma::calcMean(double *vals, uint s)
{
    // uint size = sizeof(vals) / sizeof(vals[0]);
    double total = 0;

    for (int i = 0; i < s; i++)
    {
        total += vals[i];
    }

    return total / s;
}

double Sigma::calcMedian(double *vals, uint s)
{
    // uint size = sizeof(vals) / sizeof(vals[0]);
    std::sort(vals, vals + s);
    if (s % 2 == 0)
    {
        return (vals[s / 2] + vals[(s / 2) + 1]) / 2;
    }
    else
    {
        return vals[(uint)floor(s / 2)];
    }
}

double Sigma::calcMode(double *vals, uint s)
{
    return 0.0;
}

double Sigma::calcDeviation(double *vals, uint samples)
{
    // uint size = sizeof(vals) / sizeof(vals[0]);
    double mean = calcMean(vals, samples);
    double sum = 0;

    for (int i = 0; i < samples; i++)
    {
        sum += pow(vals[i] - mean, 2);
    }

    double variance = sum / (samples);
    return sqrt(variance);
}

double Sigma::calcDeviation(double *vals)
{
    uint size = sizeof(vals) / sizeof(vals[0]);
    double mean = calcMean(vals, size);
    double sum = 0;

    for (int i = 0; i < size; i++)
    {
        sum += pow(vals[i] - mean, 2);
    }

    double variance = sum / size;
    return sqrt(variance);
}

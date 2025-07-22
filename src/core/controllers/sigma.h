
#ifndef _AERPID_SIGMA_H
#define _AERPID_SIGMA_H

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>

// using namespace std;

class Sigma
{
public:
    double calcMean(double *vals, uint s);
    double calcMedian(double *vals, uint s);
    double calcMode(double *vals, uint s);

    double calcDeviation(double *vals, uint size);
    double calcDeviation(double *vals, uint size, uint samples);

protected:
private:
};

#endif
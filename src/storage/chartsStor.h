#ifndef _CHART_STOR_H
#define _CHART_STOR_H

#include "flash.h"

#include "core/charting/ChartSeries.h"

class ChartingStorage
{
public:
    // ChartingStorage();

    void init(Flash *flash);

    void setup();
    void load();
    void save();

    void addEntry(std::string name, double val);

private:
    Flash *flash;

    ChartSeries *series1;
};

extern ChartingStorage chartingStorage;

#endif
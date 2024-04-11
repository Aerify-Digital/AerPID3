#ifndef _CHART_SERIES_H_
#define _CHART_SERIES_H_

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>

#include "util/crc.h"
#include "storage/flash.h"
#include "ChartEntry.h"

class ChartSeries
{
public:
    ChartSeries(Flash *flash, std::string name)
    {
        this->flash = flash;
        this->name = name;
        entries = {};
    }

    bool open();
    bool close();

    std::vector<ChartEntry> get() { return entries; }

    void addEntry(double v);

private:
    // flash storage pointer
    Flash *flash;

    const uint16_t sLength = 2048;

    std::string name;

    std::vector<ChartEntry> entries;
};

#endif
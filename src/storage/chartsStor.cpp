#include "chartsStor.h"

void ChartingStorage::init(Flash *flash)
{
    ChartingStorage::flash = flash;

    setup();
    load();
}

void ChartingStorage::setup()
{
    series1 = new ChartSeries(flash, "tmp");
    // series1->addEntry(25.1);
}

void ChartingStorage::load()
{
    series1->open();
}

void ChartingStorage::save()
{
    series1->close();
}

void ChartingStorage::addEntry(std::string name, double val)
{
    // TODO: setup fetching by name...
    series1->addEntry(val);
}

ChartingStorage chartingStorage;
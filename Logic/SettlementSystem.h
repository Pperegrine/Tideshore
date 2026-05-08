#ifndef SETTLEMENTSYSTEM_H
#define SETTLEMENTSYSTEM_H

#include "../data/Store.h"
#include "../data/DailyReport.h"
#include "CustomerSystem.h"

class SettlementSystem
{
public:
    SettlementSystem();

    DailyReport generateReport(const Store& store, const CustomerSystem& customerSystem, int todayPurchaseCost);
    QString findBestSeller(const QMap<QString, int>& soldCounts);
    QString findWorstSeller(const QMap<QString, int>& soldCounts);
    QString findMainShortage(const QMap<QString, int>& shortageCounts);
};

#endif // SETTLEMENTSYSTEM_H

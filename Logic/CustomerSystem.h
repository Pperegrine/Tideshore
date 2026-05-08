#ifndef CUSTOMERSYSTEM_H
#define CUSTOMERSYSTEM_H

#include <QVector>
#include <QMap>

#include "../data/Store.h"
#include "../data/Product.h"
#include "../data/Event.h"

class CustomerSystem
{
public:
    int lastCustomerFlow;
    int lastRevenue;
    QMap<QString, int> lastSoldCounts;
    QMap<QString, int> lastDemandCounts;
    QMap<QString, int> lastShortageCounts;

    CustomerSystem();

    int calculateCustomerFlow(const Store& store, const QVector<Event>& activeEvents);
    int calculateDemand(const Product& product, int customerFlow, const QVector<Event>& activeEvents);
    int simulateSales(Store& store, const QVector<Event>& activeEvents);
};

#endif // CUSTOMERSYSTEM_H

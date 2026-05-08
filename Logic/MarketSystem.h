#ifndef MARKETSYSTEM_H
#define MARKETSYSTEM_H

#include <QVector>

#include "../data/Store.h"
#include "../data/Event.h"
#include "../data/Product.h"

class MarketSystem
{
public:
    MarketSystem();

    void updateDailyCosts(Store& store, const QVector<Event>& activeEvents);
    void updateSuggestedPrices(Store& store);

    int calculateTodayCost(const Product& product, const QVector<Event>& activeEvents);
    int calculateSuggestedPrice(const Product& product, int analysisLevel);
};

#endif // MARKETSYSTEM_H

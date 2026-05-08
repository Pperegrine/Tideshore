#include "MarketSystem.h"

#include <QtGlobal>
#include <QRandomGenerator>
#include <cmath>

MarketSystem::MarketSystem()
{
}

void MarketSystem::updateDailyCosts(Store& store, const QVector<Event>& activeEvents)
{
    for (int i = 0; i < store.products.size(); ++i) {
        if (store.products[i].unlocked) {
            store.products[i].todayCost = calculateTodayCost(store.products[i], activeEvents);
        }
    }
}

void MarketSystem::updateSuggestedPrices(Store& store)
{
    int effectiveAnalysisLevel = store.analysisLevel;
    for (int i = 0; i < store.employees.size(); ++i) {
        const Employee& employee = store.employees[i];
        if (employee.hired && employee.type == "分析员") {
            effectiveAnalysisLevel += 1;
            break;
        }
    }
    if (effectiveAnalysisLevel > 5) {
        effectiveAnalysisLevel = 5;
    }

    for (int i = 0; i < store.products.size(); ++i) {
        if (store.products[i].unlocked) {
            store.products[i].suggestedPrice = calculateSuggestedPrice(store.products[i], effectiveAnalysisLevel);
        }
    }
}

int MarketSystem::calculateTodayCost(const Product& product, const QVector<Event>& activeEvents)
{
    double finalCost = static_cast<double>(product.baseCost);

    double marketFluctuation =
    0.95 + QRandomGenerator::global()->generateDouble() * (1.05 - 0.95);
    finalCost *= marketFluctuation;

    for (int i = 0; i < activeEvents.size(); ++i) {
        const Event& event = activeEvents[i];
        if (event.costEffects.contains(product.category)) {
            finalCost *= event.costEffects.value(product.category);
        }
    }

    int roundedCost = static_cast<int>(std::round(finalCost));
    if (roundedCost < 1) {
        roundedCost = 1;
    }

    return roundedCost;
}

int MarketSystem::calculateSuggestedPrice(const Product& product, int analysisLevel)
{
    double baseSuggestedPrice = static_cast<double>(product.todayCost) * 1.5;

    double errorRange = 0.20;
    if (analysisLevel == 2) {
        errorRange = 0.15;
    } else if (analysisLevel == 3) {
        errorRange = 0.10;
    } else if (analysisLevel == 4) {
        errorRange = 0.06;
    } else if (analysisLevel >= 5) {
        errorRange = 0.03;
    }

    double errorFactor =
    (1.0 - errorRange) +
    QRandomGenerator::global()->generateDouble() * (2.0 * errorRange);
    double finalSuggestedPrice = baseSuggestedPrice * errorFactor;

    int roundedPrice = static_cast<int>(std::round(finalSuggestedPrice));
    int minimumPrice = product.todayCost + 1;
    if (roundedPrice < minimumPrice) {
        roundedPrice = minimumPrice;
    }

    return roundedPrice;
}

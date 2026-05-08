#include "CustomerSystem.h"

#include <QRandomGenerator>
#include <QtGlobal>

CustomerSystem::CustomerSystem()
    : lastCustomerFlow(0),
      lastRevenue(0)
{
}

int CustomerSystem::calculateCustomerFlow(const Store& store, const QVector<Event>& activeEvents)
{
    int baseFlow = QRandomGenerator::global()->bounded(18, 29);
    double finalFlow = static_cast<double>(baseFlow);

    double levelMultiplier = 1.0 + (store.level - 1) * 0.08;
    finalFlow *= levelMultiplier;

    double cleanlinessMultiplier = 1.0;
    if (store.cleanliness >= 90) {
        cleanlinessMultiplier = 1.05;
    } else if (store.cleanliness >= 70) {
        cleanlinessMultiplier = 1.00;
    } else if (store.cleanliness >= 50) {
        cleanlinessMultiplier = 0.95;
    } else {
        cleanlinessMultiplier = 0.90;
    }
    finalFlow *= cleanlinessMultiplier;

    for (int i = 0; i < activeEvents.size(); ++i) {
        double eventFlowEffect = activeEvents[i].customerFlowEffect;
        if (eventFlowEffect > 0.0) {
            finalFlow *= eventFlowEffect;
        }
    }

    for (int i = 0; i < store.employees.size(); ++i) {
        const Employee& employee = store.employees[i];
        if (employee.hired && employee.type == "宣传员") {
            finalFlow *= 1.08;
            break;
        }
    }

    int roundedFlow = qRound(finalFlow);
    if (roundedFlow < 1) {
        roundedFlow = 1;
    }

    return roundedFlow;
}

int CustomerSystem::calculateDemand(const Product& product, int customerFlow, const QVector<Event>& activeEvents)
{
    if (!product.unlocked) {
        return 0;
    }

    if (product.stock <= 0) {
        return 0;
    }

    double baseDemandFactor = 0.20;
    if (product.demandLevel == 1) {
        baseDemandFactor = 0.08;
    } else if (product.demandLevel == 2) {
        baseDemandFactor = 0.14;
    } else if (product.demandLevel == 3) {
        baseDemandFactor = 0.22;
    } else if (product.demandLevel == 4) {
        baseDemandFactor = 0.32;
    } else if (product.demandLevel == 5) {
        baseDemandFactor = 0.45;
    }

    double demand = static_cast<double>(customerFlow) * baseDemandFactor;

    double priceRatio = 1.0;
    if (product.suggestedPrice > 0) {
        double safePlayerPrice = product.playerPrice > 0 ? static_cast<double>(product.playerPrice) : 0.0;
        priceRatio = safePlayerPrice / static_cast<double>(product.suggestedPrice);
    }

    double priceFactor = 1.0;
    if (priceRatio <= 0.85) {
        priceFactor = 1.20;
    } else if (priceRatio <= 0.95) {
        priceFactor = 1.10;
    } else if (priceRatio <= 1.05) {
        priceFactor = 1.00;
    } else if (priceRatio <= 1.15) {
        priceFactor = 0.88;
    } else if (priceRatio <= 1.30) {
        priceFactor = 0.72;
    } else {
        priceFactor = 0.50;
    }

    if (product.priceSensitivity >= 4 && priceRatio > 1.05) {
        priceFactor *= 0.90;
    } else if (product.priceSensitivity <= 2 && priceRatio > 1.05) {
        priceFactor *= 0.97;
    }

    demand *= priceFactor;

    for (int i = 0; i < activeEvents.size(); ++i) {
        const Event& event = activeEvents[i];
        if (event.demandEffects.contains(product.category)) {
            demand *= event.demandEffects.value(product.category);
        }
    }

    int roundedDemand = qRound(demand);
    if (roundedDemand < 0) {
        roundedDemand = 0;
    }

    return roundedDemand;
}

int CustomerSystem::simulateSales(Store& store, const QVector<Event>& activeEvents)
{
    lastSoldCounts.clear();
    lastDemandCounts.clear();
    lastShortageCounts.clear();

    int customerFlow = calculateCustomerFlow(store, activeEvents);
    lastCustomerFlow = customerFlow;

    int revenue = 0;

    for (int i = 0; i < store.products.size(); ++i) {
        Product& product = store.products[i];
        if (!product.unlocked) {
            continue;
        }

        int demand = calculateDemand(product, customerFlow, activeEvents);
        int soldCount = qMin(product.stock, demand);

        lastDemandCounts[product.name] = demand;
        lastSoldCounts[product.name] = soldCount;

        if (demand > soldCount) {
            lastShortageCounts[product.name] = demand - soldCount;
        }

        product.stock -= soldCount;
        revenue += soldCount * product.playerPrice;
    }

    for (int i = 0; i < store.employees.size(); ++i) {
        const Employee& employee = store.employees[i];
        if (employee.hired && employee.type == "收银员") {
            revenue = qRound(static_cast<double>(revenue) * 1.05);
            break;
        }
    }

    if (revenue < 0) {
        revenue = 0;
    }

    lastRevenue = revenue;
    store.money += revenue;
    return revenue;
}

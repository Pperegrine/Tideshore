#ifndef UPGRADESYSTEM_H
#define UPGRADESYSTEM_H

#include "../data/Store.h"
#include "../data/Product.h"

class UpgradeSystem
{
public:
    UpgradeSystem();

    int getStoreUpgradeCost(const Store& store) const;
    bool upgradeStore(Store& store);

    int getStorageUpgradeCost(const Store& store) const;
    bool upgradeStorage(Store& store);

    int getAnalysisUpgradeCost(const Store& store) const;
    bool upgradeAnalysis(Store& store);

    int getDailyGoodsUnlockCost(const Store& store) const;
    bool unlockDailyGoods(Store& store);
};

#endif // UPGRADESYSTEM_H

#include "UpgradeSystem.h"

UpgradeSystem::UpgradeSystem()
{
}

int UpgradeSystem::getStoreUpgradeCost(const Store& store) const
{
    if (store.level == 1) {
        return 1200;
    }
    if (store.level == 2) {
        return 2200;
    }
    if (store.level == 3) {
        return 3500;
    }
    return -1;
}

bool UpgradeSystem::upgradeStore(Store& store)
{
    int cost = getStoreUpgradeCost(store);
    if (cost < 0 || store.money < cost) {
        return false;
    }

    store.money -= cost;
    store.level += 1;
    store.shelfCount += 2;
    store.storageCapacity += 20;
    return true;
}

int UpgradeSystem::getStorageUpgradeCost(const Store& store) const
{
    if (store.storageLevel == 1) {
        return 600;
    }
    if (store.storageLevel == 2) {
        return 1000;
    }
    if (store.storageLevel == 3) {
        return 1600;
    }
    return -1;
}

bool UpgradeSystem::upgradeStorage(Store& store)
{
    int cost = getStorageUpgradeCost(store);
    if (cost < 0 || store.money < cost) {
        return false;
    }

    store.money -= cost;
    store.storageLevel += 1;

    if (store.storageLevel == 2) {
        store.storageCapacity += 30;
    } else if (store.storageLevel == 3) {
        store.storageCapacity += 40;
    } else if (store.storageLevel == 4) {
        store.storageCapacity += 50;
    }

    return true;
}

int UpgradeSystem::getAnalysisUpgradeCost(const Store& store) const
{
    if (store.analysisLevel == 1) {
        return 500;
    }
    if (store.analysisLevel == 2) {
        return 900;
    }
    if (store.analysisLevel == 3) {
        return 1400;
    }
    if (store.analysisLevel == 4) {
        return 2200;
    }
    return -1;
}

bool UpgradeSystem::upgradeAnalysis(Store& store)
{
    int cost = getAnalysisUpgradeCost(store);
    if (cost < 0 || store.money < cost) {
        return false;
    }

    store.money -= cost;
    store.analysisLevel += 1;
    return true;
}

int UpgradeSystem::getDailyGoodsUnlockCost(const Store& store) const
{
    if (!store.dailyGoodsUnlocked) {
        return 800;
    }
    return -1;
}

bool UpgradeSystem::unlockDailyGoods(Store& store)
{
    int cost = getDailyGoodsUnlockCost(store);
    if (cost < 0 || store.money < cost) {
        return false;
    }

    store.money -= cost;
    store.dailyGoodsUnlocked = true;

    for (int i = 0; i < store.products.size(); ++i) {
        if (store.products[i].category == "日用品") {
            store.products[i].unlocked = true;
        }
    }

    return true;
}

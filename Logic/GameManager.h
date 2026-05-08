#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QVector>
#include <QString>

#include "../data/Store.h"
#include "../data/Event.h"
#include "../data/DailyReport.h"
#include "../data/Product.h"
#include "EventSystem.h"
#include "MarketSystem.h"
#include "CustomerSystem.h"
#include "SettlementSystem.h"
#include "UpgradeSystem.h"

class GameManager
{
public:
    Store store;
    QVector<Event> todayEvents;
    DailyReport todayReport;
    int todayPurchaseCost;
    int todayRevenue;

    GameManager();

    void startNewGame();
    void initializeProducts();
    void initializeEmployees();
    void startNewDay();
    bool purchaseProduct(int productIndex, int quantity);
    bool setProductPrice(int productIndex, int price);
    int startBusiness();
    DailyReport endBusiness();
    int getTotalStock() const;
    const CustomerSystem& getCustomerSystem() const;
    void performCleaning();
    bool hireEmployee(int employeeIndex);
    bool hasEmployeeType(const QString& type) const;
    int getEmployeeCount() const;

    bool upgradeStore();
    bool upgradeStorage();
    bool upgradeAnalysis();
    bool unlockDailyGoods();

    int getStoreUpgradeCost() const;
    int getStorageUpgradeCost() const;
    int getAnalysisUpgradeCost() const;
    int getDailyGoodsUnlockCost() const;
    bool applyLoadedStore(const Store& loadedStore);

private:
    EventSystem eventSystem;
    MarketSystem marketSystem;
    CustomerSystem customerSystem;
    SettlementSystem settlementSystem;
    UpgradeSystem upgradeSystem;

    bool hasEnteredFirstDay;
    bool businessFinishedToday;
    bool reportGeneratedToday;
};

#endif // GAMEMANAGER_H
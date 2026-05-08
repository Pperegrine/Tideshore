#include "GameManager.h"

#include <QtGlobal>
#include <QDebug>

GameManager::GameManager()
    : todayPurchaseCost(0),
      todayRevenue(0),
      hasEnteredFirstDay(false),
      businessFinishedToday(false),
      reportGeneratedToday(false)
{
}

void GameManager::startNewGame()
{
    qDebug() << "[GameManager] startNewGame";

    store = Store();
    todayEvents.clear();
    todayReport = DailyReport();
    todayPurchaseCost = 0;
    todayRevenue = 0;
    hasEnteredFirstDay = false;
    businessFinishedToday = false;
    reportGeneratedToday = false;

    initializeProducts();
    initializeEmployees();
    startNewDay();
}

void GameManager::initializeProducts()
{
    store.products.clear();
    Product water; water.name="矿泉水"; water.category="饮料"; water.baseCost=4; water.todayCost=4; water.suggestedPrice=6; water.playerPrice=6; water.stock=0; water.maxStock=100; water.demandLevel=5; water.priceSensitivity=4; water.eventSensitivity=3; water.fluctuationSensitivity=2; water.unlocked=true;
    Product cannedTea; cannedTea.name="罐装茶饮"; cannedTea.category="饮料"; cannedTea.baseCost=6; cannedTea.todayCost=6; cannedTea.suggestedPrice=9; cannedTea.playerPrice=9; cannedTea.stock=0; cannedTea.maxStock=80; cannedTea.demandLevel=4; cannedTea.priceSensitivity=3; cannedTea.eventSensitivity=2; cannedTea.fluctuationSensitivity=2; cannedTea.unlocked=true;
    Product bread; bread.name="袋装面包"; bread.category="速食"; bread.baseCost=5; bread.todayCost=5; bread.suggestedPrice=8; bread.playerPrice=8; bread.stock=0; bread.maxStock=80; bread.demandLevel=4; bread.priceSensitivity=4; bread.eventSensitivity=2; bread.fluctuationSensitivity=2; bread.unlocked=true;
    Product instantNoodles; instantNoodles.name="速食泡面"; instantNoodles.category="速食"; instantNoodles.baseCost=7; instantNoodles.todayCost=7; instantNoodles.suggestedPrice=11; instantNoodles.playerPrice=11; instantNoodles.stock=0; instantNoodles.maxStock=70; instantNoodles.demandLevel=4; instantNoodles.priceSensitivity=3; instantNoodles.eventSensitivity=5; instantNoodles.fluctuationSensitivity=4; instantNoodles.unlocked=true;
    Product chips; chips.name="薯片"; chips.category="零食"; chips.baseCost=6; chips.todayCost=6; chips.suggestedPrice=10; chips.playerPrice=10; chips.stock=0; chips.maxStock=60; chips.demandLevel=3; chips.priceSensitivity=2; chips.eventSensitivity=2; chips.fluctuationSensitivity=2; chips.unlocked=true;
    Product chocolate; chocolate.name="巧克力"; chocolate.category="零食"; chocolate.baseCost=7; chocolate.todayCost=7; chocolate.suggestedPrice=11; chocolate.playerPrice=11; chocolate.stock=0; chocolate.maxStock=60; chocolate.demandLevel=3; chocolate.priceSensitivity=3; chocolate.eventSensitivity=3; chocolate.fluctuationSensitivity=2; chocolate.unlocked=true;
    Product umbrella; umbrella.name="雨伞"; umbrella.category="日用品"; umbrella.baseCost=10; umbrella.todayCost=10; umbrella.suggestedPrice=16; umbrella.playerPrice=16; umbrella.stock=0; umbrella.maxStock=50; umbrella.demandLevel=1; umbrella.priceSensitivity=1; umbrella.eventSensitivity=5; umbrella.fluctuationSensitivity=5; umbrella.unlocked=false;
    Product emergencyGoods; emergencyGoods.name="电池/应急灯"; emergencyGoods.category="日用品"; emergencyGoods.baseCost=13; emergencyGoods.todayCost=13; emergencyGoods.suggestedPrice=20; emergencyGoods.playerPrice=20; emergencyGoods.stock=0; emergencyGoods.maxStock=40; emergencyGoods.demandLevel=1; emergencyGoods.priceSensitivity=1; emergencyGoods.eventSensitivity=5; emergencyGoods.fluctuationSensitivity=5; emergencyGoods.unlocked=false;
    store.products << water << cannedTea << bread << instantNoodles << chips << chocolate << umbrella << emergencyGoods;
}

void GameManager::initializeEmployees()
{
    store.employees.clear();
    Employee e1; e1.name="小林"; e1.type="收银员"; e1.description="熟练处理结账流程，使营业额小幅提升。"; e1.hireCost=700; e1.dailyCost=20;
    Employee e2; e2.name="青叶"; e2.type="宣传员"; e2.description="擅长招揽顾客，使每日客流小幅增加。"; e2.hireCost=900; e2.dailyCost=25;
    Employee e3; e3.name="遥"; e3.type="分析员"; e3.description="擅长市场分析，使建议售价更加可靠。"; e3.hireCost=1000; e3.dailyCost=30;
    Employee e4; e4.name="森田"; e4.type="理货员"; e4.description="负责整理货架与店面，使清洁度下降减缓。"; e4.hireCost=800; e4.dailyCost=20;
    store.employees << e1 << e2 << e3 << e4;
}

void GameManager::startNewDay()
{
    if (hasEnteredFirstDay) {
        store.currentDay += 1;
    } else {
        hasEnteredFirstDay = true;
    }

    todayPurchaseCost = 0;
    todayRevenue = 0;
    todayReport = DailyReport();
    businessFinishedToday = false;
    reportGeneratedToday = false;

    todayEvents = eventSystem.generateDailyEvents();
    marketSystem.updateDailyCosts(store, todayEvents);
    marketSystem.updateSuggestedPrices(store);

    if (store.cleanliness < 100) {
        store.cleanliness = qMin(100, store.cleanliness + 10);
    }

    qDebug() << "[GameManager] startNewDay day=" << store.currentDay;
}

bool GameManager::purchaseProduct(int productIndex, int quantity)
{
    if (productIndex < 0 || productIndex >= store.products.size()) { qDebug() << "[purchaseProduct] invalid index"; return false; }
    if (quantity <= 0) { qDebug() << "[purchaseProduct] invalid quantity"; return false; }

    Product& product = store.products[productIndex];
    if (!product.unlocked) { qDebug() << "[purchaseProduct] product locked"; return false; }
    if (product.stock + quantity > product.maxStock) { qDebug() << "[purchaseProduct] exceed max stock"; return false; }
    if (getTotalStock() + quantity > store.storageCapacity) { qDebug() << "[purchaseProduct] exceed storage capacity"; return false; }

    int totalCost = product.todayCost * quantity;
    if (store.money < totalCost) { qDebug() << "[purchaseProduct] insufficient money"; return false; }

    store.money -= totalCost;
    product.stock += quantity;
    todayPurchaseCost += totalCost;
    qDebug() << "[purchaseProduct] success" << product.name << quantity;
    return true;
}

bool GameManager::setProductPrice(int productIndex, int price)
{
    if (productIndex < 0 || productIndex >= store.products.size()) { return false; }
    if (price <= 0) { return false; }
    if (!store.products[productIndex].unlocked) { return false; }
    store.products[productIndex].playerPrice = price;
    return true;
}

int GameManager::startBusiness()
{
    if (businessFinishedToday) {
        qDebug() << "[startBusiness] already finished today";
        return todayRevenue;
    }

    todayRevenue = customerSystem.simulateSales(store, todayEvents);
    int cleanlinessDrop = hasEmployeeType("理货员") ? 8 : 15;
    store.cleanliness = qMax(0, store.cleanliness - cleanlinessDrop);

    businessFinishedToday = true;
    qDebug() << "[startBusiness] revenue=" << todayRevenue;
    return todayRevenue;
}

DailyReport GameManager::endBusiness()
{
    if (!businessFinishedToday) {
        qDebug() << "[endBusiness] business not started, return empty report";
        return todayReport;
    }

    if (reportGeneratedToday) {
        qDebug() << "[endBusiness] report already generated";
        return todayReport;
    }

    todayReport = settlementSystem.generateReport(store, customerSystem, todayPurchaseCost);
    reportGeneratedToday = true;
    qDebug() << "[endBusiness] report generated";
    return todayReport;
}

int GameManager::getTotalStock() const { int total=0; for(const Product& p:store.products){ total+=p.stock;} return total; }
const CustomerSystem& GameManager::getCustomerSystem() const { return customerSystem; }
void GameManager::performCleaning(){ store.cleanliness=qMin(100, store.cleanliness+15); }

bool GameManager::hireEmployee(int employeeIndex)
{
    if (employeeIndex < 0 || employeeIndex >= store.employees.size()) { qDebug() << "[hireEmployee] invalid index"; return false; }
    Employee& employee = store.employees[employeeIndex];
    if (employee.hired) { qDebug() << "[hireEmployee] already hired"; return false; }
    if (store.money < employee.hireCost) { qDebug() << "[hireEmployee] insufficient money"; return false; }
    store.money -= employee.hireCost;
    employee.hired = true;
    qDebug() << "[hireEmployee] success" << employee.name;
    return true;
}

bool GameManager::hasEmployeeType(const QString& type) const { for(const Employee& e: store.employees){ if(e.hired && e.type==type) return true; } return false; }
int GameManager::getEmployeeCount() const { int c=0; for(const Employee& e: store.employees){ if(e.hired) ++c; } return c; }

bool GameManager::upgradeStore(){ bool ok=upgradeSystem.upgradeStore(store); qDebug()<<"[upgradeStore]"<<ok; return ok; }
bool GameManager::upgradeStorage(){ bool ok=upgradeSystem.upgradeStorage(store); qDebug()<<"[upgradeStorage]"<<ok; return ok; }
bool GameManager::upgradeAnalysis(){ bool ok=upgradeSystem.upgradeAnalysis(store); qDebug()<<"[upgradeAnalysis]"<<ok; return ok; }
bool GameManager::unlockDailyGoods(){ bool ok=upgradeSystem.unlockDailyGoods(store); qDebug()<<"[unlockDailyGoods]"<<ok; return ok; }
int GameManager::getStoreUpgradeCost() const { return upgradeSystem.getStoreUpgradeCost(store);} 
int GameManager::getStorageUpgradeCost() const { return upgradeSystem.getStorageUpgradeCost(store);} 
int GameManager::getAnalysisUpgradeCost() const { return upgradeSystem.getAnalysisUpgradeCost(store);} 
int GameManager::getDailyGoodsUnlockCost() const { return upgradeSystem.getDailyGoodsUnlockCost(store);} 

bool GameManager::applyLoadedStore(const Store& loadedStore)
{
    store = loadedStore;
    todayPurchaseCost = 0;
    todayRevenue = 0;
    todayReport = DailyReport();
    businessFinishedToday = false;
    reportGeneratedToday = false;
    hasEnteredFirstDay = true;
    todayEvents = eventSystem.generateDailyEvents();
    marketSystem.updateDailyCosts(store, todayEvents);
    marketSystem.updateSuggestedPrices(store);
    return true;
}
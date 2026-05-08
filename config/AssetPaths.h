#ifndef ASSETPATHS_H
#define ASSETPATHS_H

#include <QString>

class AssetPaths
{
public:
    static QString assetRoot();
    static bool exists(const QString& path);
    static void debugPrintMissingAssets();

    static QString townStreetDay();
    static QString shopExteriorDay();
    static QString shopInteriorBasic();

    static QString employeeCashier();
    static QString employeePlanner();
    static QString employeeOperator();
    static QString employeeManagerGuide();
    static QString employeeStockClerk();

    static QString productWater();
    static QString productBread();
    static QString productChocolate();
    static QString productChips();
    static QString productCannedTea();
    static QString productNoodles();
    static QString productUmbrella();
    static QString productBatteryLight();

    static QString eventHeavyRain();
    static QString eventHotWeather();
    static QString eventStockpileFood();
    static QString eventStudentFlow();

    static QString logoTideshore();

    static QString iconPlay();
    static QString iconSettings();
    static QString iconSave();
    static QString iconClose();
    static QString iconConfirm();
    static QString iconArrowLeft();
    static QString iconArrowRight();
    static QString iconCoin();
    static QString iconStar();
    static QString iconCalendar();
    static QString iconAlert();
    static QString iconSound();
    static QString iconHelp();
    static QString iconShopBag();
    static QString iconCleanBroom();
    static QString iconDeliveryBoxes();
    static QString iconNoticeBoard();
    static QString iconHomeNeedsCleanup();
    static QString iconTeamCustomers();

    static QString panelWoodText();
    static QString panelSquareBlueGold();
    static QString panelSquareCard();
    static QString btnBlueStatesStrip();
    static QString btnWoodStatesStrip();
    static QString uiInventorySlot();
    static QString uiLabelTagBlank();
    static QString uiProgressVertical();
    static QString uiProgressVerticalSimple();

    static QString btnBlueNormal();
    static QString btnBlueHover();
    static QString btnBluePressed();
    static QString btnWoodNormal();
    static QString btnWoodHover();
    static QString btnWoodPressed();
    static QString zpixFont();

private:
    static QString filePath(const QString& relativePath);
};

#endif // ASSETPATHS_H
#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>

class Product
{
public:
    QString name;
    QString category;
    int baseCost;
    int todayCost;
    int suggestedPrice;
    int playerPrice;
    int stock;
    int maxStock;
    int demandLevel;
    int priceSensitivity;
    int eventSensitivity;
    int fluctuationSensitivity;
    bool unlocked;

    Product();
};

#endif // PRODUCT_H

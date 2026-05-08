#ifndef STORE_H
#define STORE_H

#include <QVector>
#include "Product.h"
#include "Employee.h"

class Store
{
public:
    int money;
    int level;
    int storageLevel;
    int storageCapacity;
    int shelfCount;
    int cleanliness;
    int analysisLevel;
    int currentDay;
    bool dailyGoodsUnlocked;
    QVector<Product> products;
    QVector<Employee> employees;

    Store();
};

#endif // STORE_H

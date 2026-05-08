#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QString>

class Employee
{
public:
    QString name;
    QString type;
    QString description;
    int hireCost;
    int dailyCost;
    bool hired;

    Employee();
};

#endif // EMPLOYEE_H

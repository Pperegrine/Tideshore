#ifndef DAILYREPORT_H
#define DAILYREPORT_H

#include <QString>
#include <QMap>

class DailyReport
{
public:
    int revenue;
    int cost;
    int profit;
    int customerCount;
    QString bestSeller;
    QString worstSeller;
    QString shortageProduct;
    QString summaryText;
    QMap<QString, int> soldCounts;
    QMap<QString, int> demandCounts;
    QMap<QString, int> shortageCounts;

    DailyReport();
};

#endif // DAILYREPORT_H

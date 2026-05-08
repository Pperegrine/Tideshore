#include "SettlementSystem.h"

SettlementSystem::SettlementSystem()
{
}

DailyReport SettlementSystem::generateReport(const Store& store, const CustomerSystem& customerSystem, int todayPurchaseCost)
{
    Q_UNUSED(store);

    DailyReport report;
    report.revenue = customerSystem.lastRevenue;
    report.cost = todayPurchaseCost;
    report.profit = report.revenue - report.cost;
    report.customerCount = customerSystem.lastCustomerFlow;

    report.soldCounts = customerSystem.lastSoldCounts;
    report.demandCounts = customerSystem.lastDemandCounts;
    report.shortageCounts = customerSystem.lastShortageCounts;

    report.bestSeller = findBestSeller(report.soldCounts);
    report.worstSeller = findWorstSeller(report.soldCounts);
    report.shortageProduct = findMainShortage(report.shortageCounts);

    if (report.profit > 500) {
        report.summaryText = "今日经营表现优秀，利润较高。";
    } else if (report.profit >= 100) {
        report.summaryText = "今日经营稳定，仍有优化空间。";
    } else if (report.profit >= 0) {
        report.summaryText = "今日略有盈利，但收益偏低。";
    } else {
        report.summaryText = "今日出现亏损，需要调整进货和定价策略。";
    }

    return report;
}

QString SettlementSystem::findBestSeller(const QMap<QString, int>& soldCounts)
{
    if (soldCounts.isEmpty()) {
        return "无";
    }

    QMap<QString, int>::const_iterator it = soldCounts.constBegin();
    QString bestName = it.key();
    int bestValue = it.value();
    ++it;

    while (it != soldCounts.constEnd()) {
        if (it.value() > bestValue) {
            bestValue = it.value();
            bestName = it.key();
        }
        ++it;
    }

    return bestName;
}

QString SettlementSystem::findWorstSeller(const QMap<QString, int>& soldCounts)
{
    if (soldCounts.isEmpty()) {
        return "无";
    }

    QMap<QString, int>::const_iterator it = soldCounts.constBegin();
    QString worstName = it.key();
    int worstValue = it.value();
    ++it;

    while (it != soldCounts.constEnd()) {
        if (it.value() < worstValue) {
            worstValue = it.value();
            worstName = it.key();
        }
        ++it;
    }

    return worstName;
}

QString SettlementSystem::findMainShortage(const QMap<QString, int>& shortageCounts)
{
    if (shortageCounts.isEmpty()) {
        return "无";
    }

    QMap<QString, int>::const_iterator it = shortageCounts.constBegin();
    QString shortageName = it.key();
    int shortageValue = it.value();
    ++it;

    while (it != shortageCounts.constEnd()) {
        if (it.value() > shortageValue) {
            shortageValue = it.value();
            shortageName = it.key();
        }
        ++it;
    }

    return shortageName;
}

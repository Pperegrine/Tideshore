#ifndef SETTLEMENTPAGE_H
#define SETTLEMENTPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>

#include "../logic/GameManager.h"

class QFrame;
class AudioManager;

class SettlementPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettlementPage(GameManager* manager, AudioManager* audio, QWidget* parent = nullptr);
    void refreshPage();

private:
    GameManager* gameManager = nullptr;
    AudioManager* audioManager = nullptr;
    QLabel* titleLabel = nullptr;
    QLabel* subtitleLabel = nullptr;

    QFrame* statsFrame = nullptr;
    QLabel* revenueCard = nullptr;
    QLabel* profitCard = nullptr;
    QLabel* customerCard = nullptr;
    QLabel* bestSellerCard = nullptr;

    QFrame* summaryFrame = nullptr;
    QTextEdit* summaryTextEdit = nullptr;

    QTableWidget* salesTable = nullptr;
    QPushButton* nextDayButton = nullptr;

    void setupUi();
    void refreshSummary();
    void refreshSalesTable();
    QString productIconPathByName(const QString& productName) const;
    QString performanceText(int demand, int sold, int shortage) const;
    void showProductAnalysisPopup(int row);

    signals:
        void upgradeClicked();

private slots:
    void onUpgradeClicked();
};

#endif
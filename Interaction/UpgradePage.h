#ifndef UPGRADEPAGE_H
#define UPGRADEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVector>

#include "../logic/GameManager.h"

class QFrame;
class AudioManager;

class UpgradePage : public QWidget
{
    Q_OBJECT

public:
    explicit UpgradePage(GameManager* manager, AudioManager* audio, QWidget* parent = nullptr);
    void refreshPage();

private:
    GameManager* gameManager = nullptr;
    AudioManager* audioManager = nullptr;
    QLabel* titleLabel = nullptr;
    QLabel* moneyLabel = nullptr;
    QLabel* storeLevelLabel = nullptr;
    QLabel* storageLabel = nullptr;
    QLabel* analysisLabel = nullptr;
    QLabel* dailyGoodsLabel = nullptr;
    QLabel* employeeTitleLabel = nullptr;
    QFrame* suggestionCard = nullptr;
    QLabel* suggestionTextLabel = nullptr;

    QFrame* storeCard = nullptr;
    QFrame* storageCard = nullptr;
    QFrame* analysisCard = nullptr;
    QFrame* dailyCard = nullptr;

    QLabel* storeStateLabel = nullptr;
    QLabel* storageStateLabel = nullptr;
    QLabel* analysisStateLabel = nullptr;
    QLabel* dailyStateLabel = nullptr;

    QLabel* storeEffectLabel = nullptr;
    QLabel* storageEffectLabel = nullptr;
    QLabel* analysisEffectLabel = nullptr;
    QLabel* dailyEffectLabel = nullptr;

    QLabel* storeCostLabel = nullptr;
    QLabel* storageCostLabel = nullptr;
    QLabel* analysisCostLabel = nullptr;
    QLabel* dailyCostLabel = nullptr;

    QFrame* employeeCardsFrame = nullptr;
    QVector<QFrame*> employeeCards;
    QVector<QLabel*> employeeStateLabels;
    QVector<QPushButton*> employeeHireButtons;
    QVector<QPushButton*> employeeDetailButtons;

    QPushButton* upgradeStoreButton = nullptr;
    QPushButton* upgradeStorageButton = nullptr;
    QPushButton* upgradeAnalysisButton = nullptr;
    QPushButton* unlockDailyGoodsButton = nullptr;
    QPushButton* nextDayButton = nullptr;

    void setupUi();
    void refreshInfo();
    void refreshEmployeeCards();
    QString employeeImagePathByIndex(int index) const;
    void showEmployeeDetailPopup(int index);
    QString generateSuggestionText() const;
    QString costText(int cost) const;

signals:
    void nextDayClicked();

private slots:
    void onUpgradeStoreClicked();
    void onUpgradeStorageClicked();
    void onUpgradeAnalysisClicked();
    void onUnlockDailyGoodsClicked();
    void onHireEmployeeClicked(int index);
    void onNextDayClicked();
};

#endif
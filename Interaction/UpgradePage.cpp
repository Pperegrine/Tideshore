#include "UpgradePage.h"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QPixmap>
#include <QDir>
#include <QDialog>
#include "../config/AssetPaths.h"
#include "AudioManager.h"
#include "FloatingText.h"

static void showNoIconMessage(QWidget* parent, const QString& title, const QString& text){
    QMessageBox box(parent);
    box.setIcon(QMessageBox::NoIcon);
    box.setWindowTitle(title);
    box.setText(text);
    box.setStandardButtons(QMessageBox::Ok);
    box.exec();
}

UpgradePage::UpgradePage(GameManager* manager, AudioManager* audio, QWidget* parent)
    : QWidget(parent), gameManager(manager), audioManager(audio)
{
    setupUi();
    refreshPage();
}

void UpgradePage::setupUi()
{
    setStyleSheet("QWidget{background-color:#dff1fb; color:#17364a;} QFrame.upgradeCard{background-color: rgba(230, 247, 255, 220); border:1px solid rgba(80,160,210,190); border-radius:12px;} QPushButton{background-color:rgba(40,95,130,210); color:white; border-radius:8px; padding:6px 12px; font-size:15px; min-height:34px;} QPushButton:disabled{background-color:rgba(120,145,160,160);}");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(18, 14, 18, 14);
    mainLayout->setSpacing(10);

    titleLabel = new QLabel("店铺升级中心", this);
    titleLabel->setStyleSheet("QLabel{font-size:24px; font-weight:700; color:#1b4b6a;}");
    mainLayout->addWidget(titleLabel);

    moneyLabel = new QLabel(this); storeLevelLabel = new QLabel(this); storageLabel = new QLabel(this); analysisLabel = new QLabel(this); dailyGoodsLabel = new QLabel(this);
    QHBoxLayout* topInfo = new QHBoxLayout();
    topInfo->addWidget(moneyLabel); topInfo->addWidget(storeLevelLabel); topInfo->addWidget(storageLabel); topInfo->addWidget(analysisLabel); topInfo->addWidget(dailyGoodsLabel); topInfo->addStretch();
    for (QLabel* l : {moneyLabel, storeLevelLabel, storageLabel, analysisLabel, dailyGoodsLabel}) l->setStyleSheet("font-size:15px;");
    mainLayout->addLayout(topInfo);

    auto buildCard=[&](QFrame*& card, QLabel*& state, QLabel*& effect, QLabel*& cost, QPushButton*& btn, const QString& name){
        card = new QFrame(this); card->setObjectName("card"); card->setProperty("class","upgradeCard");
        QVBoxLayout* l = new QVBoxLayout(card);
        l->setContentsMargins(14,14,14,14);
        l->setSpacing(6);
        QLabel* nameLabel = new QLabel(name, card); nameLabel->setStyleSheet("font-size:18px; font-weight:700;");
        state = new QLabel(card); effect = new QLabel(card); cost = new QLabel(card);
        effect->setWordWrap(true);
        state->setStyleSheet("font-size:14px;"); effect->setStyleSheet("font-size:14px;"); cost->setStyleSheet("font-size:14px;");
        btn = new QPushButton(card);
        l->addWidget(nameLabel); l->addWidget(state); l->addWidget(effect); l->addWidget(cost); l->addStretch(); l->addWidget(btn);
    };

    buildCard(storeCard, storeStateLabel, storeEffectLabel, storeCostLabel, upgradeStoreButton, "店铺扩建");
    buildCard(storageCard, storageStateLabel, storageEffectLabel, storageCostLabel, upgradeStorageButton, "仓储扩容");
    buildCard(analysisCard, analysisStateLabel, analysisEffectLabel, analysisCostLabel, upgradeAnalysisButton, "经营分析");
    buildCard(dailyCard, dailyStateLabel, dailyEffectLabel, dailyCostLabel, unlockDailyGoodsButton, "解锁日用品");

    QGridLayout* grid = new QGridLayout();
    grid->addWidget(storeCard,0,0); grid->addWidget(storageCard,0,1); grid->addWidget(analysisCard,1,0); grid->addWidget(dailyCard,1,1);
    mainLayout->addLayout(grid);

    suggestionCard = new QFrame(this);
    suggestionCard->setProperty("class", "upgradeCard");
    QVBoxLayout* suggestionLayout = new QVBoxLayout(suggestionCard);
    QLabel* suggestionTitle = new QLabel("今日经营建议", suggestionCard);
    suggestionTitle->setStyleSheet("font-size:18px; font-weight:700;");
    suggestionTextLabel = new QLabel(suggestionCard);
    suggestionTextLabel->setWordWrap(true);
    suggestionLayout->addWidget(suggestionTitle);
    suggestionLayout->addWidget(suggestionTextLabel);
    mainLayout->addWidget(suggestionCard);

    employeeTitleLabel = new QLabel("员工招募", this);
    mainLayout->addWidget(employeeTitleLabel);

    employeeCardsFrame = new QFrame(this);
    QGridLayout* empGrid = new QGridLayout(employeeCardsFrame);
    empGrid->setSpacing(10);
    for (int i = 0; i < 4; ++i) {
        QFrame* card = new QFrame(employeeCardsFrame);
        card->setProperty("class", "upgradeCard");
        card->setMinimumSize(285, 170);
        card->setMaximumHeight(190);
        QVBoxLayout* cl = new QVBoxLayout(card);
        cl->setContentsMargins(14,14,14,14);
        cl->setSpacing(6);
        QLabel* avatar = new QLabel(card);
        avatar->setFixedSize(96,96);
        avatar->setAlignment(Qt::AlignCenter);
        avatar->setScaledContents(false);
        const QString avatarPath = employeeImagePathByIndex(i);
        QPixmap px(avatarPath);
        if (px.isNull()) {
            qWarning() << "[UpgradePage] missing employee image:" << avatarPath;
            avatar->setText("头像");
        } else {
            avatar->setPixmap(px.scaled(avatar->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
        }
        QLabel* name = new QLabel(card); QLabel* role = new QLabel(card); QLabel* desc = new QLabel(card); QLabel* fee = new QLabel(card);
        desc->setWordWrap(true);
        QLabel* state = new QLabel(card);
        QPushButton* detailBtn = new QPushButton(card);
        detailBtn->setStyleSheet("QPushButton{background:transparent; border:none;}");
        detailBtn->setText("");
        QPushButton* hireBtn = new QPushButton("招募", card);
        employeeStateLabels.push_back(state); employeeHireButtons.push_back(hireBtn); employeeCards.push_back(card); employeeDetailButtons.push_back(detailBtn);
        QHBoxLayout* topRow = new QHBoxLayout();
        topRow->setSpacing(10);
        QVBoxLayout* infoCol = new QVBoxLayout();
        infoCol->setSpacing(4);
        infoCol->addWidget(name); infoCol->addWidget(role); infoCol->addWidget(desc); infoCol->addWidget(fee); infoCol->addWidget(state); infoCol->addStretch();
        topRow->addWidget(avatar,0,Qt::AlignTop);
        topRow->addLayout(infoCol,1);
        cl->addLayout(topRow);
        cl->addWidget(hireBtn);
        QString n,r,d;
        if(i==0){n="小林"; r="收银员"; d="熟练处理结账流程，使排队等待时间缩短。";}
        if(i==1){n="青叶"; r="宣传员"; d="擅长招揽顾客，使每日客流小幅增加。";}
        if(i==2){n="逢"; r="分析员"; d="擅长市场分析，使建议售价更加可靠。";}
        if(i==3){n="露田"; r="理货员"; d="负责整理货架与店面，使清洁度下降速度减缓。";}
        name->setText(n); role->setText(r); desc->setText(d); fee->setObjectName("fee");
        name->setStyleSheet("font-size:17px; font-weight:700;");
        role->setStyleSheet("font-size:14px;"); desc->setStyleSheet("font-size:14px;"); fee->setStyleSheet("font-size:14px;"); state->setStyleSheet("font-size:14px;");
        connect(hireBtn, &QPushButton::clicked, this, [this,i](){ onHireEmployeeClicked(i); });
        connect(detailBtn, &QPushButton::clicked, this, [this,i](){ showEmployeeDetailPopup(i); });
        empGrid->addWidget(card, i/2, i%2);
    }
    mainLayout->addWidget(employeeCardsFrame);

    QHBoxLayout* footer = new QHBoxLayout(); footer->addStretch();
    nextDayButton = new QPushButton("进入下一天", this); footer->addWidget(nextDayButton);
    mainLayout->addLayout(footer);

    connect(upgradeStoreButton, &QPushButton::clicked, this, &UpgradePage::onUpgradeStoreClicked);
    connect(upgradeStorageButton, &QPushButton::clicked, this, &UpgradePage::onUpgradeStorageClicked);
    connect(upgradeAnalysisButton, &QPushButton::clicked, this, &UpgradePage::onUpgradeAnalysisClicked);
    connect(unlockDailyGoodsButton, &QPushButton::clicked, this, &UpgradePage::onUnlockDailyGoodsClicked);
    connect(nextDayButton, &QPushButton::clicked, this, &UpgradePage::onNextDayClicked);
}

void UpgradePage::refreshPage(){ refreshInfo(); refreshEmployeeCards(); if(suggestionTextLabel) suggestionTextLabel->setText(generateSuggestionText()); }

void UpgradePage::refreshInfo()
{
    moneyLabel->setText(QString("当前资金: %1").arg(gameManager->store.money));
    storeLevelLabel->setText(QString("店铺等级: %1").arg(gameManager->store.level));
    storageLabel->setText(QString("仓储等级: %1").arg(gameManager->store.storageLevel));
    analysisLabel->setText(QString("分析等级: %1").arg(gameManager->store.analysisLevel));
    dailyGoodsLabel->setText(QString("日用品:%1 | 已雇佣:%2").arg(gameManager->store.dailyGoodsUnlocked?"已解锁":"未解锁").arg(gameManager->getEmployeeCount()));

    int storeCost = gameManager->getStoreUpgradeCost();
    storeStateLabel->setText(QString("当前等级：%1（货架:%2）").arg(gameManager->store.level).arg(gameManager->store.shelfCount));
    storeEffectLabel->setText("效果：提升店铺规模，增加货架数量。");
    storeCostLabel->setText(QString("费用：%1").arg(costText(storeCost)));
    upgradeStoreButton->setText(storeCost < 0 ? "已满级" : "执行升级");
    upgradeStoreButton->setEnabled(storeCost >= 0 && gameManager->store.money >= storeCost);

    int storageCost = gameManager->getStorageUpgradeCost();
    storageStateLabel->setText(QString("当前等级：%1（容量:%2）").arg(gameManager->store.storageLevel).arg(gameManager->store.storageCapacity));
    storageEffectLabel->setText("效果：提升库存上限，降低缺货概率。");
    storageCostLabel->setText(QString("费用：%1").arg(costText(storageCost)));
    upgradeStorageButton->setText(storageCost < 0 ? "已满级" : "执行升级");
    upgradeStorageButton->setEnabled(storageCost >= 0 && gameManager->store.money >= storageCost);

    int analysisCost = gameManager->getAnalysisUpgradeCost();
    analysisStateLabel->setText(QString("当前等级：%1").arg(gameManager->store.analysisLevel));
    analysisEffectLabel->setText("效果：提高市场分析可靠性与建议质量。");
    analysisCostLabel->setText(QString("费用：%1").arg(costText(analysisCost)));
    upgradeAnalysisButton->setText(analysisCost < 0 ? "已满级" : "执行升级");
    upgradeAnalysisButton->setEnabled(analysisCost >= 0 && gameManager->store.money >= analysisCost);

    int dailyGoodsCost = gameManager->getDailyGoodsUnlockCost();
    dailyStateLabel->setText(QString("当前状态：%1").arg(gameManager->store.dailyGoodsUnlocked?"已解锁":"未解锁"));
    dailyEffectLabel->setText("效果：解锁日用品类商品，扩展经营范围。");
    dailyCostLabel->setText(QString("费用：%1").arg(costText(dailyGoodsCost)));
    unlockDailyGoodsButton->setText(dailyGoodsCost < 0 ? "已解锁" : "执行解锁");
    unlockDailyGoodsButton->setEnabled(dailyGoodsCost >= 0 && gameManager->store.money >= dailyGoodsCost);
}

void UpgradePage::refreshEmployeeCards()
{
    const QVector<Employee>& employees = gameManager->store.employees;
    for (int i=0; i<employeeCards.size(); ++i) {
        if (i >= employees.size()) { employeeCards[i]->hide(); continue; }
        employeeCards[i]->show();
        const Employee& e = employees[i];
        auto labels = employeeCards[i]->findChildren<QLabel*>();
        for (QLabel* l : labels) {
            if (l->objectName()=="fee") l->setText(QString("费用：%1").arg(e.hireCost));
        }
        employeeStateLabels[i]->setText(e.hired ? "状态：已雇佣" : "状态：未雇佣");
        employeeHireButtons[i]->setText(e.hired ? "已雇佣" : "招募");
        employeeHireButtons[i]->setEnabled(!e.hired && gameManager->store.money >= e.hireCost);
        if(i < employeeDetailButtons.size() && employeeDetailButtons[i]) employeeDetailButtons[i]->setGeometry(8,8, employeeCards[i]->width()-16, employeeCards[i]->height()-48);
    }
}

QString UpgradePage::employeeImagePathByIndex(int index) const
{
    if (index==0) return QDir(AssetPaths::assetRoot()).filePath("characters/employee_cashier.png");
    if (index==1) return QDir(AssetPaths::assetRoot()).filePath("characters/employee_operator.png");
    if (index==2) return QDir(AssetPaths::assetRoot()).filePath("characters/employee_planner.png");
    if (index==3) return QDir(AssetPaths::assetRoot()).filePath("characters/employee_stock_clerk.png");
    if (index==4) return QDir(AssetPaths::assetRoot()).filePath("characters/employee_manager_guide.png");
    return AssetPaths::iconHelp();
}

QString UpgradePage::costText(int cost) const{ return cost < 0 ? "已满级/已解锁" : QString::number(cost); }

void UpgradePage::onUpgradeStoreClicked(){ bool ok=gameManager->upgradeStore(); QPoint p(width()*0.5, height()*0.72); if(!ok){ if(audioManager) audioManager->playErrorSfx(); FloatingText::showText(this,"资金不足",p,"error"); showNoIconMessage(this,"提示","升级失败，可能资金不足或已经达到上限"); } else { if(audioManager) audioManager->playCoinSfx(); FloatingText::showText(this,"升级成功",p,"coin"); } refreshPage(); }
void UpgradePage::onUpgradeStorageClicked(){ bool ok=gameManager->upgradeStorage(); QPoint p(width()*0.5, height()*0.72); if(!ok){ if(audioManager) audioManager->playErrorSfx(); FloatingText::showText(this,"资金不足",p,"error"); showNoIconMessage(this,"提示","升级失败，可能资金不足或已经达到上限"); } else { if(audioManager) audioManager->playCoinSfx(); FloatingText::showText(this,"升级成功",p,"coin"); } refreshPage(); }
void UpgradePage::onUpgradeAnalysisClicked(){ bool ok=gameManager->upgradeAnalysis(); QPoint p(width()*0.5, height()*0.72); if(!ok){ if(audioManager) audioManager->playErrorSfx(); FloatingText::showText(this,"资金不足",p,"error"); showNoIconMessage(this,"提示","升级失败，可能资金不足或已经达到上限"); } else { if(audioManager) audioManager->playCoinSfx(); FloatingText::showText(this,"升级成功",p,"coin"); } refreshPage(); }
void UpgradePage::onUnlockDailyGoodsClicked(){ bool ok=gameManager->unlockDailyGoods(); if(!ok){ if(audioManager) audioManager->playErrorSfx(); showNoIconMessage(this,"提示","解锁失败，可能资金不足或已经解锁"); } else { if(audioManager) audioManager->playCoinSfx(); } refreshPage(); }

void UpgradePage::onHireEmployeeClicked(int index){ if(index<0 || index>=gameManager->store.employees.size()) return; const Employee& e=gameManager->store.employees[index]; QPoint p(width()*0.5, height()*0.72); if(e.hired){ if(audioManager) audioManager->playErrorSfx(); FloatingText::showText(this,"员工已雇佣",p,"error"); refreshPage(); return; } if(gameManager->store.money < e.hireCost){ if(audioManager) audioManager->playErrorSfx(); FloatingText::showText(this,"资金不足",p,"error"); showNoIconMessage(this,"提示","资金不足，无法招募该员工。"); refreshPage(); return; } bool ok=gameManager->hireEmployee(index); if(!ok){ if(audioManager) audioManager->playErrorSfx(); FloatingText::showText(this,"招募失败",p,"error"); showNoIconMessage(this,"提示","招募失败，可能资金不足或员工已雇佣"); } else { if(audioManager) audioManager->playCoinSfx(); FloatingText::showText(this,"员工已雇佣",p,"success"); } refreshPage(); }
void UpgradePage::onNextDayClicked(){ gameManager->startNewDay(); if(audioManager) audioManager->playConfirmSfx(); emit nextDayClicked(); }


QString UpgradePage::generateSuggestionText() const
{
    if (gameManager->store.money < 1000) return "资金偏紧，建议谨慎升级，优先进入下一天积累资金。";
    if (gameManager->store.storageLevel < 2) return "建议优先升级仓储，减少缺货风险。";
    bool hiredPromoter = false;
    for (const Employee& e : gameManager->store.employees) if (e.type.contains("宣传") && e.hired) hiredPromoter = true;
    if (gameManager->store.money >= 1200 && !hiredPromoter) return "资金充足且宣传员未雇佣，建议优先招募宣传员提升明日客流。";
    if (gameManager->store.analysisLevel < 2) return "建议升级分析能力，提高定价判断与市场响应。";
    return "当前经营状态稳定，可在升级与招募间平衡投入。";
}

void UpgradePage::showEmployeeDetailPopup(int index)
{
    if(index<0 || index>=gameManager->store.employees.size()) return;
    const Employee& e = gameManager->store.employees[index];
    QDialog dlg(this); dlg.setWindowTitle("员工详情"); dlg.resize(460, 360);
    QVBoxLayout* l = new QVBoxLayout(&dlg);
    QLabel* avatar = new QLabel(&dlg); avatar->setFixedSize(110,110); avatar->setAlignment(Qt::AlignCenter);
    QPixmap px(employeeImagePathByIndex(index));
    if (px.isNull()) avatar->setText("头像"); else avatar->setPixmap(px.scaled(110,110,Qt::KeepAspectRatio,Qt::SmoothTransformation));
QLabel* info = new QLabel(QString("姓名：%1\n职位：%2\n能力说明：%3\n招募费用：%4\n当前状态：%5").arg(e.name,e.type,e.description,QString::number(e.hireCost),e.hired?"已雇佣":"未雇佣"), &dlg);
    info->setWordWrap(true);
    l->addWidget(avatar,0,Qt::AlignHCenter); l->addWidget(info);
    QHBoxLayout* btns = new QHBoxLayout(); btns->addStretch();
    if(!e.hired){ QPushButton* hire = new QPushButton("招募", &dlg); connect(hire,&QPushButton::clicked,&dlg,[this,index,&dlg](){ onHireEmployeeClicked(index); dlg.accept(); }); btns->addWidget(hire);}
    QPushButton* close = new QPushButton("关闭", &dlg); connect(close,&QPushButton::clicked,&dlg,&QDialog::accept); btns->addWidget(close); l->addLayout(btns);
    dlg.exec();
}
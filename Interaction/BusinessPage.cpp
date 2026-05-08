#include "BusinessPage.h"

#include <QPainter>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QTextEdit>
#include <QMessageBox>
#include <QTimer>
#include <QDir>
#include <cmath>
#include <QFileInfo>

static void showNoIconMessage(QWidget* parent, const QString& title, const QString& text){
    QMessageBox box(parent);
    box.setIcon(QMessageBox::NoIcon);
    box.setWindowTitle(title);
    box.setText(text);
    box.setStandardButtons(QMessageBox::Ok);
    box.exec();
}

static QMessageBox::StandardButton askNoIconQuestion(QWidget* parent, const QString& title, const QString& text){
    QMessageBox box(parent);
    box.setIcon(QMessageBox::NoIcon);
    box.setWindowTitle(title);
    box.setText(text);
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box.setDefaultButton(QMessageBox::No);
    return static_cast<QMessageBox::StandardButton>(box.exec());
}

#include "HintBadge.h"
#include "../config/AssetPaths.h"
#include "SpriteAnimator.h"
#include "AudioManager.h"
#include "FloatingText.h"

BusinessPage::BusinessPage(GameManager* manager, AudioManager* audio, QWidget* parent)
    : QWidget(parent), gameManager(manager), audioManager(audio)
{
    backgroundPixmap = QPixmap(AssetPaths::shopInteriorBasic());
    setupUi();
    refreshPage();

    hintAnimTimer = new QTimer(this);
    connect(hintAnimTimer, &QTimer::timeout, this, [this]() {
        if (!checkoutHintBadge || !shelfHintBadge || !doorHintBadge || !messageHintBadge || !checkoutHotspot || !shelfHotspot || !doorHotspot || !messageButton) return;
        animTick++;
        qreal phase = animTick * 0.2;
        int dy = static_cast<int>(-8.0 * (0.5 + 0.5 * std::sin(phase)));
        checkoutHintBadge->setGeometry(checkoutHotspot->x()+checkoutHotspot->width()/2-24, checkoutHotspot->y()-56+dy, 48, 48);
        shelfHintBadge->setGeometry(shelfHotspot->x()+shelfHotspot->width()/2-24, shelfHotspot->y()-56+dy, 48, 48);
        doorHintBadge->setGeometry(doorHotspot->x()+doorHotspot->width()/2-24, doorHotspot->y()-56+dy, 48, 48);
        messageHintBadge->setGeometry(messageButton->x()-54, messageButton->y()+2+dy/2, 42, 42);
        checkoutHintBadge->setPhase(phase);
        shelfHintBadge->setPhase(phase);
        doorHintBadge->setPhase(phase);
        messageHintBadge->setPhase(phase);
    });
    hintAnimTimer->start(33);
}

void BusinessPage::setupUi()
{
    hudFrame = new QFrame(this);
    hudFrame->setStyleSheet("QFrame{background-color: rgba(10, 40, 70, 170); border:1px solid rgba(170,220,255,160); border-radius:10px;}");
    titleLabel = new QLabel("营业中", hudFrame);
    dayLabel = new QLabel(hudFrame); moneyLabel = new QLabel(hudFrame); cleanlinessLabel = new QLabel(hudFrame); employeeLabel = new QLabel(hudFrame);
    for (QLabel* l : {titleLabel, dayLabel, moneyLabel, cleanlinessLabel, employeeLabel}) {
        l->setStyleSheet("QLabel{color:white; background-color: rgba(10,40,70,170); border:1px solid rgba(170,220,255,150); border-radius:8px; padding:6px 10px;}");
    }

    statusLabel = new QLabel(this);
    menuButton = new QPushButton("☰", hudFrame);
    statusLabel->setStyleSheet("QLabel{background-color: rgba(10, 40, 70, 170); color:white; border:1px solid rgba(170,220,255,160); border-radius:8px; padding:6px 12px;}");

    menuButton->setStyleSheet("QPushButton{background-color: rgba(10,40,70,170); color:white; border:1px solid rgba(170,220,255,160); border-radius:8px; font-size:18px; font-weight:700;}");
    connect(menuButton, &QPushButton::clicked, this, [this](){ if(audioManager) audioManager->playClickSfx(); emit menuRequested(); });

    messageButton = new QPushButton("今日消息", this);
    messageButton->setStyleSheet("QPushButton{background-color: rgba(10,40,70,180); color:white; border:1px solid rgba(170,220,255,160); border-radius:8px; padding:6px 12px;}");
    connect(messageButton, &QPushButton::clicked, this, [this](){ if(audioManager) audioManager->playClickSfx(); showTodayEventPopup(); });

    checkoutHotspot = new QPushButton(this);
    shelfHotspot = new QPushButton(this);
    doorHotspot = new QPushButton(this);
    for (QPushButton* b : {checkoutHotspot,shelfHotspot,doorHotspot}) {
        b->setStyleSheet("QPushButton{background: transparent; border: none;}");
    }
    connect(checkoutHotspot, &QPushButton::clicked, this, [this](){ showCheckoutPopup(); });
    connect(shelfHotspot, &QPushButton::clicked, this, [this](){ showShelfPopup(); });
    connect(doorHotspot, &QPushButton::clicked, this, [this](){ showEndBusinessPopup(); });

    QPixmap panel(AssetPaths::panelSquareCard());
    QPixmap fallback(AssetPaths::iconHelp());
    checkoutHintBadge = new HintBadge(this); checkoutHintBadge->setBadgePixmaps(panel, QPixmap(AssetPaths::iconCoin()).isNull()?fallback:QPixmap(AssetPaths::iconCoin())); checkoutHintBadge->setPhaseOffset(0.0);
    shelfHintBadge = new HintBadge(this); shelfHintBadge->setBadgePixmaps(panel, QPixmap(AssetPaths::iconShopBag()).isNull()?fallback:QPixmap(AssetPaths::iconShopBag())); shelfHintBadge->setPhaseOffset(0.7);
    doorHintBadge = new HintBadge(this); doorHintBadge->setBadgePixmaps(panel, QPixmap(AssetPaths::iconConfirm()).isNull()?QPixmap(AssetPaths::iconArrowRight()):QPixmap(AssetPaths::iconConfirm())); doorHintBadge->setPhaseOffset(1.4);
    messageHintBadge = new HintBadge(this); messageHintBadge->setBadgePixmaps(panel, QPixmap(AssetPaths::iconAlert()).isNull()?fallback:QPixmap(AssetPaths::iconAlert())); messageHintBadge->setPhaseOffset(2.1);

    bottomBar = new QFrame(this);
    bottomBar->setStyleSheet("QFrame{background-color: rgba(10,35,55,155); border:1px solid rgba(170,220,255,160); border-radius:10px;}");
    bottomTipLabel = new QLabel("营业中：点击场景区域查看店铺状态。", bottomBar);
    bottomTipLabel->setStyleSheet("QLabel{color:white; background:transparent;}");

    cleanButton = new QPushButton("清洁店铺", bottomBar);
    finishBusinessButton = new QPushButton("结束营业并结算", bottomBar);
    cleanButton->setStyleSheet("QPushButton{background-color: rgba(44,90,130,200); color:white; border-radius:8px; padding:8px 16px;}");
    finishBusinessButton->setStyleSheet("QPushButton{background-color: rgba(44,90,130,220); color:white; border-radius:8px; padding:8px 16px;}");
    setupEmployeeSceneAnimators();
    connect(cleanButton, &QPushButton::clicked, this, &BusinessPage::onCleanClicked);
    connect(finishBusinessButton, &QPushButton::clicked, this, &BusinessPage::onFinishBusinessClicked);
}

void BusinessPage::resizeEvent(QResizeEvent* event){
    QWidget::resizeEvent(event);
    updateLayoutBySize();
    updateEmployeeSceneAnimatorLayout();
}

void BusinessPage::paintEvent(QPaintEvent* event){ QPainter p(this); if(!backgroundPixmap.isNull()){QPixmap s=backgroundPixmap.scaled(size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation); int x=(s.width()-width())/2,y=(s.height()-height())/2; p.drawPixmap(0,0,s,x,y,width(),height());} QWidget::paintEvent(event); }

void BusinessPage::updateLayoutBySize()
{
    hudFrame->setGeometry(16, 14, width()-32, 48);
    titleLabel->setGeometry(10,6,90,34); dayLabel->setGeometry(110,6,140,34); moneyLabel->setGeometry(260,6,180,34); cleanlinessLabel->setGeometry(450,6,140,34); employeeLabel->setGeometry(600,6,qMax(80, width()-860),34);
    menuButton->setGeometry(hudFrame->width()-52, 6, 40, 34);
    messageButton->setGeometry(width()-150, 72, 120, 36);
    statusLabel->setGeometry(width()-420, 116, 390, 36);

    const qreal sx=width()/1280.0, sy=height()/720.0;
    checkoutHotspot->setGeometry(int(170*sx), int(520*sy), int(280*sx), int(160*sy));
    shelfHotspot->setGeometry(int(760*sx), int(350*sy), int(420*sx), int(180*sy));
    doorHotspot->setGeometry(int(130*sx), int(190*sy), int(300*sx), int(260*sy));

    bottomBar->setGeometry(18, height()-96, width()-36, 78);
    cleanButton->setGeometry(30, 18, 220, 42);
    finishBusinessButton->setGeometry(bottomBar->width()-250, 18, 220, 42);
    bottomTipLabel->setGeometry(280, 22, bottomBar->width()-560, 34);
}

void BusinessPage::refreshInfo()
{
    dayLabel->setText(QString("当前天数:%1").arg(gameManager->store.currentDay));
    moneyLabel->setText(QString("当前资金:%1").arg(gameManager->store.money));
    cleanlinessLabel->setText(QString("清洁度:%1").arg(gameManager->store.cleanliness));
    QStringList hired; for (const Employee& e: gameManager->store.employees) if(e.hired) hired << e.name;
    employeeLabel->setText(QString("当前员工:%1").arg(hired.isEmpty()?"暂无":hired.join("/")));
    statusLabel->setText((!gameManager->todayEvents.isEmpty())?"今日状态：有事件发生，点击右上角查看":"今日状态：营业平稳，点击店门可结算");
}

void BusinessPage::refreshPage(){ refreshInfo(); updateLayoutBySize(); refreshEmployeeSceneVisibility(); updateEmployeeSceneAnimatorLayout(); }

void BusinessPage::showTodayEventPopup(){
    QDialog dlg(this); dlg.setWindowTitle("今日营业消息"); dlg.resize(width()*0.6, height()*0.6);
    QVBoxLayout* l=new QVBoxLayout(&dlg); QTextEdit* te=new QTextEdit(&dlg); te->setReadOnly(true); te->setStyleSheet("QTextEdit{background-color: rgba(12,30,48,220); color:white; border:1px solid rgba(170,220,255,150);}");
    if(gameManager->todayEvents.isEmpty()) te->setPlainText("今日营业环境：平稳\n暂无明显事件。");
    else for(const Event& e:gameManager->todayEvents) te->append(QString("事件名称:%1\n来源:%2\n描述:%3\n可信度:%4\n---").arg(e.name,e.source,e.description).arg(e.credibility));
    l->addWidget(te); dlg.exec();
}
void BusinessPage::showCheckoutPopup(){ showNoIconMessage(this,"收银台简报",QString("当前天数:%1\n当前资金:%2\n今日收入：统计中").arg(gameManager->store.currentDay).arg(gameManager->store.money)); }
void BusinessPage::showShelfPopup(){ showNoIconMessage(this,"货架库存简报",QString("商品数量:%1\n低库存商品：请在营业前准备页检查").arg(gameManager->store.products.size())); }
void BusinessPage::showEndBusinessPopup(){ if(audioManager) audioManager->playClickSfx(); if(askNoIconQuestion(this,"结束营业","确定结束今天营业并进入结算吗？")==QMessageBox::Yes){ if(audioManager) audioManager->playConfirmSfx(); onFinishBusinessClicked(); }}

void BusinessPage::onCleanClicked(){ gameManager->performCleaning(); if(audioManager) audioManager->playConfirmSfx(); QPoint p = mapFromGlobal(cleanButton->mapToGlobal(cleanButton->rect().center())); FloatingText::showText(this, "清洁度提升", p, "success"); refreshPage(); }
void BusinessPage::onFinishBusinessClicked(){ gameManager->startBusiness(); gameManager->endBusiness(); emit businessFinished(); }



void BusinessPage::setupEmployeeSceneAnimators()
{
    if (mopAnimator) {
        qDebug() << "[BusinessPage] mop animator already exists, skip setup";
        return;
    }
    qDebug() << "[BusinessPage] create single mop animator";

    mopAnimLabel = new QLabel(this);
    mopAnimLabel->setObjectName("employee_anim_mopping_single");
    mopAnimLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    mopAnimLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    mopAnimLabel->setAutoFillBackground(false);
    mopAnimLabel->setStyleSheet("QLabel{background: transparent; border: none;}");

    mopAnimator = new SpriteAnimator(mopAnimLabel, this);
    mopAnimator->setInterval(220);
    mopAnimator->setScaledSize(QSize(224, 308));

    const QString mopPath = QDir(AssetPaths::assetRoot()).filePath("characters/animations/employee_mop_loop_strip.png");
    if (QFileInfo::exists(mopPath) && mopAnimator->loadStrip(mopPath, 6)) {
        mopAnimator->start();
    } else {
        qWarning() << "[BusinessPage] missing mop strip:" << mopPath;
        mopAnimLabel->hide();
    }

    if (cashierAnimator) {
        qDebug() << "[BusinessPage] cashier animator already exists, skip setup";
        refreshEmployeeSceneVisibility();
        return;
    }
    qDebug() << "[BusinessPage] create single cashier animator";

    cashierAnimLabel = new QLabel(this);
    cashierAnimLabel->setObjectName("employee_anim_cashier_single");
    cashierAnimLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    cashierAnimLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    cashierAnimLabel->setAutoFillBackground(false);
    cashierAnimLabel->setStyleSheet("QLabel{background: transparent; border: none;}");

    cashierAnimator = new SpriteAnimator(cashierAnimLabel, this);
    cashierAnimator->setInterval(220);
    cashierAnimator->setScaledSize(QSize(210, 290));

    const QString cashierPath = QDir(AssetPaths::assetRoot()).filePath("characters/animations/employee_cashier_idle_strip.png");
    if (QFileInfo::exists(cashierPath) && cashierAnimator->loadStrip(cashierPath, 6)) {
        cashierAnimator->start();
    } else {
        qWarning() << "[BusinessPage] missing cashier strip:" << cashierPath;
        cashierAnimLabel->hide();
    }

    if (stockClerkAnimator) {
        qDebug() << "[BusinessPage] stock clerk animator already exists, skip setup";
        refreshEmployeeSceneVisibility();
        return;
    }
    qDebug() << "[BusinessPage] create single stock clerk animator";

    stockClerkAnimLabel = new QLabel(this);
    stockClerkAnimLabel->setObjectName("employee_anim_stock_clerk_single");
    stockClerkAnimLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    stockClerkAnimLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    stockClerkAnimLabel->setAutoFillBackground(false);
    stockClerkAnimLabel->setStyleSheet("QLabel{background: transparent; border: none;}");

    stockClerkAnimator = new SpriteAnimator(stockClerkAnimLabel, this);
    stockClerkAnimator->setInterval(220);
    stockClerkAnimator->setScaledSize(QSize(210, 290));

    const QString stockClerkPath = QDir(AssetPaths::assetRoot()).filePath("characters/animations/employee_stock_clerk_idle_strip.png");
    if (QFileInfo::exists(stockClerkPath) && stockClerkAnimator->loadStrip(stockClerkPath, 6)) {
        stockClerkAnimator->start();
    } else {
        qWarning() << "[BusinessPage] missing stock clerk strip:" << stockClerkPath;
        stockClerkAnimLabel->hide();
    }

    if (analystAnimator) {
        qDebug() << "[BusinessPage] analyst animator already exists, skip setup";
        refreshEmployeeSceneVisibility();
        return;
    }
    qDebug() << "[BusinessPage] create single analyst animator";

    analystAnimLabel = new QLabel(this);
    analystAnimLabel->setObjectName("employee_anim_analyst_single");
    analystAnimLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    analystAnimLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    analystAnimLabel->setAutoFillBackground(false);
    analystAnimLabel->setStyleSheet("QLabel{background: transparent; border: none;}");

    analystAnimator = new SpriteAnimator(analystAnimLabel, this);
    analystAnimator->setInterval(220);
    analystAnimator->setScaledSize(QSize(210, 290));

    const QString analystPath = QDir(AssetPaths::assetRoot()).filePath("characters/animations/employee_analyst_idle_strip.png");
    if (QFileInfo::exists(analystPath) && analystAnimator->loadStrip(analystPath, 6)) {
        analystAnimator->start();
    } else {
        qWarning() << "[BusinessPage] missing analyst strip:" << analystPath;
        analystAnimLabel->hide();
    }

    if (promoterAnimator) {
        qDebug() << "[BusinessPage] promoter animator already exists, skip setup";
        refreshEmployeeSceneVisibility();
        return;
    }
    qDebug() << "[BusinessPage] create single promoter animator";

    promoterAnimLabel = new QLabel(this);
    promoterAnimLabel->setObjectName("employee_anim_promoter_single");
    promoterAnimLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    promoterAnimLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    promoterAnimLabel->setAutoFillBackground(false);
    promoterAnimLabel->setStyleSheet("QLabel{background: transparent; border: none;}");

    promoterAnimator = new SpriteAnimator(promoterAnimLabel, this);
    promoterAnimator->setInterval(220);
    promoterAnimator->setScaledSize(QSize(210, 290));

    const QString promoterPath = QDir(AssetPaths::assetRoot()).filePath("characters/animations/employee_promoter_idle_strip.png");
    if (QFileInfo::exists(promoterPath) && promoterAnimator->loadStrip(promoterPath, 6)) {
        promoterAnimator->start();
    } else {
        qWarning() << "[BusinessPage] missing promoter strip:" << promoterPath;
        promoterAnimLabel->hide();
    }
    refreshEmployeeSceneVisibility();
}

void BusinessPage::refreshEmployeeSceneVisibility()
{
    if (mopAnimLabel) {
        const bool visible = mopAnimator && !mopAnimLabel->pixmap(Qt::ReturnByValue).isNull();
        mopAnimLabel->setVisible(visible);
    }
    const bool cashierHired = gameManager && gameManager->hasEmployeeType("收银员");
    const bool stockClerkHired = gameManager && gameManager->hasEmployeeType("理货员");
    const bool analystHired = gameManager && gameManager->hasEmployeeType("分析员");
    const bool promoterHired = gameManager && gameManager->hasEmployeeType("宣传员");
    qDebug() << "[BusinessPage] employee visibility:"
             << "cashier" << cashierHired
             << "stock" << stockClerkHired
             << "analyst" << analystHired
             << "promoter" << promoterHired;
    if (cashierAnimLabel) {
        const bool cashierVisible = cashierHired && cashierAnimator && !cashierAnimLabel->pixmap(Qt::ReturnByValue).isNull();
        cashierAnimLabel->setVisible(cashierVisible);
    }
    if (stockClerkAnimLabel) {
        const bool stockClerkVisible = stockClerkHired && stockClerkAnimator && !stockClerkAnimLabel->pixmap(Qt::ReturnByValue).isNull();
        stockClerkAnimLabel->setVisible(stockClerkVisible);
    }
    if (analystAnimLabel) {
        const bool analystVisible = analystHired && analystAnimator && !analystAnimLabel->pixmap(Qt::ReturnByValue).isNull();
        analystAnimLabel->setVisible(analystVisible);
    }
    if (promoterAnimLabel) {
        const bool promoterVisible = promoterHired && promoterAnimator && !promoterAnimLabel->pixmap(Qt::ReturnByValue).isNull();
        promoterAnimLabel->setVisible(promoterVisible);
    }
}

void BusinessPage::updateEmployeeSceneAnimatorLayout()
{
    if (mopAnimLabel) {
        constexpr int kMopWidth = 224;
        constexpr int kMopHeight = 308;
        constexpr qreal kMopXRatio = 0.46;
        constexpr qreal kMopYRatio = 0.60;
        mopAnimLabel->setGeometry(int(width() * kMopXRatio), int(height() * kMopYRatio), kMopWidth, kMopHeight);
        mopAnimLabel->lower();
    }
    if (cashierAnimLabel) {
        constexpr int kCashierWidth = 210;
        constexpr int kCashierHeight = 290;
        constexpr qreal kCashierXRatio = 0.255;
        constexpr qreal kCashierYRatio = 0.475;
        cashierAnimLabel->setGeometry(int(width() * kCashierXRatio), int(height() * kCashierYRatio), kCashierWidth, kCashierHeight);
        cashierAnimLabel->lower();
    }
    if (stockClerkAnimLabel) {
        constexpr int kStockClerkWidth = 210;
        constexpr int kStockClerkHeight = 290;
        constexpr qreal kStockClerkXRatio = 0.575;
        constexpr qreal kStockClerkYRatio = 0.535;
        stockClerkAnimLabel->setGeometry(int(width() * kStockClerkXRatio), int(height() * kStockClerkYRatio), kStockClerkWidth, kStockClerkHeight);
        stockClerkAnimLabel->lower();
    }
    if (analystAnimLabel) {
        constexpr int kAnalystWidth = 210;
        constexpr int kAnalystHeight = 290;
        constexpr qreal kAnalystXRatio = 0.355;
        constexpr qreal kAnalystYRatio = 0.385;
        analystAnimLabel->setGeometry(int(width() * kAnalystXRatio), int(height() * kAnalystYRatio), kAnalystWidth, kAnalystHeight);
        analystAnimLabel->lower();
    }
    if (promoterAnimLabel) {
        constexpr int kPromoterWidth = 210;
        constexpr int kPromoterHeight = 290;
        constexpr qreal kPromoterXRatio = 0.27;
        constexpr qreal kPromoterYRatio = 0.23;
        promoterAnimLabel->setGeometry(int(width() * kPromoterXRatio), int(height() * kPromoterYRatio), kPromoterWidth, kPromoterHeight);
        promoterAnimLabel->lower();
    }
    if (hudFrame) hudFrame->raise();
    if (bottomBar) bottomBar->raise();
    if (checkoutHintBadge) checkoutHintBadge->raise();
    if (shelfHintBadge) shelfHintBadge->raise();
    if (doorHintBadge) doorHintBadge->raise();
    if (messageHintBadge) messageHintBadge->raise();
    if (messageButton) messageButton->raise();
}
#include "PreOpenPage.h"

#include <QPainter>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTimer>
#include <QRandomGenerator>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QDir>
#include <QFileInfo>
#include <cmath>

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

#include "../config/AssetPaths.h"
#include "HintBadge.h"
#include "SpriteAnimator.h"
#include "AudioManager.h"
#include "FloatingText.h"

PreOpenPage::PreOpenPage(GameManager* manager, AudioManager* audio, QWidget* parent)
    : QWidget(parent), gameManager(manager), audioManager(audio)
{
    qDebug() << "[VERIFY] New PreOpenPage constructor is running";
    backgroundPixmap = QPixmap(AssetPaths::shopInteriorBasic());
    setupUi();
    refreshPage();

    hintAnimTimer = new QTimer(this);
    connect(hintAnimTimer, &QTimer::timeout, this, &PreOpenPage::onHintAnimTick);
    hintAnimTimer->start(50);

    sparkleTimer = new QTimer(this);
    connect(sparkleTimer, &QTimer::timeout, this, &PreOpenPage::onSparkleTick);
    // Temporarily keep sparkle timer disabled for stability while investigating crashes.
    sparkleTimer->stop();
}


PreOpenPage::~PreOpenPage()
{
    if (hintAnimTimer) hintAnimTimer->stop();
    if (sparkleTimer) sparkleTimer->stop();
}

void PreOpenPage::setupUi()
{
    qDebug() << "[VERIFY] New PreOpenPage::setupUi is running";
    setAttribute(Qt::WA_StyledBackground, false);

    hudPanelLabel = nullptr;
    useHudPanelImage = false;

    hudFrame = new QFrame(this);
    hudFrame->setStyleSheet("QFrame { background-color: rgba(8,30,50,170); border:1px solid rgba(130,200,235,180); border-radius:10px; }");

    titleLabel = new QLabel("营业前准备", hudFrame);
    dayLabel = new QLabel(hudFrame);
    moneyLabel = new QLabel(hudFrame);
    employeeLabel = new QLabel(hudFrame);
    statusLabel = new QLabel(hudFrame);
    menuButton = new QPushButton("☰", hudFrame);
    for (QLabel* l : {titleLabel, dayLabel, moneyLabel, employeeLabel, statusLabel}) {
        l->setStyleSheet("QLabel { color:#eefaff; font-size:14px; font-weight:700; background-color: rgba(8,30,50,155); border:1px solid rgba(130,200,235,140); border-radius:8px; padding:4px 8px; }");
    }
    cleanIconLabel = new QLabel(hudFrame);
    dayIconLabel = new QLabel(hudFrame);
    moneyIconLabel = new QLabel(hudFrame);
    employeeIconLabel = new QLabel(hudFrame);
    statusIconLabel = new QLabel(hudFrame);
    for (QLabel* l : {cleanIconLabel, dayIconLabel, moneyIconLabel, employeeIconLabel, statusIconLabel}) { l->setStyleSheet("background: transparent;"); l->setAlignment(Qt::AlignCenter); }
    auto loadHudIcon=[&](QLabel* lbl,const QString& p1,const QString& p2=QString()){
        QPixmap px(p1); if(px.isNull() && !p2.isEmpty()) px=QPixmap(p2);
        if(px.isNull()) { qWarning() << "[PreOpenPage] missing hud icon:" << p1; lbl->setText("•"); return; }
        lbl->setPixmap(px.scaled(20,20,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    };
    loadHudIcon(cleanIconLabel, AssetPaths::iconCleanBroom());
    loadHudIcon(dayIconLabel, AssetPaths::iconCalendar());
    loadHudIcon(moneyIconLabel, AssetPaths::iconCoin());
    loadHudIcon(employeeIconLabel, AssetPaths::iconTeamCustomers(), AssetPaths::iconHelp());
    loadHudIcon(statusIconLabel, AssetPaths::iconNoticeBoard(), AssetPaths::iconHelp());

    menuButton->setCursor(Qt::PointingHandCursor);
    menuButton->setStyleSheet("QPushButton { background-color: rgba(8,30,50,155); color:#eefaff; border:1px solid rgba(130,200,235,150); border-radius:8px; font-size:18px; font-weight:700; }");
    connect(menuButton, &QPushButton::clicked, this, &PreOpenPage::menuRequested);


    noticeBoardLabel = new QLabel(this);
    noticeBoardLabel->setAlignment(Qt::AlignCenter);
    noticeBoardLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    noticeBoardLabel->setAutoFillBackground(false);
    noticeBoardLabel->setStyleSheet("QLabel { background: transparent; border: none; }");
    const QString boardPath = QDir(AssetPaths::assetRoot()).filePath("ui/elements/prop_notice_board.png");
    if (QFileInfo::exists(boardPath)) {
        noticeBoardPixmap = QPixmap(boardPath);
        noticeBoardLabel->setPixmap(noticeBoardPixmap);
        if (!noticeBoardPixmap.hasAlphaChannel()) { qWarning() << "[PreOpenPage] notice board image has no alpha channel:" << boardPath; }
    } else {
        qWarning() << "[PreOpenPage] Missing notice board asset:" << boardPath;
        noticeBoardLabel->setText("公告板");
        noticeBoardLabel->setStyleSheet("QLabel { background-color: rgba(30, 45, 60, 90); color: #f2f6ff; border: 1px solid rgba(180, 210, 230, 140); border-radius: 6px; }");
    }

    noticeHotspot = new QPushButton(this);
    shelfHotspot = new QPushButton(this);
    doorHotspot = new QPushButton(this);
    for (QPushButton* b : {noticeHotspot, shelfHotspot, doorHotspot}) {
        b->setStyleSheet("QPushButton { background: transparent; border: none; }");
        b->setCursor(Qt::PointingHandCursor);
    }

    noticeHintBadge = new HintBadge(this);
    shelfHintBadge = new HintBadge(this);
    doorHintBadge = new HintBadge(this);

    QPixmap panelBadge(AssetPaths::panelSquareCard());
    QPixmap noticeIcon(AssetPaths::iconHelp());
    if (noticeIcon.isNull()) noticeIcon = QPixmap(AssetPaths::iconAlert());
    QPixmap shelfIcon(AssetPaths::iconShopBag());
    if (shelfIcon.isNull()) shelfIcon = QPixmap(AssetPaths::iconDeliveryBoxes());
    if (shelfIcon.isNull()) qWarning() << "[PreOpenPage] missing shelf hotspot icon:" << AssetPaths::iconShopBag();
    QPixmap doorIcon(AssetPaths::iconConfirm());
    if (doorIcon.isNull()) doorIcon = QPixmap(AssetPaths::iconPlay());
    if (doorIcon.isNull()) qWarning() << "[PreOpenPage] missing door hotspot icon:" << AssetPaths::iconConfirm();

    noticeHintBadge->setBadgePixmaps(panelBadge, noticeIcon);
    shelfHintBadge->setBadgePixmaps(panelBadge, shelfIcon);
    doorHintBadge->setBadgePixmaps(panelBadge, doorIcon);
    noticeHintBadge->setPhaseOffset(0.0);
    shelfHintBadge->setPhaseOffset(0.7);
    doorHintBadge->setPhaseOffset(1.4);

    bottomHintLabel = new QLabel("点击提示区域完成开店准备，准备好后点击店门开始营业。", this);
    bottomHintLabel->setStyleSheet("QLabel { color: white; background-color: rgba(10, 35, 60, 170); border:1px solid rgba(170,220,255,150); border-radius:10px; padding: 6px 14px; font-size:14px; }");
    bottomHintLabel->setAlignment(Qt::AlignCenter);

    popupPanel = new QFrame(this);
    popupPanel->hide();
    popupPanel->setStyleSheet("QFrame { background-color: rgba(235, 249, 255, 225); border: 2px solid rgba(120, 190, 225, 210); border-radius: 14px; }");

    popupTitleLabel = new QLabel("商品准备", popupPanel);
    popupTitleLabel->setStyleSheet("QLabel { color: #174d7a; font-size: 20px; font-weight: bold; background: transparent; }");

    productTable = new QTableWidget(popupPanel);
    productTable->setColumnCount(8);
    productTable->setHorizontalHeaderLabels(QStringList() << "序号" << "图标" << "商品" << "类别" << "今日进价" << "建议售价" << "玩家售价" << "库存");
    productTable->setIconSize(QSize(40,40));
    productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    productTable->verticalHeader()->setVisible(false);
    productTable->setColumnWidth(0, 52);
    productTable->setColumnWidth(1, 78);
    productTable->setColumnWidth(2, 150);
    productTable->setColumnWidth(3, 90);
    productTable->setColumnWidth(4, 100);
    productTable->setColumnWidth(5, 100);
    productTable->setColumnWidth(6, 100);
    productTable->setColumnWidth(7, 80);
    productTable->horizontalHeader()->setFixedHeight(38);
    productTable->setAlternatingRowColors(true);
    productTable->setStyleSheet("QTableWidget{background: rgba(255,255,255,145); alternate-background-color: rgba(232,247,255,120); gridline-color:#a8d3ea; color:#0b4268; font-size:14px;} QTableWidget::item:selected{background-color: rgba(132,190,225,130);} QHeaderView::section{background: rgba(205,234,248,190); color:#0b4268; font-weight:bold; font-size:14px; border:1px solid #a8d3ea;}");
    productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    productIndexSpinBox = new QSpinBox(popupPanel);
    quantitySpinBox = new QSpinBox(popupPanel);
    priceProductIndexSpinBox = new QSpinBox(popupPanel);
    priceSpinBox = new QSpinBox(popupPanel);
    quantitySpinBox->setRange(1, 999);
    priceSpinBox->setRange(1, 9999);
    for (QSpinBox* sb : {productIndexSpinBox, quantitySpinBox, priceProductIndexSpinBox, priceSpinBox}) { sb->setKeyboardTracking(false); sb->setWrapping(false); }

    purchaseButton = new QPushButton("进货", popupPanel);
    setPriceButton = new QPushButton("修改售价", popupPanel);
    popupCloseButton = new QPushButton("保存并关闭", popupPanel);
    popupStartBusinessButton = new QPushButton("开始营业", popupPanel);
    const QString btnStyle = "QPushButton{background: rgba(67,129,164,230); border: 1px solid #b8e6ff; border-radius: 8px; color: white; font-weight: bold; font-size: 14px;}";
    purchaseButton->setStyleSheet(btnStyle); setPriceButton->setStyleSheet(btnStyle); popupCloseButton->setStyleSheet(btnStyle); popupStartBusinessButton->setStyleSheet(btnStyle);

    popupTitleLabel->setGeometry(0,24,1120,44);
    popupTitleLabel->setAlignment(Qt::AlignCenter);
    popupTitleLabel->setStyleSheet("QLabel{font-size:23px; font-weight:bold; color:#0b4268; background: transparent; border:none;}");
    productTable->setGeometry(70,102,980,380);
    QLabel* p1 = new QLabel("进货商品序号:", popupPanel); p1->setGeometry(80,518,130,34);
    productIndexSpinBox->setGeometry(214,518,95,34);
    QLabel* p2 = new QLabel("进货数量:", popupPanel); p2->setGeometry(326,518,120,34);
    quantitySpinBox->setGeometry(448,518,95,34);
    purchaseButton->setGeometry(850,516,170,40);
    QLabel* p3 = new QLabel("改价商品序号:", popupPanel); p3->setGeometry(80,570,130,34);
    priceProductIndexSpinBox->setGeometry(214,570,95,34);
    QLabel* p4 = new QLabel("新售价:", popupPanel); p4->setGeometry(326,570,120,34);
    priceSpinBox->setGeometry(448,570,95,34);
    setPriceButton->setGeometry(850,568,170,40);
    popupCloseButton->setGeometry(850,626,170,40);
    popupStartBusinessButton->setGeometry(670,626,170,40);
    for (QLabel* l : {p1,p2,p3,p4}) l->setStyleSheet("QLabel{color:#0b4268; font-size:13px; font-weight:bold; background:transparent;}");
    for (QSpinBox* sb : {productIndexSpinBox, quantitySpinBox, priceProductIndexSpinBox, priceSpinBox}) sb->setStyleSheet("QSpinBox{background: rgba(255,255,255,220); border: 1px solid #7db8dd; border-radius: 5px; color:#0b4268; font-size:13px;}");
    connect(noticeHotspot, &QPushButton::clicked, this, &PreOpenPage::onNoticeClicked);
    connect(shelfHotspot, &QPushButton::clicked, this, &PreOpenPage::onShelfClicked);
    connect(doorHotspot, &QPushButton::clicked, this, &PreOpenPage::onDoorClicked);
    connect(purchaseButton, &QPushButton::clicked, this, &PreOpenPage::onPurchaseClicked);
    connect(setPriceButton, &QPushButton::clicked, this, &PreOpenPage::onSetPriceClicked);
    connect(popupCloseButton, &QPushButton::clicked, this, &PreOpenPage::onPopupCloseClicked);
    connect(popupStartBusinessButton, &QPushButton::clicked, this, &PreOpenPage::onPopupStartBusinessClicked);
    connect(productTable, &QTableWidget::cellClicked, this, [this](int row, int){
        if (!productTable->item(row,0)) return;
        const int idx = productTable->item(row,0)->text().toInt();
        productIndexSpinBox->setValue(idx);
        priceProductIndexSpinBox->setValue(idx);
    });

    mopAnimLabel = new QLabel(this);
    mopAnimLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    mopAnimLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    mopAnimLabel->setAutoFillBackground(false);
    mopAnimLabel->setStyleSheet("QLabel{background: transparent; border: none;}");
    mopAnimator = new SpriteAnimator(mopAnimLabel, this);
    mopAnimator->setScaledSize(QSize(224,308));
    mopAnimator->setInterval(200);
    const QString mopPath = QDir(AssetPaths::assetRoot()).filePath("characters/animations/employee_mop_loop_strip.png");
    if (mopAnimator->loadStrip(mopPath, 6)) mopAnimator->start();
    QPixmap star(AssetPaths::iconStar());
    if (!star.isNull()) {
        for (int i=0;i<4;++i) {
            QLabel* s = new QLabel(this);
            s->setPixmap(star.scaled(18,18,Qt::KeepAspectRatio,Qt::SmoothTransformation));
            s->hide();
            sparkleLabels.push_back(s);
        }
    }
}

void PreOpenPage::paintEvent(QPaintEvent* event){QPainter p(this); if(!backgroundPixmap.isNull()){QPixmap s=backgroundPixmap.scaled(size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);int x=(s.width()-width())/2;int y=(s.height()-height())/2;p.drawPixmap(0,0,s,x,y,width(),height());} QWidget::paintEvent(event);}
void PreOpenPage::resizeEvent(QResizeEvent* event){QWidget::resizeEvent(event); updateLayoutBySize();}

void PreOpenPage::updateLayoutBySize()
{
    const int hudW = width()-36;
    const int hudH = 54;
    const int hudX = 18;
    const int hudY = 12;
    hudFrame->setGeometry(hudX, hudY, hudW, hudH);
    const int y0=8, h0=38, wItem=150;
    cleanIconLabel->setGeometry(12,y0,20,h0); titleLabel->setGeometry(36,y0,wItem,h0);
    dayIconLabel->setGeometry(190,y0,20,h0); dayLabel->setGeometry(214,y0,wItem,h0);
    moneyIconLabel->setGeometry(368,y0,20,h0); moneyLabel->setGeometry(392,y0,wItem+20,h0);
    employeeIconLabel->setGeometry(584,y0,20,h0); employeeLabel->setGeometry(608,y0,120,h0);
    statusIconLabel->setGeometry(734,y0,20,h0); statusLabel->setGeometry(758,y0,qMax(120, hudW-850),h0);
    menuButton->setGeometry(hudW-56, y0, 42, h0);

    if (!noticeBoardLabel || !noticeHotspot || !shelfHotspot || !doorHotspot) return;
    const qreal sx = width() / 1280.0;
    const qreal sy = height() / 720.0;
    noticeBoardLabel->setGeometry(int(width()*0.30), int(height()*0.28), int(82*sx), int(116*sy));
    if (!noticeBoardPixmap.isNull()) noticeBoardLabel->setPixmap(noticeBoardPixmap.scaled(noticeBoardLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    noticeHotspot->setGeometry(noticeBoardLabel->geometry());
    shelfHotspot->setGeometry(int(770*sx), int(340*sy), int(350*sx), int(180*sy));
    doorHotspot->setGeometry(int(180*sx), int(220*sy), int(220*sx), int(220*sy));

    if (noticeHintBadge) noticeHintBadge->setGeometry(noticeBoardLabel->x()+noticeBoardLabel->width()/2-20, noticeBoardLabel->y()-44, 40, 40);
    if (shelfHintBadge) shelfHintBadge->setGeometry(shelfHotspot->x()+shelfHotspot->width()-64, shelfHotspot->y()-52, 40, 40);
    if (doorHintBadge) doorHintBadge->setGeometry(doorHotspot->x()+doorHotspot->width()/2-20, doorHotspot->y()-50, 40, 40);

    const int hintW = qBound(420, int(width()*0.52), 760);
    bottomHintLabel->setGeometry((width()-hintW)/2, height()-56, hintW, 40);
    const int popupW = qMin(1120, int(width()*0.82));
    const int popupH = qMin(720, int(height()*0.82));
    popupPanel->setGeometry((width()-popupW)/2, (height()-popupH)/2, popupW, popupH);
    if (mopAnimLabel) mopAnimLabel->setGeometry(int(width()*0.42), int(height()*0.50), 224, 308);
}

void PreOpenPage::refreshHud(){
    titleLabel->setText(QString("清洁度:%1").arg(gameManager->store.cleanliness));
    dayLabel->setText(QString("当前天数: %1").arg(gameManager->store.currentDay));
    moneyLabel->setText(QString("当前资金: %1").arg(gameManager->store.money));
    employeeLabel->setText(QString("员工:%1").arg(gameManager->getEmployeeCount()));
    statusLabel->setText(QString("%1 / %2").arg(noticeViewed?"消息已查看":"消息未查看", goodsChecked?"商品已检查":"商品未检查"));
}
void PreOpenPage::refreshProductTable(){ const QVector<Product>& products=gameManager->store.products; productTable->setRowCount(products.size()); for(int i=0;i<products.size();++i){const Product&p=products[i]; productTable->setItem(i,0,new QTableWidgetItem(QString::number(i+1))); QTableWidgetItem* iconItem = new QTableWidgetItem(); QString iconPath = QDir(AssetPaths::assetRoot()).filePath("products/product_water.png"); if (p.name.contains("罐装茶饮")) iconPath = QDir(AssetPaths::assetRoot()).filePath("products/product_canned_tea.png"); else if (p.name.contains("袋装面包")||p.name.contains("面包")) iconPath = QDir(AssetPaths::assetRoot()).filePath("products/product_bread.png"); else if (p.name.contains("速食泡面")) iconPath = QDir(AssetPaths::assetRoot()).filePath("products/product_noodles.png"); else if (p.name.contains("薯片")) iconPath = QDir(AssetPaths::assetRoot()).filePath("products/product_chips.png"); else if (p.name.contains("巧克力")) iconPath = QDir(AssetPaths::assetRoot()).filePath("products/product_chocolate.png"); else if (p.name.contains("雨伞")) iconPath = QDir(AssetPaths::assetRoot()).filePath("products/product_umbrella.png"); else if (p.name.contains("电池")) iconPath = QDir(AssetPaths::assetRoot()).filePath("products/product_battery_light.png"); QPixmap icon(iconPath); if(icon.isNull()){ qWarning() << "[PreOpenPage] missing product icon:" << iconPath; icon = QPixmap(AssetPaths::iconShopBag()); } if(!icon.isNull()) iconItem->setIcon(QIcon(icon.scaled(36,36,Qt::KeepAspectRatio,Qt::SmoothTransformation))); productTable->setItem(i,1,iconItem); productTable->setItem(i,2,new QTableWidgetItem(p.name)); productTable->setItem(i,3,new QTableWidgetItem(p.category)); productTable->setItem(i,4,new QTableWidgetItem(QString::number(p.todayCost))); productTable->setItem(i,5,new QTableWidgetItem(QString::number(p.suggestedPrice))); productTable->setItem(i,6,new QTableWidgetItem(QString::number(p.playerPrice))); productTable->setItem(i,7,new QTableWidgetItem(QString::number(p.stock))); productTable->setRowHeight(i,54);} int maxIndex=products.isEmpty()?1:products.size(); productIndexSpinBox->setMaximum(maxIndex); priceProductIndexSpinBox->setMaximum(maxIndex);}
void PreOpenPage::refreshPage(){refreshHud(); refreshProductTable();}

void PreOpenPage::onNoticeClicked(){ if(audioManager) audioManager->playClickSfx(); QString msg="今日暂无明显市场消息。"; if(!gameManager->todayEvents.isEmpty()){ const Event&e=gameManager->todayEvents.first(); msg=QString("事件名称: %1\n来源: %2\n描述: %3\n是否大事件: %4\n可信度: %5").arg(e.name,e.source,e.description,e.isMajor?"是":"否",QString::number(e.credibility)); } showNoIconMessage(this,"今日市场消息",msg+"\n\n知道了"); noticeViewed=true; refreshHud(); }
void PreOpenPage::showGoodsPopup(){ if(audioManager) audioManager->playClickSfx(); popupTitleLabel->setText("商品准备"); popupStartBusinessButton->hide(); popupPanel->show(); popupPanel->raise(); goodsChecked=true; refreshHud(); }
void PreOpenPage::onShelfClicked(){ showGoodsPopup(); }
void PreOpenPage::showDoorPopup(){ QString extra = (!noticeViewed || !goodsChecked) ? "\n还有准备事项未确认，确定开始营业吗？" : ""; QString info=QString("准备开始营业？\n当前天数: %1\n当前资金: %2\n今日消息: %3\n商品准备: %4%5").arg(gameManager->store.currentDay).arg(gameManager->store.money).arg(noticeViewed?"已查看":"未查看").arg(goodsChecked?"已检查":"未检查").arg(extra); auto ret=askNoIconQuestion(this,"开始营业确认",info); if(ret==QMessageBox::Yes){ if(audioManager) audioManager->playConfirmSfx(); emit startBusinessClicked(); }}
void PreOpenPage::onDoorClicked(){ showDoorPopup(); }
void PreOpenPage::onPurchaseClicked(){ bool ok=gameManager->purchaseProduct(productIndexSpinBox->value()-1,quantitySpinBox->value()); QPoint p = mapFromGlobal(purchaseButton->mapToGlobal(purchaseButton->rect().center())); if(!ok){ if(audioManager) audioManager->playErrorSfx(); FloatingText::showText(this,"资金不足",p,"error"); showNoIconMessage(this,"提示","进货失败，请检查资金、库存容量或商品状态"); } else { if(audioManager) audioManager->playCoinSfx(); const int idx = productIndexSpinBox->value()-1; const QString n = (idx>=0 && idx<gameManager->store.products.size())?gameManager->store.products[idx].name:"商品"; FloatingText::showText(this,QString("+%1 %2").arg(quantitySpinBox->value()).arg(n),p,"coin"); } refreshPage(); }
void PreOpenPage::onSetPriceClicked(){ bool ok=gameManager->setProductPrice(priceProductIndexSpinBox->value()-1,priceSpinBox->value()); QPoint p = mapFromGlobal(setPriceButton->mapToGlobal(setPriceButton->rect().center())); if(!ok){ if(audioManager) audioManager->playErrorSfx(); FloatingText::showText(this,"商品序号无效",p,"error"); showNoIconMessage(this,"提示","修改售价失败，请检查商品序号和售价"); } else { if(audioManager) audioManager->playConfirmSfx(); FloatingText::showText(this,"售价已修改",p,"success"); } refreshPage(); }
void PreOpenPage::onPopupCloseClicked(){ if(audioManager) audioManager->playConfirmSfx(); popupPanel->hide(); goodsChecked=true; refreshHud(); }
void PreOpenPage::onPopupStartBusinessClicked(){ emit startBusinessClicked(); }

QPoint PreOpenPage::hotspotCenter(QWidget* hotspot) const { return hotspot->geometry().center(); }
void PreOpenPage::onHintAnimTick(){ if(!noticeHintBadge || !shelfHintBadge || !doorHintBadge || !noticeBoardLabel || !shelfHotspot || !doorHotspot) return; ++animTick; const qreal phase = animTick * 0.35; const int dy = static_cast<int>(-6.0 * (0.5 + 0.5 * std::sin(phase)));
    noticeHintBadge->setGeometry(noticeBoardLabel->x()+noticeBoardLabel->width()/2-20, noticeBoardLabel->y()-44+dy, 40, 40);
    shelfHintBadge->setGeometry(shelfHotspot->x()+shelfHotspot->width()-64, shelfHotspot->y()-52+dy, 40, 40);
    doorHintBadge->setGeometry(doorHotspot->x()+doorHotspot->width()/2-20, doorHotspot->y()-50+dy, 40, 40);
    const qreal breathe = 0.75 + 0.25 * (0.5 + 0.5 * std::sin(phase));
    noticeHintBadge->setWindowOpacity(breathe);
    shelfHintBadge->setWindowOpacity(breathe);
    doorHintBadge->setWindowOpacity(breathe);
    noticeHintBadge->setPhase(phase);
    shelfHintBadge->setPhase(phase);
    doorHintBadge->setPhase(phase);
}

void PreOpenPage::onSparkleTick(){ if(!sparkleTimer || sparkleLabels.isEmpty()) return; QLabel* free=nullptr; for(QLabel* s:sparkleLabels){ if(!s->isVisible()){ free=s; break; } } if(!free){ sparkleTimer->start(QRandomGenerator::global()->bounded(1500,4001)); return; }
    const int r = QRandomGenerator::global()->bounded(3); QWidget* target = r==0?static_cast<QWidget*>(noticeHotspot):(r==1?static_cast<QWidget*>(shelfHotspot):static_cast<QWidget*>(doorHotspot)); QPoint c=hotspotCenter(target); int x=c.x()+QRandomGenerator::global()->bounded(-22,23); int y=c.y()+QRandomGenerator::global()->bounded(-20,21); free->move(x,y); free->show();
    auto *eff=new QGraphicsOpacityEffect(free); free->setGraphicsEffect(eff); auto *anim=new QPropertyAnimation(eff,"opacity",free); anim->setDuration(900); anim->setKeyValueAt(0.0,0.0); anim->setKeyValueAt(0.5,1.0); anim->setKeyValueAt(1.0,0.0); connect(anim,&QPropertyAnimation::finished,free,[free,eff](){ free->hide(); free->setGraphicsEffect(nullptr); eff->deleteLater(); }); anim->start(QAbstractAnimation::DeleteWhenStopped);
    auto *move=new QPropertyAnimation(free,"pos",free); move->setDuration(900); move->setStartValue(free->pos()); move->setEndValue(free->pos()+QPoint(0,-6)); move->start(QAbstractAnimation::DeleteWhenStopped);
    sparkleTimer->start(QRandomGenerator::global()->bounded(1500,4001));
}
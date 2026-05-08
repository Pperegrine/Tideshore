#include "MainWindow.h"
#include "../config/AssetPaths.h"
#include "../logic/SaveSystem.h"
#include "AudioManager.h"
#include "ClickEffectWidget.h"
#include "FloatingText.h"
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDir>
#include <QApplication>
#include <QMouseEvent>

static void showNoIconMessageMain(QWidget* parent, const QString& title, const QString& text){
    QMessageBox box(parent);
    box.setIcon(QMessageBox::NoIcon);
    box.setWindowTitle(title);
    box.setText(text);
    box.setStandardButtons(QMessageBox::Ok);
    box.exec();
}

static QMessageBox::StandardButton askNoIconQuestionMain(QWidget* parent, const QString& title, const QString& text){
    QMessageBox box(parent);
    box.setIcon(QMessageBox::NoIcon);
    box.setWindowTitle(title);
    box.setText(text);
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box.setDefaultButton(QMessageBox::No);
    return static_cast<QMessageBox::StandardButton>(box.exec());
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      stackedWidget(nullptr),
      introPage(nullptr),
      startPage(nullptr),
      preOpenPage(nullptr),
      businessPage(nullptr),
      settlementPage(nullptr),
      upgradePage(nullptr)
{
    setWindowTitle("潮汐小镇");
    resize(1000, 650);

    gameManager.startNewGame();

    audioManager = new AudioManager(this);
    audioManager->setVolume(0.25f);
    bgmPath = QDir(AssetPaths::assetRoot()).filePath("audio/bgm_seaside_shop_loop.mp3");

    stackedWidget = new QStackedWidget(this);
    introPage = new IntroPage(this);
    startPage = new StartPage(this);
    preOpenPage = new PreOpenPage(&gameManager, audioManager, this);
    businessPage = new BusinessPage(&gameManager, audioManager, this);
    settlementPage = new SettlementPage(&gameManager, audioManager, this);
    upgradePage = new UpgradePage(&gameManager, audioManager, this);

    stackedWidget->addWidget(introPage);
    stackedWidget->addWidget(startPage);
    stackedWidget->addWidget(preOpenPage);
    stackedWidget->addWidget(businessPage);
    stackedWidget->addWidget(settlementPage);
    stackedWidget->addWidget(upgradePage);
    setCentralWidget(stackedWidget);

    applyGlobalStyle();
    setupOverlayMenu();
    qApp->installEventFilter(this);

    const QDir root(AssetPaths::assetRoot());
    const QString normalPath = root.filePath("ui/cursor/cursor_normal.png");
    const QString hoverPath = root.filePath("ui/cursor/cursor_hover.png");
    const QString pressedPath = root.filePath("ui/cursor/cursor_pressed.png");
    QPixmap normalPx(normalPath), hoverPx(hoverPath), pressedPx(pressedPath);
    if (normalPx.isNull() || hoverPx.isNull() || pressedPx.isNull()) {
        if (normalPx.isNull()) qWarning() << "[Cursor] missing:" << normalPath;
        if (hoverPx.isNull()) qWarning() << "[Cursor] missing:" << hoverPath;
        if (pressedPx.isNull()) qWarning() << "[Cursor] missing:" << pressedPath;
        unsetCursor();
        customCursorReady = false;
    } else {
        normalCursor = QCursor(normalPx, 0, 0);
        hoverCursor = QCursor(hoverPx, 0, 0);
        pressedCursor = QCursor(pressedPx, 0, 0);
        setCursor(normalCursor);
        customCursorReady = true;
    }

    stackedWidget->setCurrentWidget(introPage);

    connect(introPage, &IntroPage::introFinished, this, [this]() {
        showStartPage();
        if (audioManager && audioManager->isBgmEnabled()) {
            audioManager->playBgm(bgmPath);
        }
    });
    connect(startPage, &StartPage::clickSfxRequested, this, [this]() { if (audioManager) audioManager->playClickSfx(); });
    connect(startPage, &StartPage::startGameRequested, this, &MainWindow::showPreOpenPage);
    connect(startPage, &StartPage::continueGameRequested, this, [this]() {
        QPoint p(width()*0.5, height()*0.72);
        if (!SaveSystem::hasSave()) {
            if (audioManager) audioManager->playErrorSfx();
            FloatingText::showText(this, "暂无存档", p, "error");
            showStartPage();
            return;
        }
        if (SaveSystem::loadGame(gameManager)) {
            if (audioManager) audioManager->playConfirmSfx();
            FloatingText::showText(this, "读取成功", p, "success");
            showPreOpenPage();
        } else {
            if (audioManager) audioManager->playErrorSfx();
            FloatingText::showText(this, "读取失败", p, "error");
            showStartPage();
        }
    });
    connect(preOpenPage, &PreOpenPage::menuRequested, this, &MainWindow::showMenuOverlay);
    connect(businessPage, &BusinessPage::menuRequested, this, &MainWindow::showMenuOverlay);
    connect(preOpenPage, &PreOpenPage::startBusinessClicked, this, &MainWindow::showBusinessPage);
    connect(businessPage, &BusinessPage::businessFinished, this, &MainWindow::showSettlementPage);
    connect(settlementPage, &SettlementPage::upgradeClicked, this, &MainWindow::showUpgradePage);
    connect(upgradePage, &UpgradePage::nextDayClicked, this, [this]() {
        showPreOpenPage();
        if (!SaveSystem::saveGame(gameManager)) qWarning() << "[SaveSystem] Auto-save failed after next day";
    });
}

void MainWindow::applyGlobalStyle()
{
    setStyleSheet(
        "QMainWindow { background-color: #eef7fb; }"
        "QWidget { background-color: #eef7fb; }"
        "QLabel { color: #1f3b52; font-size: 14px; }"
        "QPushButton {"
        "  background-color: #bfe7ff;"
        "  color: #1b3a4a;"
        "  border: 1px solid #9fd4f2;"
        "  border-radius: 8px;"
        "  padding: 6px 12px;"
        "}"
        "QPushButton:hover { background-color: #a9dcfb; }"
        "QTableWidget { background-color: #ffffff; gridline-color: #dce5ec; border: 1px solid #c8dff0; }"
        "QHeaderView::section { background-color: #d9effd; color: #244a61; border: 1px solid #c7e0f2; padding: 4px; }"
        "QTableWidget::item:selected { background-color: #d8ecfb; color: #18394b; }"
        "QTextEdit { background-color: #ffffff; border: 1px solid #b8d9ee; border-radius: 6px; }"
        "QSpinBox { background-color: #ffffff; border: 1px solid #b8d9ee; border-radius: 5px; padding: 2px 4px; }"
    );
}

void MainWindow::showStartPage(){ startPage->refreshContinueButtonState(SaveSystem::hasSave()); stackedWidget->setCurrentWidget(startPage); }
void MainWindow::showPreOpenPage(){ preOpenPage->refreshPage(); stackedWidget->setCurrentWidget(preOpenPage); }
void MainWindow::showBusinessPage(){ businessPage->refreshPage(); stackedWidget->setCurrentWidget(businessPage); }
void MainWindow::showSettlementPage(){ settlementPage->refreshPage(); stackedWidget->setCurrentWidget(settlementPage); }
void MainWindow::showUpgradePage(){ upgradePage->refreshPage(); stackedWidget->setCurrentWidget(upgradePage); }

void MainWindow::setupOverlayMenu(){
    menuButton = new QPushButton("菜单", this);
    menuButton->hide();
    connect(menuButton,&QPushButton::clicked,this,[this](){ if (audioManager) audioManager->playClickSfx(); toggleOverlayMenu(); });

    menuOverlay = new QWidget(this);
    menuOverlay->hide();
    menuOverlay->setStyleSheet("background-color: rgba(0, 15, 30, 110);");
    menuPanel = new QWidget(menuOverlay);
    menuPanel->setStyleSheet("background-color: rgba(8, 32, 55, 225); border:1px solid rgba(140, 215, 255, 190); border-radius:16px; color:white;");
    QVBoxLayout* l = new QVBoxLayout(menuPanel);
    l->setContentsMargins(30, 24, 30, 24);
    l->setSpacing(12);
    QPushButton* resume = new QPushButton("继续游戏", menuPanel);
    QPushButton* saveBtn = new QPushButton("保存游戏", menuPanel);
    QPushButton* loadBtn = new QPushButton("读取存档", menuPanel);
    QPushButton* backStartBtn = new QPushButton("返回首页", menuPanel);
    bgmToggleButton = new QPushButton(menuPanel);
    updateBgmButtonText();
    volumeSlider = new QSlider(Qt::Horizontal, menuPanel);
    volumeSlider->setRange(0,100); volumeSlider->setValue(25);
    QPushButton* about = new QPushButton("关于游戏", menuPanel);
    QPushButton* quit = new QPushButton("退出游戏", menuPanel);
    QWidget* primaryBlock = new QWidget(menuPanel);
    QVBoxLayout* primaryLayout = new QVBoxLayout(primaryBlock);
    primaryLayout->setContentsMargins(0,0,0,0);
    primaryLayout->setSpacing(12);

    QWidget* musicBlock = new QWidget(menuPanel);
    musicBlock->setObjectName("musicBlock");
    musicBlock->setStyleSheet("#musicBlock{background-color: rgba(22,58,86,95); border: 1px solid rgba(150,220,255,70); border-radius: 10px;}");
    QVBoxLayout* musicLayout = new QVBoxLayout(musicBlock);
    musicLayout->setContentsMargins(16, 12, 16, 12);
    musicLayout->setSpacing(6);

    QWidget* secondaryBlock = new QWidget(menuPanel);
    QVBoxLayout* secondaryLayout = new QVBoxLayout(secondaryBlock);
    secondaryLayout->setContentsMargins(0,0,0,0);
    secondaryLayout->setSpacing(12);

    QLabel* nowPlayingLabel = new QLabel("正在播放", musicBlock);
    nowPlayingLabel->setAlignment(Qt::AlignCenter);
    nowPlayingLabel->setStyleSheet("QLabel{color:#dff4ff; font-size:14px; font-weight:700; background: transparent; border:none;}");
    QLabel* trackLabel = new QLabel("岸部真明 - 流行的云", musicBlock);
    trackLabel->setAlignment(Qt::AlignCenter);
    trackLabel->setStyleSheet("QLabel{color:#c9ebff; font-size:13px; background: transparent; border:none;}");
    QLabel* volumeLabel = new QLabel("音量", musicBlock);
    volumeLabel->setAlignment(Qt::AlignCenter);
    volumeLabel->setStyleSheet("QLabel{color:#dff4ff; font-size:13px; font-weight:700; background: transparent; border:none;}");
    for(QPushButton* b:{resume,saveBtn,loadBtn,backStartBtn,bgmToggleButton,about,quit}) {
        b->setStyleSheet("QPushButton{background-color: rgba(45,95,130,220); border: 1px solid rgba(150,220,255,180); border-radius:8px; color:white; font-weight:bold; font-size:13px; min-height:30px; padding:5px 10px;}");
        b->setMinimumWidth(220);
        b->setMaximumWidth(240);
    }

    primaryLayout->addWidget(resume, 0, Qt::AlignHCenter);
    primaryLayout->addWidget(saveBtn, 0, Qt::AlignHCenter);
    primaryLayout->addWidget(loadBtn, 0, Qt::AlignHCenter);
    primaryLayout->addWidget(backStartBtn, 0, Qt::AlignHCenter);

    musicLayout->addWidget(bgmToggleButton, 0, Qt::AlignHCenter);
    musicLayout->addSpacing(2);
    musicLayout->addWidget(nowPlayingLabel);
    musicLayout->addWidget(trackLabel);
    musicLayout->addSpacing(2);
    musicLayout->addWidget(volumeLabel);
    musicLayout->addWidget(volumeSlider);

    secondaryLayout->addWidget(about, 0, Qt::AlignHCenter);
    secondaryLayout->addWidget(quit, 0, Qt::AlignHCenter);

    l->addWidget(primaryBlock);
    l->addSpacing(12);
    l->addWidget(musicBlock);
    l->addSpacing(14);
    l->addWidget(secondaryBlock);
    connect(resume,&QPushButton::clicked,this,[this](){ if (audioManager) audioManager->playClickSfx(); toggleOverlayMenu(); });
    connect(saveBtn,&QPushButton::clicked,this,[this,saveBtn](){
        const bool ok = SaveSystem::saveGame(gameManager);
        QPoint p = mapFromGlobal(saveBtn->mapToGlobal(saveBtn->rect().center()));
        if(ok){ if(audioManager) audioManager->playConfirmSfx(); FloatingText::showText(this,"保存成功",p,"success"); }
        else { if(audioManager) audioManager->playErrorSfx(); FloatingText::showText(this,"保存失败",p,"error"); }
    });
    connect(loadBtn,&QPushButton::clicked,this,[this,loadBtn](){
        QPoint p = mapFromGlobal(loadBtn->mapToGlobal(loadBtn->rect().center()));
        if(!SaveSystem::hasSave()){ if(audioManager) audioManager->playErrorSfx(); FloatingText::showText(this,"暂无存档",p,"error"); return; }
        if(askNoIconQuestionMain(this,"读取存档","读取存档会覆盖当前进度，确定继续吗？")!=QMessageBox::Yes) return;
        const bool ok = SaveSystem::loadGame(gameManager);
        if(ok){ if(audioManager) audioManager->playConfirmSfx(); FloatingText::showText(this,"读取成功",p,"success"); showPreOpenPage(); if(menuOverlay) menuOverlay->hide(); }
        else { if(audioManager) audioManager->playErrorSfx(); FloatingText::showText(this,"读取失败",p,"error"); }
    });
    connect(backStartBtn,&QPushButton::clicked,this,[this](){
        if(askNoIconQuestionMain(this,"返回首页","确定返回首页吗？未保存的进度可能丢失。")!=QMessageBox::Yes) return;
        if(audioManager) audioManager->playConfirmSfx();
        if(menuOverlay) menuOverlay->hide();
        showStartPage();
    });
    connect(bgmToggleButton,&QPushButton::clicked,this,[this](){ if (audioManager) { audioManager->playClickSfx(); audioManager->setBgmEnabled(!audioManager->isBgmEnabled()); } updateBgmButtonText();});
    connect(volumeSlider,&QSlider::valueChanged,this,[this](int v){ audioManager->setVolume(v/100.0f);});
    connect(about,&QPushButton::clicked,this,[this](){ if (audioManager) audioManager->playClickSfx(); showNoIconMessageMain(this,"关于游戏","潮汐小镇 Tideshore\n一款蓝白海边便利店模拟经营小游戏。\n当前版本：Demo\n制作方向：经营、升级、顾客、事件、结算。");});
    connect(quit,&QPushButton::clicked,this,[this](){ if (audioManager) audioManager->playClickSfx(); if(askNoIconQuestionMain(this,"退出","确定要退出游戏吗？")==QMessageBox::Yes) close();});
}
void MainWindow::updateBgmButtonText(){ if(bgmToggleButton) bgmToggleButton->setText(audioManager && audioManager->isBgmEnabled()?"背景音乐：开":"背景音乐：关"); }
void MainWindow::showMenuOverlay(){ if(!menuOverlay) return; if(!menuOverlay->isVisible()){ menuOverlay->show(); menuOverlay->raise(); }}
void MainWindow::toggleOverlayMenu(){ if(!menuOverlay) return; menuOverlay->setVisible(!menuOverlay->isVisible()); if(menuOverlay->isVisible()) menuOverlay->raise(); }
void MainWindow::resizeEvent(QResizeEvent* e){ QMainWindow::resizeEvent(e); if(menuOverlay){ menuOverlay->setGeometry(rect()); if(menuPanel) menuPanel->setGeometry((width()-500)/2,(height()-580)/2,500,580);} }
void MainWindow::keyPressEvent(QKeyEvent* e){ if(e->key()==Qt::Key_Escape){ toggleOverlayMenu(); return; } QMainWindow::keyPressEvent(e);}

bool MainWindow::isClickableWidget(QWidget* w) const {
    if (!w) return false;
    if (qobject_cast<QPushButton*>(w) || qobject_cast<QToolButton*>(w)) return true;
    if (w->metaObject()->className() && QString::fromLatin1(w->metaObject()->className()).contains("PixelImageButton")) return true;
    return w->cursor().shape() == Qt::PointingHandCursor;
}

void MainWindow::applyCursorForWidget(QWidget* w, bool pressed) {
    if (!customCursorReady) return;
    if (pressed && isClickableWidget(w)) setCursor(pressedCursor);
    else if (isClickableWidget(w)) setCursor(hoverCursor);
    else setCursor(normalCursor);
}

void MainWindow::spawnClickEffect(const QPoint& globalPos) {
    const QString fxPath = QDir(AssetPaths::assetRoot()).filePath("ui/effects/fx_click_star_strip.png");
    QPoint localPos = mapFromGlobal(globalPos);
    auto* fx = new ClickEffectWidget(fxPath, localPos, this);
    fx->raise();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    QWidget* w = qobject_cast<QWidget*>(watched);
    if (!customCursorReady || !w) return QMainWindow::eventFilter(watched, event);
    switch (event->type()) {
    case QEvent::Enter:
        applyCursorForWidget(w, false);
        break;
    case QEvent::Leave:
        setCursor(normalCursor);
        break;
    case QEvent::MouseButtonPress: {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) applyCursorForWidget(w, true);
        break;
    }
    case QEvent::MouseButtonRelease: {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            applyCursorForWidget(w, false);
            spawnClickEffect(me->globalPosition().toPoint());
        }
        break;
    }
    default:
        break;
    }
    return QMainWindow::eventFilter(watched, event);
}
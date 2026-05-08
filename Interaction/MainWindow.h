#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include <QString>
#include <QCursor>

#include "../logic/GameManager.h"
#include "StartPage.h"
#include "IntroPage.h"
#include "PreOpenPage.h"
#include "BusinessPage.h"
#include "SettlementPage.h"
#include "UpgradePage.h"

class QPushButton;
class QSlider;
class QLabel;
class AudioManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    GameManager gameManager;
    QStackedWidget* stackedWidget;
    IntroPage* introPage;
    StartPage* startPage;
    PreOpenPage* preOpenPage;
    BusinessPage* businessPage;
    SettlementPage* settlementPage;
    UpgradePage* upgradePage;
    AudioManager* audioManager = nullptr;
    QWidget* menuOverlay = nullptr;
    QWidget* menuPanel = nullptr;
    QPushButton* menuButton = nullptr;
    QPushButton* bgmToggleButton = nullptr;
    QSlider* volumeSlider = nullptr;
    QString bgmPath;

    void applyGlobalStyle();
    void setupOverlayMenu();
    void toggleOverlayMenu();
    void showMenuOverlay();
    void updateBgmButtonText();

private slots:
    void showStartPage();
    void showPreOpenPage();
    void showBusinessPage();
    void showSettlementPage();
    void showUpgradePage();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    bool isClickableWidget(QWidget* w) const;
    void applyCursorForWidget(QWidget* w, bool pressed);
    void spawnClickEffect(const QPoint& globalPos);
    QCursor normalCursor;
    QCursor hoverCursor;
    QCursor pressedCursor;
    bool customCursorReady = false;
};

#endif // MAINWINDOW_H
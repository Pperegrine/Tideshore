#ifndef BUSINESSPAGE_H
#define BUSINESSPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QVector>
#include <QPointF>
#include <QSizeF>
#include <QStringList>

#include "../logic/GameManager.h"

class QFrame;
class HintBadge;
class QTimer;
class SpriteAnimator;
class AudioManager;

class BusinessPage : public QWidget
{
    Q_OBJECT
public:
    explicit BusinessPage(GameManager* manager, AudioManager* audio, QWidget* parent = nullptr);
    void refreshPage();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

signals:
    void businessFinished();
    void menuRequested();

private slots:
    void onCleanClicked();
    void onFinishBusinessClicked();

private:
    GameManager* gameManager = nullptr;
    AudioManager* audioManager = nullptr;
    QPixmap backgroundPixmap;

    QFrame* hudFrame = nullptr;
    QLabel* titleLabel = nullptr;
    QLabel* dayLabel = nullptr;
    QLabel* moneyLabel = nullptr;
    QLabel* cleanlinessLabel = nullptr;
    QLabel* employeeLabel = nullptr;
    QLabel* statusLabel = nullptr;
    QPushButton* menuButton = nullptr;

    QPushButton* messageButton = nullptr;
    QPushButton* checkoutHotspot = nullptr;
    QPushButton* shelfHotspot = nullptr;
    QPushButton* doorHotspot = nullptr;

    HintBadge* checkoutHintBadge = nullptr;
    HintBadge* shelfHintBadge = nullptr;
    HintBadge* doorHintBadge = nullptr;
    HintBadge* messageHintBadge = nullptr;

    QFrame* bottomBar = nullptr;
    QLabel* bottomTipLabel = nullptr;
    QPushButton* cleanButton = nullptr;
    QPushButton* finishBusinessButton = nullptr;

    int animTick = 0;
    QTimer* hintAnimTimer = nullptr;
    QLabel* mopAnimLabel = nullptr;
    SpriteAnimator* mopAnimator = nullptr;
    QLabel* cashierAnimLabel = nullptr;
    SpriteAnimator* cashierAnimator = nullptr;
    QLabel* stockClerkAnimLabel = nullptr;
    SpriteAnimator* stockClerkAnimator = nullptr;
    QLabel* analystAnimLabel = nullptr;
    SpriteAnimator* analystAnimator = nullptr;
    QLabel* promoterAnimLabel = nullptr;
    SpriteAnimator* promoterAnimator = nullptr;
    void setupEmployeeSceneAnimators();
    void updateEmployeeSceneAnimatorLayout();
    void refreshEmployeeSceneVisibility();

    void setupUi();
    void updateLayoutBySize();
    void refreshInfo();
    void showTodayEventPopup();
    void showCheckoutPopup();
    void showShelfPopup();
    void showEndBusinessPopup();
};

#endif
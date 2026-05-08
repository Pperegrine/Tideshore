#ifndef PREOPENPAGE_H
#define PREOPENPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QSpinBox>
#include <QPixmap>
#include <QVector>

#include "../logic/GameManager.h"

class QFrame;
class QTimer;
class HintBadge;
class SpriteAnimator;
class AudioManager;

class PreOpenPage : public QWidget
{
    Q_OBJECT

public:
    explicit PreOpenPage(GameManager* manager, AudioManager* audio, QWidget* parent = nullptr);
    ~PreOpenPage() override;
    void refreshPage();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

signals:
    void startBusinessClicked();
    void menuRequested();

private slots:
    void onNoticeClicked();
    void onShelfClicked();
    void onDoorClicked();
    void onPurchaseClicked();
    void onSetPriceClicked();
    void onPopupCloseClicked();
    void onPopupStartBusinessClicked();
    void onHintAnimTick();
    void onSparkleTick();

private:
    GameManager* gameManager = nullptr;
    AudioManager* audioManager = nullptr;
    QPixmap backgroundPixmap;

    bool noticeViewed = false;
    bool goodsChecked = false;
    int animTick = 0;

    QLabel* hudPanelLabel = nullptr;
    bool useHudPanelImage = false;
    QFrame* hudFrame = nullptr;
    QLabel* titleLabel = nullptr;
    QLabel* dayLabel = nullptr;
    QLabel* moneyLabel = nullptr;
    QLabel* employeeLabel = nullptr;
    QLabel* statusLabel = nullptr;
    QPushButton* menuButton = nullptr;
    QLabel* cleanIconLabel = nullptr;
    QLabel* dayIconLabel = nullptr;
    QLabel* moneyIconLabel = nullptr;
    QLabel* employeeIconLabel = nullptr;
    QLabel* statusIconLabel = nullptr;
    QLabel* bottomHintLabel = nullptr;

    QLabel* noticeBoardLabel = nullptr;
    QPixmap noticeBoardPixmap;
    QPushButton* noticeHotspot = nullptr;
    QPushButton* shelfHotspot = nullptr;
    QPushButton* doorHotspot = nullptr;

    HintBadge* noticeHintBadge = nullptr;
    HintBadge* shelfHintBadge = nullptr;
    HintBadge* doorHintBadge = nullptr;

    QFrame* popupPanel = nullptr;
    QLabel* popupTitleLabel = nullptr;
    QTableWidget* productTable = nullptr;
    QSpinBox* productIndexSpinBox = nullptr;
    QSpinBox* quantitySpinBox = nullptr;
    QPushButton* purchaseButton = nullptr;
    QSpinBox* priceProductIndexSpinBox = nullptr;
    QSpinBox* priceSpinBox = nullptr;
    QPushButton* setPriceButton = nullptr;
    QPushButton* popupCloseButton = nullptr;
    QPushButton* popupStartBusinessButton = nullptr;

    QTimer* hintAnimTimer = nullptr;
    QTimer* sparkleTimer = nullptr;
    QVector<QLabel*> sparkleLabels;
    // sprite animation members (used in PreOpenPage.cpp setupUi/updateLayoutBySize)
    QLabel* mopAnimLabel = nullptr;
    SpriteAnimator* mopAnimator = nullptr;

    void setupUi();
    void updateLayoutBySize();
    void refreshProductTable();
    void refreshHud();
    void showGoodsPopup();
    void showDoorPopup();
    QPoint hotspotCenter(QWidget* hotspot) const;
};

#endif // PREOPENPAGE_H
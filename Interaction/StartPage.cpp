#include "StartPage.h"

#include <QApplication>
#include <QPainter>
#include <QPaintEvent>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QFont>
#include <QDebug>

#include "PixelImageButton.h"
#include "../config/AssetPaths.h"

StartPage::StartPage(QWidget* parent)
    : QWidget(parent),
      backgroundPixmap(AssetPaths::exists(AssetPaths::townStreetDay()) ? QPixmap(AssetPaths::townStreetDay()) : QPixmap()),
      logoPixmap(AssetPaths::exists(AssetPaths::logoTideshore()) ? QPixmap(AssetPaths::logoTideshore()) : QPixmap()),
      logoLabel(new QLabel(this)),
      menuPanel(new QFrame(this)),
      startButton(new PixelImageButton("开始游戏", this)),
      continueButton(new PixelImageButton("继续游戏", this)),
      quitButton(new PixelImageButton("退出游戏", this))
{
    setMinimumSize(960, 540);

    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    logoLabel->setStyleSheet("background: transparent;");
    logoLabel->setAutoFillBackground(false);

    qDebug() << "[StartPage] logo size:" << logoPixmap.size()
             << "has alpha:" << logoPixmap.hasAlphaChannel();

    if (!logoPixmap.isNull()) {
        logoLabel->setPixmap(logoPixmap);
    } else {
        logoLabel->setText("Tideshore\n潮汐小镇");
        logoLabel->setStyleSheet("QLabel { background: transparent; color: #ffffff; font-size: 42px; font-weight: bold; }");
    }

    menuPanel->setStyleSheet(
        "QFrame {"
        " background-color: rgba(20, 55, 80, 35);"
        " border: 1px solid rgba(210, 240, 255, 40);"
        " border-radius: 8px;"
        "}"
    );

    QVBoxLayout* panelLayout = new QVBoxLayout(menuPanel);
    panelLayout->setContentsMargins(24, 24, 24, 24);
    panelLayout->setSpacing(18);

    QFont buttonFont = startButton->font();
    buttonFont.setPointSize(15);
    startButton->setFont(buttonFont);
    continueButton->setFont(buttonFont);
    quitButton->setFont(buttonFont);

    startButton->setButtonImages(AssetPaths::btnBlueNormal(), AssetPaths::btnBlueHover(), AssetPaths::btnBluePressed());
    continueButton->setButtonImages(AssetPaths::btnBlueNormal(), AssetPaths::btnBlueHover(), AssetPaths::btnBluePressed());
    quitButton->setButtonImages(AssetPaths::btnBlueNormal(), AssetPaths::btnBlueHover(), AssetPaths::btnBluePressed());

    continueButton->setEnabled(false);

    panelLayout->addWidget(startButton, 0, Qt::AlignHCenter);
    panelLayout->addWidget(continueButton, 0, Qt::AlignHCenter);
    panelLayout->addWidget(quitButton, 0, Qt::AlignHCenter);

    connect(startButton, &QPushButton::clicked, this, [this]() {
        emit clickSfxRequested();
        emit startGameRequested();
    });
    connect(continueButton, &QPushButton::clicked, this, [this]() {
        emit clickSfxRequested();
        emit continueGameRequested();
    });
    connect(quitButton, &QPushButton::clicked, this, [this]() {
        emit clickSfxRequested();
        qApp->quit();
    });

    updateLayoutBySize();
}

void StartPage::refreshContinueButtonState(bool hasSave)
{
    continueButton->setEnabled(hasSave);
    continueButton->setToolTip(hasSave ? QString() : QStringLiteral("暂无存档"));
}

void StartPage::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateLayoutBySize();
}

void StartPage::updateLayoutBySize()
{
    const int w = width();
    const int h = height();

    int logoW = qMin(static_cast<int>(w * 0.42), 560);
    logoW = qMax(logoW, 300);

    int logoH = static_cast<int>(logoW * 0.38);
    if (!logoPixmap.isNull() && logoPixmap.width() > 0) {
        logoH = logoW * logoPixmap.height() / logoPixmap.width();
    }

    const int logoX = (w - logoW) / 2;
    const int logoY = static_cast<int>(h * 0.11);
    logoLabel->setGeometry(logoX, logoY, logoW, logoH);

    if (!logoPixmap.isNull()) {
        logoLabel->setPixmap(logoPixmap.scaled(logoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    qDebug() << "[StartPage] logoLabel geometry:" << logoLabel->geometry();

    const int panelW = qBound(410, static_cast<int>(w * 0.38), 420);
    const int panelH = qBound(260, static_cast<int>(h * 0.33), 290);
    const int panelX = (w - panelW) / 2;
    const int panelY = qMax(logoY + logoH + 20, static_cast<int>(h * 0.50));
    menuPanel->setGeometry(panelX, panelY, panelW, panelH);

    const int btnW = 360;
    const int btnH = 78;
    startButton->setFixedSize(btnW, btnH);
    continueButton->setFixedSize(btnW, btnH);
    quitButton->setFixedSize(btnW, btnH);
}

void StartPage::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (!backgroundPixmap.isNull()) {
        QPixmap scaled = backgroundPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        const int x = (scaled.width() - width()) / 2;
        const int y = (scaled.height() - height()) / 2;
        painter.drawPixmap(0, 0, scaled, x, y, width(), height());
    } else {
        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0.0, QColor(90, 145, 190));
        gradient.setColorAt(1.0, QColor(225, 242, 255));
        painter.fillRect(rect(), gradient);
    }

    QWidget::paintEvent(event);
}
#include "PixelImageButton.h"

#include <QEnterEvent>
#include <QPainter>
#include <QDebug>
#include <QFileInfo>

PixelImageButton::PixelImageButton(const QString& text, QWidget* parent)
    : QPushButton(text, parent)
{
    setFlat(true);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);
    setStyleSheet("QPushButton { background: transparent; border: none; }");
}

void PixelImageButton::setButtonImages(const QString& normalPath, const QString& hoverPath, const QString& pressedPath)
{
    normalPixmap = QPixmap(normalPath);
    hoverPixmap = QPixmap(hoverPath);
    pressedPixmap = QPixmap(pressedPath);

    qDebug() << "[PixelImageButton] load normal:" << normalPath << QFileInfo::exists(normalPath) << normalPixmap.size() << normalPixmap.isNull();
    qDebug() << "[PixelImageButton] load hover:" << hoverPath << QFileInfo::exists(hoverPath) << hoverPixmap.size() << hoverPixmap.isNull();
    qDebug() << "[PixelImageButton] load pressed:" << pressedPath << QFileInfo::exists(pressedPath) << pressedPixmap.size() << pressedPixmap.isNull();

    if (normalPixmap.isNull()) {
        qWarning() << "[PixelImageButton] failed to load normal image:" << normalPath;
    }
    if (hoverPixmap.isNull()) {
        qWarning() << "[PixelImageButton] failed to load hover image:" << hoverPath;
    }
    if (pressedPixmap.isNull()) {
        qWarning() << "[PixelImageButton] failed to load pressed image:" << pressedPath;
    }

    update();
}

void PixelImageButton::enterEvent(QEnterEvent* event)
{
    QPushButton::enterEvent(event);
    hovered = true;
    update();
}

void PixelImageButton::leaveEvent(QEvent* event)
{
    QPushButton::leaveEvent(event);
    hovered = false;
    update();
}

void PixelImageButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPixmap current = normalPixmap;
    if (isDown() && !pressedPixmap.isNull()) {
        current = pressedPixmap;
    } else if (hovered && !hoverPixmap.isNull()) {
        current = hoverPixmap;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

    if (!current.isNull()) {
        if (!isEnabled()) {
            painter.setOpacity(0.65);
        }
        painter.drawPixmap(rect(), current);
        if (!isEnabled()) {
            painter.setOpacity(1.0);
        }
    } else {
        qWarning() << "[PixelImageButton] current pixmap is null for" << text();
    }

    QFont f = font();
    f.setPointSize(16);
    f.setBold(false);
    painter.setFont(f);

    QRect textRect = rect();
    if (isDown()) {
        textRect = textRect.translated(0, 1);
    }

    QColor mainColor(22, 82, 140);
    QColor glowColor(255, 255, 255, 180);

    if (!isEnabled()) {
        mainColor = QColor(95, 125, 150);
        glowColor = QColor(255, 255, 255, 120);
    } else if (hovered) {
        mainColor = QColor(12, 70, 125);
    }

    QRect highlightRect = textRect.adjusted(-1, -1, -1, -1);
    painter.setPen(glowColor);
    painter.drawText(highlightRect, Qt::AlignCenter, text());

    painter.setPen(mainColor);
    painter.drawText(textRect, Qt::AlignCenter, text());
}
#include "ClickEffectWidget.h"

#include <QFileInfo>
#include <QPixmap>
#include <QTimer>
#include <QDebug>

ClickEffectWidget::ClickEffectWidget(const QString& stripPath, const QPoint& centerPos, QWidget* parent)
    : QLabel(parent), timer(new QTimer(this)) {
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet("QLabel{background: transparent; border: none;}");

    if (!QFileInfo::exists(stripPath)) {
        qWarning() << "[ClickEffect] missing strip:" << stripPath;
        deleteLater();
        return;
    }

    strip = QPixmap(stripPath);
    if (strip.isNull()) {
        qWarning() << "[ClickEffect] failed to load strip:" << stripPath;
        deleteLater();
        return;
    }

    frameCount = 6;
    const int frameW = strip.width() / frameCount;
    const int frameH = strip.height();
    if (frameW <= 0 || frameH <= 0) {
        qWarning() << "[ClickEffect] invalid strip size:" << stripPath;
        deleteLater();
        return;
    }

    frameSize = QSize(frameW, frameH);
    setGeometry(centerPos.x() - frameW / 2, centerPos.y() - frameH / 2, frameW, frameH);

    connect(timer, &QTimer::timeout, this, &ClickEffectWidget::advanceFrame);
    timer->setInterval(30);
    renderFrame();
    show();
    timer->start();
}

void ClickEffectWidget::advanceFrame() {
    ++frameIndex;
    if (frameIndex >= frameCount) {
        timer->stop();
        deleteLater();
        return;
    }
    renderFrame();
}

void ClickEffectWidget::renderFrame() {
    if (strip.isNull() || frameCount <= 0) return;
    const int frameW = strip.width() / frameCount;
    const int frameH = strip.height();
    QPixmap frame = strip.copy(frameIndex * frameW, 0, frameW, frameH);
    setPixmap(frame);
}
#include "SpriteAnimator.h"

#include <QLabel>
#include <QTimer>
#include <QFileInfo>
#include <QDebug>

SpriteAnimator::SpriteAnimator(QLabel* target, QObject* parent)
    : QObject(parent), targetLabel(target), timer(new QTimer(this)), scaled(96, 128) {
    connect(timer, &QTimer::timeout, this, &SpriteAnimator::advanceFrame);
}

bool SpriteAnimator::loadStrip(const QString& path, int frameCount) {
    if (frameCount <= 0 || !targetLabel) return false;
    if (!QFileInfo::exists(path)) {
        qWarning() << "[SpriteAnimator] missing strip:" << path;
        targetLabel->clear();
        targetLabel->hide();
        return false;
    }
    QPixmap px(path);
    if (px.isNull()) {
        qWarning() << "[SpriteAnimator] failed to load strip:" << path;
        targetLabel->clear();
        targetLabel->hide();
        return false;
    }
    if (!px.hasAlphaChannel()) qWarning() << "[Animation] sprite may not have real alpha" << path;
    strip = px;
    frames = frameCount;
    current = 0;
    targetLabel->show();
    renderCurrentFrame();
    return true;
}

void SpriteAnimator::setInterval(int ms) { timer->setInterval(ms); }
void SpriteAnimator::setScaledSize(const QSize& size) { scaled = size; renderCurrentFrame(); }
void SpriteAnimator::start() { if (!strip.isNull() && frames > 0) timer->start(); }
void SpriteAnimator::stop() { timer->stop(); }

void SpriteAnimator::advanceFrame() {
    if (strip.isNull() || frames <= 0) return;
    current = (current + 1) % frames;
    renderCurrentFrame();
}

void SpriteAnimator::renderCurrentFrame() {
    if (!targetLabel || strip.isNull() || frames <= 0) return;
    const int fw = strip.width() / frames;
    const int fh = strip.height();
    if (fw <= 0 || fh <= 0) return;
    QPixmap frame = strip.copy(current * fw, 0, fw, fh);
    targetLabel->setPixmap(frame.scaled(scaled, Qt::KeepAspectRatio, Qt::FastTransformation));
}
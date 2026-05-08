#include "HintBadge.h"

#include <QPainter>
#include <cmath>

HintBadge::HintBadge(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet("background: transparent;");
}

void HintBadge::setBadgePixmaps(const QPixmap& panel, const QPixmap& icon)
{
    panelPixmap = panel;
    iconPixmap = icon;
    update();
}

void HintBadge::setPhaseOffset(qreal offset) { phaseOffset = offset; }
void HintBadge::setPhase(qreal p) { phase = p; update(); }

void HintBadge::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

    const qreal t = phase + phaseOffset;
    const qreal scaleX = 0.86 + 0.14 * (0.5 + 0.5 * std::sin(t));

    painter.translate(width() / 2.0, height() / 2.0);
    painter.scale(scaleX, 1.0);

    QRect panelRect(-width() / 2, -height() / 2, width(), height());
    if (!panelPixmap.isNull()) {
        painter.drawPixmap(panelRect, panelPixmap);
    }

    if (!iconPixmap.isNull()) {
        const QSize iconSize(26, 26);
        QRect iconRect(-iconSize.width() / 2, -iconSize.height() / 2, iconSize.width(), iconSize.height());
        painter.drawPixmap(iconRect, iconPixmap.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
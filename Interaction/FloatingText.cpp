#include "FloatingText.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>

FloatingText::FloatingText(QWidget* parent) : QLabel(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAlignment(Qt::AlignCenter);
}

void FloatingText::showText(QWidget* parent, const QString& text, const QPoint& centerPos, const QString& type) {
    if (!parent) return;
    auto* w = new FloatingText(parent);
    w->setText(text);

    QString style = "QLabel{color:#EAF8FF;background:rgba(10,35,60,170);border-radius:8px;padding:6px 12px;font-size:16px;font-weight:700;}";
    if (type == "success") style = "QLabel{color:#EFFFFF;background:rgba(20,80,110,180);border-radius:8px;padding:6px 12px;font-size:16px;font-weight:700;}";
    else if (type == "error") style = "QLabel{color:#FFECEC;background:rgba(100,30,45,180);border-radius:8px;padding:6px 12px;font-size:16px;font-weight:700;}";
    else if (type == "coin") style = "QLabel{color:#FFF2A8;background:rgba(80,60,20,170);border-radius:8px;padding:6px 12px;font-size:16px;font-weight:700;}";
    w->setStyleSheet(style);
    w->adjustSize();

    QPoint start(centerPos.x() - w->width()/2, centerPos.y() - w->height()/2);
    start.setX(qMax(8, qMin(start.x(), parent->width() - w->width() - 8)));
    start.setY(qMax(8, qMin(start.y(), parent->height() - w->height() - 8)));
    QPoint end(start.x(), start.y() - 32);
    w->move(start);
    w->show();

    auto* opacity = new QGraphicsOpacityEffect(w);
    opacity->setOpacity(1.0);
    w->setGraphicsEffect(opacity);

    auto* posAnim = new QPropertyAnimation(w, "pos", w);
    posAnim->setDuration(1100);
    posAnim->setStartValue(start);
    posAnim->setEndValue(end);
    posAnim->setEasingCurve(QEasingCurve::OutCubic);

    auto* opaAnim = new QPropertyAnimation(opacity, "opacity", w);
    opaAnim->setDuration(1100);
    opaAnim->setStartValue(1.0);
    opaAnim->setEndValue(0.0);
    opaAnim->setEasingCurve(QEasingCurve::InOutSine);

    auto* group = new QParallelAnimationGroup(w);
    group->addAnimation(posAnim);
    group->addAnimation(opaAnim);
    QObject::connect(group, &QParallelAnimationGroup::finished, w, &FloatingText::deleteLater);
    group->start(QAbstractAnimation::DeleteWhenStopped);
}
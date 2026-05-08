#ifndef HINTBADGE_H
#define HINTBADGE_H

#include <QWidget>
#include <QPixmap>

class HintBadge : public QWidget
{
    Q_OBJECT
public:
    explicit HintBadge(QWidget* parent = nullptr);
    void setBadgePixmaps(const QPixmap& panel, const QPixmap& icon);
    void setPhaseOffset(qreal offset);
    void setPhase(qreal phase);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap panelPixmap;
    QPixmap iconPixmap;
    qreal phaseOffset = 0.0;
    qreal phase = 0.0;
};

#endif
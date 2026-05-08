#ifndef CLICKEFFECTWIDGET_H
#define CLICKEFFECTWIDGET_H

#include <QLabel>

class QTimer;

class ClickEffectWidget : public QLabel {
    Q_OBJECT
public:
    explicit ClickEffectWidget(const QString& stripPath, const QPoint& centerPos, QWidget* parent = nullptr);

private slots:
    void advanceFrame();

private:
    void renderFrame();
    QPixmap strip;
    QTimer* timer = nullptr;
    int frameCount = 0;
    int frameIndex = 0;
    QSize frameSize;
};

#endif
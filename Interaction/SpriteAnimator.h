#ifndef SPRITEANIMATOR_H
#define SPRITEANIMATOR_H

#include <QObject>
#include <QPixmap>
#include <QSize>

class QLabel;
class QTimer;

class SpriteAnimator : public QObject {
    Q_OBJECT
public:
    explicit SpriteAnimator(QLabel* target, QObject* parent = nullptr);
    bool loadStrip(const QString& path, int frameCount);
    void setInterval(int ms);
    void setScaledSize(const QSize& size);
    void start();
    void stop();

private slots:
    void advanceFrame();

private:
    void renderCurrentFrame();
    QLabel* targetLabel = nullptr;
    QTimer* timer = nullptr;
    QPixmap strip;
    int frames = 0;
    int current = 0;
    QSize scaled;
};

#endif
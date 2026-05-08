#ifndef INTROPAGE_H
#define INTROPAGE_H

#include <QWidget>

class QLabel;
class QTimer;
class QGraphicsOpacityEffect;
class QPropertyAnimation;
class QSoundEffect;

class IntroPage : public QWidget {
    Q_OBJECT
public:
    explicit IntroPage(QWidget* parent = nullptr);

    signals:
        void introFinished();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void startTyping();
    void updateTypedText();
    void finishIntro(bool skipped = false);
    void setupKeyTapSound();
    void beginIntroSequence();

    QLabel* textLabel = nullptr;
    QTimer* cursorTimer = nullptr;
    QTimer* typingTimer = nullptr;
    QGraphicsOpacityEffect* opacityEffect = nullptr;
    QPropertyAnimation* fadeAnim = nullptr;
    QPropertyAnimation* fadeInAnim = nullptr;
    QSoundEffect* keyTapSound = nullptr;

    QString fullText = "made by Peregine.";
    int currentIndex = 0;
    bool cursorOn = true;
    bool finishing = false;
    bool startedTyping = false;
};

#endif
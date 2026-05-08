#include "IntroPage.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSoundEffect>
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>

#include "../config/AssetPaths.h"

IntroPage::IntroPage(QWidget* parent): QWidget(parent) {
    setStyleSheet("QWidget{background-color: rgb(12, 14, 18);} QLabel{color:#f2f2f2; font-size:24px; font-weight:600;}");
    QVBoxLayout* l = new QVBoxLayout(this);
    textLabel = new QLabel("|", this);
    textLabel->setAlignment(Qt::AlignCenter);
    l->addWidget(textLabel);

    cursorTimer = new QTimer(this);
    cursorTimer->setInterval(420);
    connect(cursorTimer,&QTimer::timeout,this,[this](){ cursorOn=!cursorOn; updateTypedText(); });

    typingTimer = new QTimer(this);
    typingTimer->setInterval(150);
    connect(typingTimer,&QTimer::timeout,this,[this](){
        if (finishing) return;
        if (currentIndex < fullText.size()) {
            ++currentIndex;
            updateTypedText();
            if (keyTapSound) keyTapSound->play();
        } else {
            typingTimer->stop();
            QTimer::singleShot(2200, this, [this](){ finishIntro(false); });
        }
    });

    opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(0.0);
    setGraphicsEffect(opacityEffect);

    fadeInAnim = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeInAnim->setDuration(1800);
    fadeInAnim->setEasingCurve(QEasingCurve::InOutCubic);
    fadeInAnim->setStartValue(0.0);
    fadeInAnim->setEndValue(1.0);
    connect(fadeInAnim, &QPropertyAnimation::finished, this, &IntroPage::beginIntroSequence);

    fadeAnim = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeAnim->setDuration(1800);
    fadeAnim->setEasingCurve(QEasingCurve::InOutCubic);
    fadeAnim->setStartValue(1.0);
    fadeAnim->setEndValue(0.0);
    connect(fadeAnim,&QPropertyAnimation::finished,this,[this](){ if(!finishing) return; emit introFinished(); });

    setupKeyTapSound();
    fadeInAnim->start();
}

void IntroPage::beginIntroSequence(){
    if (finishing || startedTyping) return;
    QTimer::singleShot(1000, this, [this](){
        if (finishing) return;
        cursorTimer->start();
        QTimer::singleShot(900, this, &IntroPage::startTyping);
    });
}

void IntroPage::setupKeyTapSound(){
    keyTapSound = new QSoundEffect(this);
    keyTapSound->setVolume(0.30f);
    const QString wavPath = QDir(AssetPaths::assetRoot()).filePath("audio/sfx_key_tap.wav");
    const QString mp3Path = QDir(AssetPaths::assetRoot()).filePath("audio/sfx_key_tap.mp3");
    if (QFileInfo::exists(wavPath)) {
        keyTapSound->setSource(QUrl::fromLocalFile(wavPath));
        return;
    }
    if (QFileInfo::exists(mp3Path)) {
        keyTapSound->setSource(QUrl::fromLocalFile(mp3Path));
        return;
    }
    qWarning() << "[IntroPage] Missing key tap sound";
    keyTapSound = nullptr;
}

void IntroPage::startTyping(){ if(finishing || startedTyping) return; startedTyping = true; typingTimer->start(); }

void IntroPage::updateTypedText(){
    const QString typed = fullText.left(currentIndex);
    textLabel->setText(typed + (cursorOn ? "|" : " "));
}

void IntroPage::finishIntro(bool skipped){
    if (finishing) return;
    finishing = true;
    typingTimer->stop();
    cursorTimer->stop();
    if (fadeInAnim) fadeInAnim->stop();
    fadeAnim->stop();
    fadeAnim->setDuration(skipped ? 600 : 1800);
    fadeAnim->setEasingCurve(QEasingCurve::InOutCubic);
    fadeAnim->start();
}

void IntroPage::keyPressEvent(QKeyEvent* event){ Q_UNUSED(event); finishIntro(true); }
void IntroPage::mousePressEvent(QMouseEvent* event){ Q_UNUSED(event); finishIntro(true); }
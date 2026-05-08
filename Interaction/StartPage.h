#ifndef STARTPAGE_H
#define STARTPAGE_H

#include <QWidget>
#include <QPixmap>

class QLabel;
class QFrame;
class PixelImageButton;

class StartPage : public QWidget
{
    Q_OBJECT

public:
    explicit StartPage(QWidget* parent = nullptr);

    signals:
        void startGameRequested();
    void continueGameRequested();
    void clickSfxRequested();
public:
    void refreshContinueButtonState(bool hasSave);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QPixmap backgroundPixmap;
    QPixmap logoPixmap;

    QLabel* logoLabel;
    QFrame* menuPanel;
    PixelImageButton* startButton;
    PixelImageButton* continueButton;
    PixelImageButton* quitButton;

    void updateLayoutBySize();
};

#endif // STARTPAGE_H
#ifndef PIXELIMAGEBUTTON_H
#define PIXELIMAGEBUTTON_H

#include <QPushButton>
#include <QPixmap>

class PixelImageButton : public QPushButton
{
    Q_OBJECT

public:
    explicit PixelImageButton(const QString& text, QWidget* parent = nullptr);

    void setButtonImages(const QString& normalPath, const QString& hoverPath, const QString& pressedPath);

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap normalPixmap;
    QPixmap hoverPixmap;
    QPixmap pressedPixmap;
    bool hovered = false;
};

#endif // PIXELIMAGEBUTTON_H
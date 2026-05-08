#ifndef FLOATINGTEXT_H
#define FLOATINGTEXT_H

#include <QLabel>

class FloatingText : public QLabel {
    Q_OBJECT
public:
    explicit FloatingText(QWidget* parent = nullptr);
    static void showText(QWidget* parent, const QString& text, const QPoint& centerPos, const QString& type = "normal");
};

#endif
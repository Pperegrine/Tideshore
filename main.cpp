#include <QApplication>
#include <QFontDatabase>
#include <QFont>
#include <QDebug>

#include <QIcon>
#include <QDir>
#include <QFileInfo>
#include "config/AssetPaths.h"

#include "interaction/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString iconPath = QDir(AssetPaths::assetRoot()).filePath("app_icon/tideshore.ico");

    if (QFileInfo::exists(iconPath)) {
        QApplication::setWindowIcon(QIcon(iconPath));
    } else {
        qWarning() << "[AppIcon] Missing icon:" << iconPath;
    }
    AssetPaths::debugPrintMissingAssets();

    int fontId = QFontDatabase::addApplicationFont(AssetPaths::zpixFont());
    if (fontId >= 0) {
        const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty()) {
            QApplication::setFont(QFont(families.first(), 10));
            qDebug() << "[Assets] Loaded font:" << families.first();
        }
    } else {
        qWarning() << "[Assets] Failed to load font:" << AssetPaths::zpixFont();
    }

    MainWindow w;
    // w.showNormal(); // debug mode
    w.showFullScreen(); // intro cinematic mode

    return app.exec();
}
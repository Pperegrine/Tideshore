#include "SaveSystem.h"

#include "GameManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

static constexpr int kSaveVersion = 1;

QString SaveSystem::saveDirPath() {
    return QDir(QCoreApplication::applicationDirPath()).filePath("saves");
}

QString SaveSystem::saveFilePath() {
    return QDir(saveDirPath()).filePath("save_001.json");
}

bool SaveSystem::hasSave() {
    return QFile::exists(saveFilePath());
}

bool SaveSystem::saveGame(const GameManager& gameManager) {
    QDir dir(saveDirPath());
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "[SaveSystem] Save failed: cannot create save dir" << dir.path();
        return false;
    }

    QJsonObject root;
    root["version"] = kSaveVersion;
    root["day"] = gameManager.store.currentDay;
    root["money"] = gameManager.store.money;
    root["cleanliness"] = gameManager.store.cleanliness;
    root["storeLevel"] = gameManager.store.level;
    root["storageLevel"] = gameManager.store.storageLevel;
    root["analysisLevel"] = gameManager.store.analysisLevel;
    root["dailyGoodsUnlocked"] = gameManager.store.dailyGoodsUnlocked;

    QJsonArray products;
    for (int i = 0; i < gameManager.store.products.size(); ++i) {
        const Product& p = gameManager.store.products[i];
        QJsonObject obj;
        obj["id"] = i;
        obj["name"] = p.name;
        obj["stock"] = p.stock;
        obj["playerPrice"] = p.playerPrice;
        products.append(obj);
    }
    root["products"] = products;

    QJsonArray employees;
    for (int i = 0; i < gameManager.store.employees.size(); ++i) {
        const Employee& e = gameManager.store.employees[i];
        QJsonObject obj;
        obj["id"] = i;
        obj["name"] = e.name;
        obj["hired"] = e.hired;
        employees.append(obj);
    }
    root["employees"] = employees;

    QFile file(saveFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "[SaveSystem] Save failed:" << file.errorString();
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();
    qDebug() << "[SaveSystem] Saved game to:" << file.fileName();
    return true;
}

bool SaveSystem::loadGame(GameManager& gameManager) {
    QFile file(saveFilePath());
    if (!file.exists()) {
        qWarning() << "[SaveSystem] Load failed: save file not found" << file.fileName();
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[SaveSystem] Load failed:" << file.errorString();
        return false;
    }

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    file.close();
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "[SaveSystem] Load failed: invalid JSON" << err.errorString();
        return false;
    }

    const QJsonObject root = doc.object();
    const int version = root.contains("version") ? root.value("version").toInt(-1) : -1;
    if (version < 0) qWarning() << "[SaveSystem] Load warning: missing version";
    if (version > kSaveVersion) {
        qWarning() << "[SaveSystem] Load failed: unsupported save version" << version;
        return false;
    }

    Store loaded = gameManager.store;
    loaded.currentDay = root.value("day").toInt(loaded.currentDay);
    loaded.money = root.value("money").toInt(loaded.money);
    loaded.cleanliness = root.value("cleanliness").toInt(loaded.cleanliness);
    loaded.level = root.value("storeLevel").toInt(loaded.level);
    loaded.storageLevel = root.value("storageLevel").toInt(loaded.storageLevel);
    loaded.analysisLevel = root.value("analysisLevel").toInt(loaded.analysisLevel);
    loaded.dailyGoodsUnlocked = root.value("dailyGoodsUnlocked").toBool(loaded.dailyGoodsUnlocked);

    const QJsonArray products = root.value("products").toArray();
    for (const QJsonValue& v : products) {
        const QJsonObject obj = v.toObject();
        const int id = obj.value("id").toInt(-1);
        if (id < 0 || id >= loaded.products.size()) { qWarning() << "[SaveSystem] product id out of range:" << id; continue; }
        loaded.products[id].stock = obj.value("stock").toInt(loaded.products[id].stock);
        loaded.products[id].playerPrice = obj.value("playerPrice").toInt(loaded.products[id].playerPrice);
    }

    const QJsonArray employees = root.value("employees").toArray();
    for (const QJsonValue& v : employees) {
        const QJsonObject obj = v.toObject();
        const int id = obj.value("id").toInt(-1);
        if (id < 0 || id >= loaded.employees.size()) { qWarning() << "[SaveSystem] employee id out of range:" << id; continue; }
        loaded.employees[id].hired = obj.value("hired").toBool(loaded.employees[id].hired);
    }

    gameManager.applyLoadedStore(loaded);
    qDebug() << "[SaveSystem] Loaded game from:" << file.fileName();
    return true;
}
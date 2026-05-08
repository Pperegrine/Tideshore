#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

#include <QString>

class GameManager;

class SaveSystem {
public:
    static QString saveDirPath();
    static QString saveFilePath();
    static bool hasSave();
    static bool saveGame(const GameManager& gameManager);
    static bool loadGame(GameManager& gameManager);
};

#endif
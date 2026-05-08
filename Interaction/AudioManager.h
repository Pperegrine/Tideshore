#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
class QMediaPlayer;
class QAudioOutput;
class QSoundEffect;

class AudioManager : public QObject {
    Q_OBJECT
public:
    explicit AudioManager(QObject* parent = nullptr);
    void playBgm(const QString& path);
    void stopBgm();
    void setBgmEnabled(bool enabled);
    bool isBgmEnabled() const;
    void setVolume(float volume);
    float volume() const;

    void playClickSfx();
    void playConfirmSfx();
    void playErrorSfx();
    void playCoinSfx();

    void setSfxVolume(float volume);
    float sfxVolume() const;

private:
    bool loadSfxWithFallback(QSoundEffect* effect, const QString& wavRelativePath, const QString& mp3RelativePath, const char* tag);
    void playSfx(QSoundEffect* effect, const char* tag);

    QMediaPlayer* player = nullptr;
    QAudioOutput* output = nullptr;
    bool enabled = true;

    QSoundEffect* clickSfx = nullptr;
    QSoundEffect* confirmSfx = nullptr;
    QSoundEffect* errorSfx = nullptr;
    QSoundEffect* coinSfx = nullptr;
    float sfxVolumeValue = 0.45f;
};

#endif
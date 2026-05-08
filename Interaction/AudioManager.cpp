#include "AudioManager.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>
#include <QDir>

#include "../config/AssetPaths.h"

AudioManager::AudioManager(QObject* parent)
    : QObject(parent), player(new QMediaPlayer(this)), output(new QAudioOutput(this)) {
    player->setAudioOutput(output);
    output->setVolume(0.25f);
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    player->setLoops(QMediaPlayer::Infinite);
#else
    connect(player,&QMediaPlayer::mediaStatusChanged,this,[this](QMediaPlayer::MediaStatus s){ if(s==QMediaPlayer::EndOfMedia && enabled) player->play();});
#endif

    clickSfx = new QSoundEffect(this);
    confirmSfx = new QSoundEffect(this);
    errorSfx = new QSoundEffect(this);
    coinSfx = new QSoundEffect(this);
    setSfxVolume(sfxVolumeValue);

    loadSfxWithFallback(clickSfx, "audio/sfx_ui_click.wav", "audio/sfx_ui_click.mp3", "click");
    loadSfxWithFallback(confirmSfx, "audio/sfx_ui_confirm.wav", "audio/sfx_ui_confirm.mp3", "confirm");
    loadSfxWithFallback(errorSfx, "audio/sfx_ui_error.wav", "audio/sfx_ui_error.mp3", "error");
    loadSfxWithFallback(coinSfx, "audio/sfx_ui_coin.wav", "audio/sfx_ui_coin.mp3", "coin");
}

bool AudioManager::loadSfxWithFallback(QSoundEffect* effect, const QString& wavRelativePath, const QString& mp3RelativePath, const char* tag) {
    if (!effect) return false;

    const QDir root(AssetPaths::assetRoot());
    const QString wavPath = root.filePath(wavRelativePath);
    const QString mp3Path = root.filePath(mp3RelativePath);

    if (QFileInfo::exists(wavPath)) {
        effect->setSource(QUrl::fromLocalFile(wavPath));
        return true;
    }

    if (QFileInfo::exists(mp3Path)) {
        effect->setSource(QUrl::fromLocalFile(mp3Path));
        qWarning() << "[AudioManager] wav missing, fallback to mp3 for" << tag << ":" << mp3Path;
        return true;
    }

    qWarning() << "[AudioManager] Missing sfx files for" << tag << ":" << wavPath << "and" << mp3Path;
    return false;
}

void AudioManager::playSfx(QSoundEffect* effect, const char* tag) {
    if (!effect) return;
    if (effect->source().isEmpty()) return;
    if (effect->status() == QSoundEffect::Error) {
        qWarning() << "[AudioManager] SFX error status:" << tag << effect->source().toString();
        return;
    }
    effect->play();
}

void AudioManager::playBgm(const QString& path){
    if(!QFileInfo::exists(path)){ qWarning() << "[AudioManager] missing bgm:" << path; return; }
    player->setSource(QUrl::fromLocalFile(path));
    if(enabled) player->play();
}
void AudioManager::stopBgm(){ player->stop(); }
void AudioManager::setBgmEnabled(bool e){ enabled=e; if(enabled) player->play(); else player->pause(); }
bool AudioManager::isBgmEnabled() const{ return enabled; }
void AudioManager::setVolume(float v){ output->setVolume(qBound(0.0f,v,1.0f)); }
float AudioManager::volume() const{ return output->volume(); }

void AudioManager::playClickSfx() { playSfx(clickSfx, "click"); }
void AudioManager::playConfirmSfx() { playSfx(confirmSfx, "confirm"); }
void AudioManager::playErrorSfx() { playSfx(errorSfx, "error"); }
void AudioManager::playCoinSfx() { playSfx(coinSfx, "coin"); }

void AudioManager::setSfxVolume(float volume) {
    sfxVolumeValue = qBound(0.0f, volume, 1.0f);
    for (QSoundEffect* e : {clickSfx, confirmSfx, errorSfx, coinSfx}) {
        if (e) e->setVolume(sfxVolumeValue);
    }
}

float AudioManager::sfxVolume() const { return sfxVolumeValue; }
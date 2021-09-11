#include "SoundManager.h"

#include "GameConfig.h"

#include <soloud_error.h>

constexpr const char* c_backgroundTrackFile = "..\\Data\\Sounds\\Sun Tribe.ogg";
constexpr const char* c_completeTrackFile = "..\\Data\\Sounds\\complete.ogg";

SoundManager::SoundManager()
{
    m_soloudEngine.init();
    SoLoud::result result;

    result = m_backgroundTrack.load(c_backgroundTrackFile);
    if (result != SoLoud::SO_NO_ERROR)
    {
        LOGI("Failed to load sound file %s with error code %d", c_backgroundTrackFile, result);
    }
    
    m_backgroundTrack.setVolume(0.1f);
    m_backgroundTrack.setLooping(true);
    m_backgroundTrack.setSingleInstance(true);

    result = m_completeTrack.load(c_completeTrackFile);
    if (result != SoLoud::SO_NO_ERROR)
    {
        LOGI("Failed to load sound file %s with error code %d", c_completeTrackFile, result);
    }

    m_completeTrack.setVolume(0.2f);
}

SoundManager::~SoundManager()
{
    m_soloudEngine.deinit();
}

void SoundManager::PlayBackgroundSound()
{
    m_soloudEngine.play(m_backgroundTrack);
}

void SoundManager::PlayCompleteSound()
{
    m_soloudEngine.play(m_completeTrack);
}

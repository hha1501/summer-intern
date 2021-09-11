#pragma once

#include <soloud.h>
#include <soloud_wav.h>

class SoundManager
{
public:
    SoundManager();
    ~SoundManager();

    void PlayBackgroundSound();

    void PlayCompleteSound();

private:
    SoLoud::Soloud m_soloudEngine;

    SoLoud::Wav m_backgroundTrack;
    SoLoud::Wav m_completeTrack;
};

/*
 *  AudioSystem.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <SDL/SDL.h>
#include <SDL_mixer/SDL_mixer.h>
#include <Base/Exception.h>

#include "AudioSystem.h"

AudioSystem::AudioSystem(): _music(NULL) {
    // Set 2 channel, 16-bit stereo audio at 22Khz
    if (Mix_OpenAudio(22050, AUDIO_S16, 2, 4096)) {
        THROW(InternalError, "Unable to initialize SDL_audio: " << Mix_GetError());
    }

    Uint16 audio_format;
    int audio_rate, audio_channels;
    Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
    Info("Audio system succesfully setup.");
    LogStream::IncrementIndent();
    Info("Bit rate: " << audio_rate << "Hz");
    Info("Channels: " << audio_channels);
    Info("Format:   " << audio_format);
    LogStream::DecrementIndent();
}

AudioSystem::~AudioSystem() {
    Mix_CloseAudio();
}

void AudioSystem::haltMusic() {
    Mix_HaltMusic();
    Mix_FreeMusic(_music);
    _music = NULL;
}

void AudioSystem::playMusic(const std::string &file, int loopCount) {
    if (_music) { haltMusic(); }
    if (!(_music = Mix_LoadMUS(file.c_str()))) {
        THROW(InternalError, "Could not load audio file '" << file << "': " << Mix_GetError());
    }

    if (Mix_PlayMusic(_music, loopCount)) {
        THROW(InternalError, "Could not start music: " << Mix_GetError());
    }
}

/*
 *  AudioSystem.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _AUDIOSYSTEM_H_
#define _AUDIOSYSTEM_H_
#include <string>

class _Mix_Music;

/*! \note This is just a temporary version until I get my hands on my arch book. It will
 *  eventually be changed to use the resource group manager and whatnot. */
class AudioSystem {
public:
    AudioSystem();
    virtual ~AudioSystem();

    /* \param loopCount The number of times to loop. -1 for infinite, 0 for just once. */
    void playMusic(const std::string &file, int loopCount = -1);

    void haltMusic();

protected:
    _Mix_Music *_music;

};

#endif

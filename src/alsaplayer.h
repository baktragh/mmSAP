//
// File:   alsaplayer.h
// Author: michael
//
// Created on 22. únor 2008, 17:13
//

#ifndef _ALSAPLAYER_H
#define	_ALSAPLAYER_H

#include "platform.h"
#include "asap.h"
#include "preferences.h"
#include "gui.h"
#include <gtkmm.h>
#include <alsa/asoundlib.h>
#include <stdio.h>

/*Player state*/
#define PLAYER_EMPTY 0
#define PLAYER_STOPPED 1
#define PLAYER_PLAYING 2
#define PLAYER_PAUSED 3

/*Load and play results*/
#define PLAYER_LP_OK 0
#define PLAYER_LP_LOAD_ERROR -1
#define PLAYER_LP_ALSA_ERROR -2


class AlsaPlayer {

public:
    

    /*C-tor*/
    AlsaPlayer(Preferences* pf,GUI* gi);
    ~AlsaPlayer();

    /*Update configuration*/
    void updateConfig();

    /*Load file*/
    int load(Glib::ustring filespec);
    /*Play or Pause*/
    int playOrPause(bool afterLoad=false);
    /*Stop*/
    void stop();
    /*Next subsong*/
    void next();
    /*Previous subsong*/
    void previous();
    /*Set subsong*/
    void setSubSong(int i);

    /*Seek request*/
    void setSeekRequest(double d);

    /*Return last error message*/
    Glib::ustring getLastError();

    
    void destroy();

    /*Second tick & slider update*/
    bool tick();

    /*Mute channels*/
    void setChannelsMute(int mute);
    void setExternalEventRequest();

    /*Empty*/
    bool isEmpty();
    Glib::ustring getCurrentFilespec();

private:

    /*Links to other program parts*/
    Preferences* prefs;
    GUI* gui;
    
    
    /*Player state*/
    volatile int playerState;

    /*Buffer*/
    unsigned char* buffer/*[8192]*/;

    /*SAP Data buffer*/
    unsigned char* sapBuffer;

    /*ASAP state*/
    ASAP *state;
    /*ASAP module info*/
    const ASAPInfo *modInfo;

    /*Threads*/
    Glib::Thread* playbackThread;

    /*Sound card related*/
    snd_pcm_t *playback_handle;
    
    /*Channels and songs*/
    int currentChannels;
    int currentSong;
    int currentSongCount;
    int currentLength;

    /*Mute mask*/
    int muteMask;
    
    /*Error message*/
    Glib::ustring errorMessage;

    /*Flags*/
    bool cancelRepeatFlag;

    /*Seek request*/
    volatile int seekToMiliseconds;

    /*Current file specifier*/
    Glib::ustring currentFilespec;
    
    /*Repeat handler (after play loop terminates*/
    bool handleRepeat();
    /*Isolate file name from file specifier*/
    Glib::ustring getFilename(Glib::ustring fspec);
    
    /*Play first subsong*/
    void first();
    
    /*Get length of a subsong*/
    int getSubsongLength(int index);
    
    /*Make ASAP change subsong*/
    void changeSubsong();
    
    /*Perform seeking*/
    void seek(int tgtMiliseconds);

    /*Allocate buffer for frames provided by ASAP*/
    bool allocateBuffer();

    /*Visualization related*/
    void startVisualization();
    void stopVisualization(bool ultimately=false);
    bool visualizationTick(int ti);
    sigc::slot<bool> visualizationSlot;
    sigc::connection visualizationConnection;
    
    /*Visualization data*/
    int  visualizationPool[20][8];
    int  visualizationVolumes[8];

    /*Visualization read and write indices*/
    int visualizationWriteIndex;
    int visualizationReadIndex;

    /*Visualization decrement*/
    int visualizationDecrement;
    
    /*Buffer size*/
    volatile int bufferSize;

    /*Normalize SAP header (clear metadata)*/
    int normalizeSAP(unsigned char** _buf,int buflen);
    
    /*Open sound card*/
    int openSoundCard(int channels);
    /*Close sound card*/
    int closeSoundCard();
    
    /*Playing loop*/
    void playLoopNormal();

};




#endif	/* _ALSAPLAYER_H */


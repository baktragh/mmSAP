
#ifndef _ALSAPLAYER_H
#define _ALSAPLAYER_H

#include "platform.h"
#include <asap.h>
#include "preferences.h"
#include "gui.h"
#include <gtkmm.h>
#include <stdio.h>
#define NO_STDIO_REDIRECT
#include <SDL2/SDL.h>

/*Player state*/
#define PLAYER_EMPTY 0
#define PLAYER_STOPPED 1
#define PLAYER_PLAYING 2
#define PLAYER_PAUSED 3

/*Load and play results*/
#define PLAYER_LP_OK 0
#define PLAYER_LP_LOAD_ERROR -1
#define PLAYER_LP_ALSA_ERROR -2

void playLoopNormal(void *udata, Uint8 *stream, int len);

class AlsaPlayer {
public:
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

    /*Error handling*/
    Glib::ustring errorMessage;

    /*Channels and songs*/
    int currentChannels;
    int currentSong;
    int currentSongCount;
    bool currentFakeStereo;
    int currentLength;
    /*Seek request*/
    volatile int seekToMiliseconds;
    volatile bool cancelRepeatFlag;
    volatile int lastPosition;

    /*Mute mask*/
    int muteMask;

    /*C-tor*/
    AlsaPlayer(Preferences* pf, GUI* gi);
    ~AlsaPlayer();

    /*Update configuration*/
    void updateConfig();

    /*Load file*/
    int load(Glib::ustring filespec);
    /*Play or Pause*/
    int playOrPause(bool afterLoad = false);
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


    /*Open sound card*/
    int openSoundCard(int channels);
    /*Close sound card*/
    int closeSoundCard();

    /*Return last error message*/
    Glib::ustring getLastError();

    void destroy();
    void seek(int tgtMiliseconds);

    /*Mute channels*/
    void setChannelsMute(int mute);
    void setExternalEventRequest();

    /*Empty*/
    bool isEmpty();
    Glib::ustring getCurrentFilespec();
    bool handleRepeat();
    bool tick();

private:

    /*Links to other program parts*/
    Preferences* prefs;
    GUI* gui;

    /*Flags*/
    bool blockSeekingUpdateFlag;
    Glib::ustring currentFilespec;

    Glib::ustring getFilename(Glib::ustring fspec);
    void first();
    int getSubsongLength(int index);
    void changeSubsong();


    bool allocateBuffer();

    /*Ticks*/
    sigc::slot<bool()> tickSlot;

    /*Visualization related*/
    void startVisualization();
    void stopVisualization(bool ultimately = false);
    bool visualizationTick(int ti);
    sigc::slot<bool()> visualizationSlot;
    sigc::connection visualizationConnection;
    sigc::connection tickConnection;

    /*Visualization data*/
    int visualizationPool[20][8];
    int visualizationVolumes[8];

    /*Visualization read and write indices*/
    int visualizationWriteIndex;
    int visualizationReadIndex;

    /*Visualization decrement*/
    int visualizationDecrement;

    volatile int bufferSize;


    /*Normalizing*/
    int normalizeSAP(unsigned char** _buf, int buflen);

};




#endif /* _ALSAPLAYER_H */


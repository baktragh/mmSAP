#include "alsaplayer.h"

extern void handlePlaylistContinue();


AlsaPlayer::AlsaPlayer(Preferences* pf,GUI* gi) {

    playerState=PLAYER_EMPTY;
    errorMessage="No error";
    sapBuffer=NULL;
    prefs=pf;
    gui=gi;
    cancelRepeatFlag=false;
    
    state = ASAP_New();
    modInfo = NULL;

    updateConfig();
    seekToMiliseconds=-1;

    buffer=NULL;
    bufferSize=0;

    visualizationReadIndex=0;
    visualizationWriteIndex=0;
    visualizationDecrement=prefs->getVisualizationDecrement();
    muteMask=0;

    memset(visualizationPool,0,sizeof(int)*160);

    currentChannels=1;
    currentFilespec="";
    currentLength=0;
    currentSong=0;
    currentSongCount=0;

}

AlsaPlayer::~AlsaPlayer() {

    if (state!=NULL) {
        ASAP_Delete(state);
    }
}

int AlsaPlayer::load(Glib::ustring filespec) {

    bool r=TRUE;
    gboolean b=FALSE;
    
    #ifdef DEBUG_PRINTOUTS
    printf("Loading |%s|\n",filespec.c_str());
    #endif

    currentFilespec=filespec;

    /*If something loaded*/
    if (playerState!=PLAYER_EMPTY) {
        /*And possibly some playing, stop it if necessary*/
         stop();
    }

    /*Nothing loaded*/
    playerState=PLAYER_EMPTY;

    /*Load data from file, first, determine length and check if
     it does not exceed maximum size*/
    int flen=0;

    /*Test whether file is regular file*/
    b=g_file_test(filespec.c_str(),(GFileTest)(G_FILE_TEST_IS_REGULAR));
    if (b==FALSE) {
        errorMessage="Attempt to load something that is not regular file";
        return PLAYER_LP_LOAD_ERROR;
    }
        

    FILE* f = fopen(filespec.c_str(),"rb");
    if (f==NULL) {
        errorMessage="Unable to open file for binary reading";
        return PLAYER_LP_LOAD_ERROR;
    }

    if (fseek(f,0,SEEK_END)<0 ) {
        errorMessage="Unable to seek() to the end of the file";
        return PLAYER_LP_LOAD_ERROR;
    }

    if ((flen=(int)ftell(f))<0) {
        errorMessage="Unable to ftell() to determine file size";
        return PLAYER_LP_LOAD_ERROR;
    }

    if (fseek(f,0,SEEK_SET)<0)  {
        errorMessage="Unable to seek() back to the beginning of the file";
        return PLAYER_LP_LOAD_ERROR;
    }
    
    if (flen>ASAPInfo_MAX_MODULE_LENGTH) {
        errorMessage="File is too large";
        return PLAYER_LP_LOAD_ERROR;
    }
    currentLength=flen;

    /*Allocate memory*/
    if (sapBuffer!=NULL) free(sapBuffer);
    sapBuffer=(unsigned char*)malloc(flen);
    if (sapBuffer==NULL) {
        errorMessage="Unable to allocate memory for tune";
        return PLAYER_LP_LOAD_ERROR;
    }

    int bc = fread(sapBuffer,1,flen,f);
    fclose(f);

    if (bc<flen) {
        errorMessage="Unable to read all data from file";
        return PLAYER_LP_LOAD_ERROR;
    }

    /*Try to load*/
    r=ASAP_Load(state,filespec.c_str(),sapBuffer,flen);

    /*If it fails,try to normalize or bail out*/
    if (r!=TRUE) {

        int nr=0;

        if (prefs->getNormalizeSAPHeader()==true) {

            /*Try to 'normalize' tune info part*/
            nr=normalizeSAP(&sapBuffer,flen);
            if (nr!=0) {
                r=ASAP_Load(state,filespec.c_str(),sapBuffer,nr);
            }

        }

        if (nr==0 || r!=TRUE) {
            errorMessage="ASAP library rejected the file. It has unknown or invalid format";
            return PLAYER_LP_LOAD_ERROR;
        }

    }

    /*Retrieve basic parameters*/
    modInfo = ASAP_GetInfo(state);

    if (prefs->getAlwaysFirstSubsong()==true) {
        currentSong=0;
    }
    else {
        currentSong=ASAPInfo_GetDefaultSong(modInfo);
    }

    currentSongCount=ASAPInfo_GetSongs(modInfo);
    currentChannels=ASAPInfo_GetChannels(modInfo);


    /*Set silence detection*/
    ASAP_DetectSilence(state,prefs->getSilenceLimit());

    /*New player state is stopped*/
    playerState=PLAYER_STOPPED;
    #ifdef DEBUG_PRINTOUTS
    printf("Loaded...\n");
    #endif

    /*Update GUI*/
    Glib::ustring infoString(ASAPInfo_GetTitleOrFilename(modInfo));

    if (strcmp(ASAPInfo_GetAuthor(modInfo),"")!=0) {
        infoString+='\n';
        infoString+=ASAPInfo_GetAuthor(modInfo);
    }

    if (strcmp(ASAPInfo_GetDate(modInfo),"")!=0) {
        infoString+='\n';
        infoString+=ASAPInfo_GetDate(modInfo);
    }
    
    Glib::ustring shortInfo=infoString;
    
    infoString+='\n';
    infoString+=currentFilespec;
    

    gui->updateAfterSAPLoad(getFilename(filespec),currentSong,currentSongCount,getSubsongLength(currentSong),shortInfo,infoString,prefs->getIgnoreTimeTag());
    gui->resetSeekScale();
    
    seekToMiliseconds=-1;

    /*Start playing*/
    if (playOrPause(true)<0) return PLAYER_LP_ALSA_ERROR;

    return PLAYER_LP_OK;

}

int AlsaPlayer::playOrPause(bool afterLoad) {
    
    #ifdef DEBUG_PRINTOUTS
    printf("playOrPause: BEGIN\n");
    #endif

    cancelRepeatFlag=true;
    
    /*Empty > do nothing*/
    if (playerState==PLAYER_EMPTY) return 0;


    switch(playerState) {
        /*From STOPPED to PLAYING*/
        case (PLAYER_STOPPED): {

            /*Open sound card*/
            int r = openSoundCard(currentChannels);
            if (r==-1) {
                if (afterLoad==false) gui->runMessageDialog("Error","Sound card initialization error",getLastError());
                return -1;
            }
            startVisualization();
            ASAP_Load(state,currentFilespec.c_str(),sapBuffer,currentLength);
	    ASAP_DetectSilence(state,prefs->getSilenceLimit());
            if (prefs->getIgnoreTimeTag()==true) {
                ASAP_PlaySong(state, currentSong, -1);
            }
            else {
                ASAP_PlaySong(state, currentSong, getSubsongLength(currentSong));
            }
            ASAP_MutePokeyChannels(state,muteMask);
            gui->updateAfterSubsongChange(currentSong,currentSongCount,getSubsongLength(currentSong),prefs->getIgnoreTimeTag());
            


            /*We're playing*/
            playerState=PLAYER_PLAYING;
            /*Start playback thread*/
            playbackThread = Glib::Thread::create(sigc::mem_fun(*this, &AlsaPlayer::playLoopNormal), true);
            break;
        }

        /*From PAUSED to PLAYING*/
        case (PLAYER_PAUSED): {
            int r = openSoundCard(currentChannels);
            if (r==-1) {
                if (afterLoad==false) gui->runMessageDialog("Error","Sound card initialization error",getLastError());
                return -1;
            }
            startVisualization();
            playerState=PLAYER_PLAYING;
            playbackThread = Glib::Thread::create(sigc::mem_fun(*this, &AlsaPlayer::playLoopNormal), true);
            break;
        }

        /*From PLAYING to PAUSED*/
        case (PLAYER_PLAYING): {
            playerState=PLAYER_PAUSED;
            playbackThread->join();
            closeSoundCard();
            stopVisualization();
            break;
        }

        default: {
            #ifdef DEBUG_PRINTOUTS
            printf("playOrPause: default branch [E]\n");
            #endif
        }
    }

    return 0;

    #ifdef DEBUG_PRINTOUTS
    printf("playOrPause:END\n");
    #endif


}

void AlsaPlayer::stop() {
    #ifdef DEBUG_PRINTOUTS
    printf("stop:BEGIN\n");
    #endif
    
    cancelRepeatFlag=true;

    /*When empty or stopped or empty, do nothing*/
    if (playerState==PLAYER_EMPTY || playerState==PLAYER_STOPPED) return;

    gui->updateTick(0,0,0.0f);
    gui->resetSeekScale();
    

    /*Clears all seek requests*/
    seekToMiliseconds=-1;


    /*If paused, reset is enough, so return*/
    if (playerState==PLAYER_PAUSED) {
        playerState=PLAYER_STOPPED;
        return;
    }

    playerState=PLAYER_STOPPED;
    /*Set state to stopped and wait join the thread*/

    #ifdef DEBUG_PRINTOUTS
    printf("stop:JOIN\n");
    #endif
    playbackThread->join();

    /*Close sound card*/
    closeSoundCard();
    stopVisualization();
    #ifdef DEBUG_PRINTOUTS
    printf("stop:END\n");
    #endif
}

void AlsaPlayer::changeSubsong() {
    ASAP_Load(state,currentFilespec.c_str(),sapBuffer,currentLength);
    if (prefs->getIgnoreTimeTag()==true) {
        ASAP_PlaySong(state, currentSong, -1);
    }
    else {
        ASAP_PlaySong(state, currentSong, getSubsongLength(currentSong));
    }
    
    ASAP_MutePokeyChannels(state,muteMask);
    /*Update GUI*/
    gui->updateAfterSubsongChange(currentSong,currentSongCount,getSubsongLength(currentSong),prefs->getIgnoreTimeTag());
}

void AlsaPlayer::next() {

    /*When empty, do nothing*/
    if (playerState==PLAYER_EMPTY) return;

    /*Update current song*/
    if (currentSong<currentSongCount-1) {
        currentSong++;
        changeSubsong();
    }

}

void AlsaPlayer::first() {
    if (playerState==PLAYER_EMPTY) return;
    currentSong=0;
    changeSubsong();
}

void AlsaPlayer::previous() {

    if (playerState==PLAYER_EMPTY) return;

    /*Update current song*/
    if (currentSong>0) {
        currentSong--;
        changeSubsong();
    }
}

void AlsaPlayer::seek(int tgtMiliseconds) {

    #ifdef DEBUG_PRINTOUTS
    printf("Seek to: %d\n",tgtMiliseconds);
    #endif

    /*Update counters*/
    ASAP_Seek(state,tgtMiliseconds);
    ASAP_MutePokeyChannels(state,muteMask);
    seekToMiliseconds=-1;

}

Glib::ustring AlsaPlayer::getLastError() {
    return errorMessage;
}

int AlsaPlayer::closeSoundCard() {
    #ifdef DEBUG_PRINTOUTS
    printf("closeSoundCard:BEGIN\n");
    #endif
    snd_pcm_close(playback_handle);

    #ifdef DEBUG_PRINTOUTS
    printf("closeSoundCard:END\n");
    #endif
}


void AlsaPlayer::playLoopNormal() {

    #ifdef DEBUG_PRINTOUTS
    printf("NORMAL LOOP START\n");
    #endif

    /*Allocate buffer*/
    if (!allocateBuffer()) {
        return;
    }

    int requestedBytes=bufferSize;
    int generatedBytes=0;
    int status=0;
    int lastPosition=0;
    int position=0;
    int requestedFramesBase=bufferSize>>currentChannels;
    int requestedFrames;
    unsigned char* bufferBase=buffer;

    int i=0;
    int j=0;

    /*Try at least once*/
    do {

       /*Seek request ?*/
       if (seekToMiliseconds>=0) {
           seek(seekToMiliseconds);
       }

       generatedBytes=ASAP_Generate(state,buffer,requestedBytes,ASAPSampleFormat_S16_L_E);
       requestedFrames=requestedFramesBase;

       while (requestedFrames>0) {

           status=snd_pcm_writei(playback_handle,buffer,requestedFrames);
           if (status==-EAGAIN) {
               Glib::Thread::self()->yield();
               continue;
           }
           if (status==-ESTRPIPE) {
               do {
                   Glib::Thread::self()->yield();
               }
               while (snd_pcm_resume(playback_handle)==-EAGAIN);
           }
           if (status<0) {
               status=snd_pcm_prepare(playback_handle);
           }
           if (status<0) {
               #ifdef DEBUG_PRINTOUTS
               printf("AlsaPlayer::playLoopNormal() : status<0");
               #endif
               return;
           }
           buffer+=(status<<(currentChannels-1));
           requestedFrames-=status;
       }
       buffer=bufferBase;
       
       position=(int)ASAP_GetPosition(state);
       
       if (abs(position-lastPosition)>1000) {
             Glib::signal_idle().connect(sigc::mem_fun(*this,&AlsaPlayer::tick),false);
             lastPosition=position;
       }
       

    }
    
    while (generatedBytes==requestedBytes && playerState==PLAYER_PLAYING);
    
    if (playerState==PLAYER_PLAYING) {
        cancelRepeatFlag=false;
        Glib::signal_idle().connect(sigc::mem_fun(*this,&AlsaPlayer::handleRepeat),false);
    }


    #ifdef DEBUG_PRINTOUTS
    printf("NORMAL LOOP END\n");
    #endif
}


bool AlsaPlayer::tick() {

    if (playerState!=PLAYER_PLAYING) return false;
            
    if (seekToMiliseconds==-1) {
        int elapsedMiliseconds=(int)ASAP_GetPosition(state);
        int elapsedSeconds=elapsedMiliseconds/1000;
        if (elapsedMiliseconds<0) elapsedMiliseconds=0;
        if (getSubsongLength(currentSong)<1) elapsedMiliseconds=0;
        gui->updateTick(elapsedSeconds/60,elapsedSeconds%60,elapsedMiliseconds);
    }
    
    return false;
    
    
}

bool AlsaPlayer::handleRepeat() {
    
    /*Before this routine has opportunity to be called, state of the
     *player could have chaged. If so, we do nothing. This routine
     *runs in the GTK+ thread, so sync is not needed. All methods that
     *neutralize repeat handling (lika playOrPause() etc. must have this
     *routine neutralized*/
     
    if (playerState!=PLAYER_PLAYING || cancelRepeatFlag==true) {
        cancelRepeatFlag=false;
        return false;
    }
    
    /*Always stop and join*/
    stop();
    int m = prefs->getRepeatMode();

    switch(m) {
        case REPEAT_NO_PROGRESS: return false;

        case REPEAT_SUBSONG: {
            playOrPause();
            return false;
        }
        case REPEAT_TUNE: {

            if(currentSong<currentSongCount-1) {
                next();
            }
            else {
                first();
            }
            playOrPause();
            return false;
        }
        case REPEAT_PLAYLIST: {
            if (currentSong<currentSongCount-1) {
                next();
                playOrPause();
            }
            else {
                handlePlaylistContinue();
            }
            return false;
        }
    }
    
}

int AlsaPlayer::openSoundCard(int channels) {

    #ifdef DEBUG_PRINTOUTS
    printf("openSoundCard: device: %s, channels: %d\n",prefs->getAlsaDevice().c_str(),channels);
    #endif

    int err;
    
    snd_pcm_hw_params_t *hw_params;
    

    int retVal=0;
    unsigned int sampleRate=44100;
    unsigned int periods=4;


    bufferSize=prefs->getAlsaBufferSize();
    snd_pcm_uframes_t periodSize=bufferSize;

    /*Setting up sound card*/
    errorMessage="";

    if ((err = snd_pcm_open(&playback_handle, prefs->getAlsaDevice().c_str(), SND_PCM_STREAM_PLAYBACK,0)) < 0) {
        errorMessage="Unable to open PCM device for playback";
        printf("%s\n",snd_strerror(err));
        return -1;

    }

    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
        errorMessage="Unable to allocate hw_params structure";
        printf("%s\n",snd_strerror(err));
        snd_pcm_close(playback_handle);
        return -1;
    }

    if ((err = snd_pcm_hw_params_any(playback_handle, hw_params)) < 0) {
        errorMessage="Function snd_pcm_hw_params_any() failed";
        printf("%s\n",snd_strerror(err));
        retVal=-1;

    }

    if ((err = snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        errorMessage="Unable to open PCM device for reading and writing interleaved data";
        printf("%s\n",snd_strerror(err));
        retVal=-1;
    }

    if ((err = snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
        errorMessage="Unable to set sample format to S16_LE";
        printf("%s\n",snd_strerror(err));
        retVal=-1;
    }

    if ((err = snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &sampleRate, 0)) < 0) {
        errorMessage="Unable to set 44100 Hz sample rate";
        printf("%s\n",snd_strerror(err));
        retVal=-1;
    }

    if ((err = snd_pcm_hw_params_set_channels(playback_handle, hw_params, channels)) < 0) {
        errorMessage="Unable to set number of channels";
        printf("%s\n",snd_strerror(err));
        retVal=-1;
    }

    
    if ((err=snd_pcm_hw_params_set_period_size_near(playback_handle, hw_params, &periodSize, NULL))<0) {
            errorMessage="snd_pcm_hw_params_set_period_size_near() returned zero or negative buffer size";
            printf("%s\n",snd_strerror(err));
            retVal=-1;
    }
    

    if ( (err=snd_pcm_hw_params_set_periods_near(playback_handle, hw_params, &periods, NULL))<0) {
        errorMessage="snd_pcm_hw_params_set_periods_near() failed";
        printf("%s\n",snd_strerror(err));
        retVal=-1;
    }

    if ((err = snd_pcm_hw_params(playback_handle, hw_params)) < 0) {
        errorMessage="snd_pcm_hw_params() failed";
        printf("%s\n",snd_strerror(err));
        retVal=-1;
    }

    /*Free hw params*/
    snd_pcm_hw_params_free(hw_params);
    

    /*If failed close handle*/
    if (retVal<0) snd_pcm_close(playback_handle);

    return retVal;
}

void AlsaPlayer::updateConfig() {

    /*Silence detection*/
    int l=prefs->getSilenceLimit();
    if (l>0) {
        ASAP_DetectSilence(state,l);
    }

    /*Visualization decrement*/
    visualizationDecrement=prefs->getVisualizationDecrement();

}







Glib::ustring AlsaPlayer::getFilename(Glib::ustring fspec) {
    int fsp = fspec.find_last_of('/');
    if (fsp==fspec.npos || fsp==0 || fsp==fspec.length()-1) return fspec;
    return fspec.substr(fsp+1,fspec.length()-1);
}

int AlsaPlayer::getSubsongLength(int index) {
    if (index<0 || index >32) return 0;
    else return ASAPInfo_GetDuration(modInfo,index);
}

void AlsaPlayer::startVisualization() {

    if (prefs->getVisualizationEnabled()==false) return;

    /*Set pointers*/
    visualizationWriteIndex=prefs->getVisualizationSynchro();
    visualizationReadIndex=0;
    
    visualizationSlot= sigc::bind(sigc::mem_fun(*this, &AlsaPlayer::visualizationTick), 0);
    visualizationConnection = Glib::signal_timeout().connect(visualizationSlot, prefs->getVisualizationPeriod());
}

void AlsaPlayer::stopVisualization(bool ultimately) {

    if (!visualizationConnection.empty()) visualizationConnection.disconnect();
    gui->resetVisualization();
    
}

void AlsaPlayer::destroy() {
    /*Stop playback if possible*/
    stop();
    stopVisualization();
}



bool AlsaPlayer::visualizationTick(int ti) {

    /*Retrieve current volumes*/
    if (currentChannels==1) {
        *(visualizationVolumes)=ASAP_GetPokeyChannelVolume(state,0)<<4;
        *(visualizationVolumes+2)=ASAP_GetPokeyChannelVolume(state,1)<<4;
        *(visualizationVolumes+4)=ASAP_GetPokeyChannelVolume(state,2)<<4;
        *(visualizationVolumes+6)=ASAP_GetPokeyChannelVolume(state,3)<<4;
        *(visualizationVolumes+1)=*(visualizationVolumes);
        *(visualizationVolumes+3)=*(visualizationVolumes+2);
        *(visualizationVolumes+5)=*(visualizationVolumes+4);
        *(visualizationVolumes+7)=*(visualizationVolumes+6);
    }
    else {
        for(int i=0;i<8;i++) {
                *(visualizationVolumes+i)=ASAP_GetPokeyChannelVolume(state,i)<<4;
        }
    }
        

    /*Previus index*/
    int z=(visualizationWriteIndex+19)%20;
    int* writePtr;
    int* volumePtr;
    int* previousPtr;
    for (int i=0;i<8;i++) {

        /*Pointers*/
        writePtr=&(visualizationPool[visualizationWriteIndex][i]);
        volumePtr=(visualizationVolumes+i);
        previousPtr=&(visualizationPool[z][i]);

        if (*previousPtr<(*volumePtr)) {
            *writePtr=*volumePtr;
        }
        else {
            *writePtr=*previousPtr-visualizationDecrement;
        }
        if (*writePtr<0) *writePtr=0;
    }

    gui->updateVisualization(visualizationPool[visualizationReadIndex]);

    visualizationWriteIndex++;
    visualizationReadIndex++;
    visualizationWriteIndex%=20;
    visualizationReadIndex%=20;

    return true;
}


/*Seek request*/
void AlsaPlayer::setSeekRequest(double d) {
    if (isEmpty()) return;
    int tgt=(int)d;
    if (d<0 || getSubsongLength(currentSong)<1) seekToMiliseconds=-1;
    else seekToMiliseconds=tgt;
}

void AlsaPlayer::setChannelsMute(int mute) {
    ASAP_MutePokeyChannels(state,mute);
    muteMask=mute;
}



bool AlsaPlayer::isEmpty() {
    if (playerState==PLAYER_EMPTY) return true;
    return false;
}

Glib::ustring AlsaPlayer::getCurrentFilespec() {
    return currentFilespec;
}

bool AlsaPlayer::allocateBuffer() {


    #ifdef DEBUG_PRINTOUTS
    printf("AlsaPlayer::allocateBuffer() %d\n",bufferSize);
    #endif

    if (buffer!=NULL) {
        free(buffer);
        buffer=NULL;
    }


    buffer = (unsigned char*)malloc(bufferSize);
    if (buffer==NULL) return false;
    return true;
}

int AlsaPlayer::normalizeSAP(unsigned char** _buf,int buflen) {

    unsigned char* alternateBuffer=NULL;
    unsigned char* buf=NULL;

    #ifdef DEBUG_PRINTOUTS
    printf("AlsaPlayer::normalizeSAP()\n");
    #endif

    /*null or too small*/
    if (_buf==NULL || buflen<3) return 0;

    buf=*_buf;
    /*points to null*/
    if (buf==NULL) return 0;

    /*Allocate alternate ?*/
    alternateBuffer=(unsigned char*)calloc(buflen,1);
    if (alternateBuffer==NULL) return 0;

    /*Not SAP ?*/
    if (buf[0]!=0x53 || buf[1]!=0x41 || buf[2]!=0x50) {
        if (alternateBuffer!=NULL) free(alternateBuffer);
        return 0;
    }

    #ifdef DEBUG_PRINTOUTS
    printf("normalizeSAP():Detected as SAP\n");
    #endif

    /*Copy SAP header*/
    memcpy(alternateBuffer,buf,3);

    int pos=3;
    int apos=3;

    int cnt=0;
    int ers=0;

    while (pos<buflen) {

        cnt=buflen-pos;
        ers=0;

        /*Check for $FF $FF and copy data and break if necessary*/
        if (cnt>1) {
            if (buf[pos]==255 && buf[pos+1]==255) {

                for(int i=0;i<cnt;i++) {
                    alternateBuffer[apos+i]=buf[pos+i];
                }
                free(*_buf);    /*We are sure it is not NULL*/
                *_buf=alternateBuffer;

                #ifdef DEBUG_PRINTOUTS
                printf("normalizeSAP():OK, size: %d\n",apos+cnt);
                #endif

                return apos+cnt;
            }
        }

        /*Check for AUTHOR,NAME and TITLE problematic tags*/
        if (cnt>5 && strncmp("AUTHOR",(const char*)buf+pos,6)==0) {ers=1;pos+=6;}
        else if (cnt>3 && strncmp("NAME",(const char*)buf+pos,4)==0) {ers=1;pos+=4;}
        else if (cnt>3 && strncmp("DATE",(const char*)buf+pos,4)==0) {ers=1;pos+=4;}

        /*If problematic tag is found, we will simply not add it to the
         *alternate buffer.
         */
        if (ers==1) {

            /*1st " */
            while (pos<buflen && buf[pos]!='\"') {
                pos++;
            }
            pos++;

            /*2nd " */
            while (pos<buflen && buf[pos]!='\"') {
                pos++;
            }
            pos++;
        }

        /*Find End of line*/
        while (pos<buflen && buf[pos]!=0x0A) {
            alternateBuffer[apos]=buf[pos];
            pos++;apos++;
        }
        alternateBuffer[apos]=buf[pos];
        pos++;apos++;

    }

    if (alternateBuffer!=NULL) free(alternateBuffer);
    return 0;

}

#ifndef _PREFERENCES_H
#define _PREFERENCES_H


#include "platform.h"
#include <glib.h>
#include <gtkmm.h>
#include <stdlib.h>

#define REPEAT_NO_PROGRESS 0
#define REPEAT_SUBSONG 1
#define REPEAT_TUNE 2
#define REPEAT_PLAYLIST 3

#define DEFAULT_SILENCE 5

class Preferences {
public:
    Preferences();
    int getSilenceLimit();
    int getRepeatMode();
    bool getUseDefaultDirectory();
    Glib::ustring getDefaultDirectory();
    bool getAlwaysFirstSubsong();
    int getVisualizationPeriod();
    int getMainWindowHeight();
    bool getAdvancedControlsShownByDefault();
    int getVisualizationSynchro();
    Glib::ustring getLastPlaylist();
    Glib::ustring getAsmaDirectory();
    Glib::ustring getStilFile();
    bool getUseStilFile();
    bool getNormalizeSAPHeader();
    int getVisualizationDecrement();

    Glib::ustring getLastLoadDirectory();
    Glib::ustring getLastBrowseAndPlayDirectory();
    Glib::ustring getLastAddToPlaylistDirectory();
    Glib::ustring getLastPlaylistDirectory();

    void setRepeatMode(int mode);
    void setUseDefaultDirectory(bool use);
    void setDefaultDirectory(Glib::ustring dir);
    void setSilenceLimit(int limit);
    void setAlwaysFirstSubsong(bool b);
    void setVisualizationPeriod(int period);
    void setMainWindowHeight(int height);
    void setAdvancedControlsShownByDefault(bool b);
    void setVisualizationSynchro(int sync);
    void setLastPlaylist(Glib::ustring fspec);
    void setAsmaDirectory(Glib::ustring fspec);
    void setStilFile(Glib::ustring fspec);
    void setUseStilFile(bool b);
    void setNormalizeSAPHeader(bool b);
    void setVisualizationDecrement(int d);

    void setLastLoadDirectory(Glib::ustring fspec);
    void setLastBrowseAndPlayDirectory(Glib::ustring fspec);
    void setLastAddToPlaylistDirectory(Glib::ustring fspec);
    void setLastPlaylistDirectory(Glib::ustring fspec);

    void load();
    void save();


private:
    int silenceLimit;
    int repeatMode;
    bool useDefaultDirectory;
    Glib::ustring defaultDirectory;
    bool alwaysFirstSubsong;
    int visualizationPeriod;
    int visualizationDecrement;
    int mainWindowHeight;
    bool advancedControlsShownByDefault;
    int visualizationSynchro;
    Glib::ustring lastPlaylist;
    Glib::ustring asmaDirectory;
    Glib::ustring stilFile;
    bool useStilFile;
    bool normalizeSAPHeader;

    Glib::ustring lastLoadDirectory;
    Glib::ustring lastBrowseAndPlayDirectory;
    Glib::ustring lastPlaylistDirectory;
    Glib::ustring lastAddToPlaylistDirectory;

    


    int getIntProperty(const char* key, int dflt, int min, int max);
    Glib::ustring getStringProperty(const char* key, const char* dflt);
    bool getBooleanProperty(const char* key, bool dflt);

    Glib::ustring preferencesFile;

    GKeyFile* keyFile;

};


#endif /* _PREFERENCES_H */


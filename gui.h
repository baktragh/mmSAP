
#ifndef _GUI_H
#define _GUI_H


#include "platform.h"
#include "preferences.h"

#include <gtkmm.h>


#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

class GUI {
public:

    /*Tooltips*/
    Gtk::Tooltips toolTips;


    /*Window and player*/
    Gtk::Window* wndMain;
    Gtk::FileChooserDialog* fcdLoad;
    Gtk::Button* btnPlayOrPause;
    Gtk::ImageMenuItem* mniLoadSAP;
    Gtk::Button* btnStop;
    Gtk::Button* btnPrevious;
    Gtk::Button* btnNext;
    Gtk::Label* lblSongCounter;
    Gtk::HScale* hscSeeking;
    Gtk::Label* lblElapsedTime;
    Gtk::Label* lblFilename;
    Gtk::VBox* vbxPlayerBox;
    Gtk::ToggleButton* tgbAdvancedControls;
    Gtk::Button* btnNextTune;
    Gtk::Button* btnPreviousTune;
    Gtk::Label* lblTuneInfo;

    /*Browse and play*/
    Gtk::ImageMenuItem* mniBrowseAndPlay;
    Gtk::FileChooserDialog* fcdBrowseAndPlay;
    Gtk::Button* btnBrowseAndPlayClose;



    /*Advanced controls*/
    Gtk::Notebook* ntbAdvancedControls;
    Gtk::TextView* txvTuneInfo;
    Gtk::CheckButton* chbChannelMutes[8];

    /*Repeat*/
    Gtk::RadioMenuItem* mniRepNothing;
    Gtk::RadioMenuItem* mniRepSubsong;
    Gtk::RadioMenuItem* mniRepTune;
    Gtk::RadioMenuItem* mniRepPlaylist;

    /*About*/
    Gtk::ImageMenuItem* mniAbout;
    Gtk::Dialog* dlgAbout;
    Gtk::Label* lblVersion;

    /*Quit*/
    Gtk::ImageMenuItem* mniQuit;

    /*Preferences dialog*/
    Gtk::Dialog* dlgPreferences;
    Gtk::CheckButton* chbUseDefaultDirectory;
    Gtk::Entry* entDefaultDirectory;
    Gtk::ImageMenuItem* mniPreferences;
    Gtk::HScale* hscSilenceLimit;
    Gtk::CheckButton* chbAlwaysFirstSubsong;
    Gtk::CheckButton* chbNormalizeSAPHeader;
    Gtk::SpinButton* spbVisualizationPeriod;
    Gtk::CheckButton* chbAdvancedControlsShownByDefault;
    Gtk::HScale* hscVisualizationSynchro;
    Gtk::Entry* entAsmaDirectory;
    Gtk::Entry* entStilFile;
    Gtk::CheckButton* chbUseStil;
    Gtk::Entry* entAlsaDevice;
    Gtk::CheckButton* chbAlwaysStereo;
    Gtk::SpinButton* spbVisualizationDecrement;
    Gtk::Button* btnBrowseASMADir;
    Gtk::Button* btnBrowseSTIL;


    /*Visualization*/
    Gtk::ProgressBar* prbChans[8];

    /*Playlist*/
    Gtk::TreeView* trvPlaylist;
    Gtk::ImageMenuItem* mniAddFileToPlaylist;
    Gtk::ImageMenuItem* mniAddDirectoryToPlaylist;

    Gtk::FileChooserDialog* fcdAddToPlaylist;
    Gtk::ImageMenuItem* mniSavePlaylist;
    Gtk::ImageMenuItem* mniLoadPlaylist;
    Gtk::FileChooserDialog* fcdPlaylist;

    Gtk::ImageMenuItem* mniPlaylistRemoveAll;
    Gtk::ImageMenuItem* mniPlaylistRemoveSelected;
    Gtk::MenuItem* mniPlaylistTrimSelected;
    Gtk::ImageMenuItem* mniPlaylistPurge;

    Gtk::MenuItem* mniPlaylistSelectAll;
    Gtk::MenuItem* mniPlaylistSelectNone;
    Gtk::MenuItem* mniPlaylistSelectInvert;

    Gtk::ImageMenuItem* mniAddPlayerFile;

    Gtk::VBox* vbxPlaylist;


    /*Establishment*/
    void create(const char* pathToMainBinary);
    void connectSignals();
    void setupPlaylistView(Gtk::TreeModelColumn<bool> *tmc0, Gtk::TreeModelColumn<Glib::ustring> *tmc1, Glib::RefPtr<Gtk::ListStore> model);

    /*Updates called by player*/
    void updateAfterSAPLoad(Glib::ustring filename, int songNumber, int songCount, int duration, Glib::ustring info);
    void updateTick(int minutes, int seconds, int totalMiliseconds);
    void updateAfterSubsongChange(int songNumber, int songCount, int duration);
    void resetSeekScale();
    void resetPlayer();
    void resetVisualization();

    /*Supplementary*/
    void runMessageDialog(Glib::ustring title, Glib::ustring mainText, Glib::ustring text);
    void updatePreferencesChanged(Preferences* prefs, bool startup = false);
    void flushPreferences(Preferences* prefs);
    void setVersionString(const char* versionString, const char* asapVersionString);

    /*Visualization*/
    void updateVisualization(int* volumes);

    void on_playlistSelectInvert();

private:
    sigc::connection hscSeekingConnection;

    bool on_hscSeekingButtonPressed(GdkEventButton* btn);
    bool on_hscSeekingButtonReleased(GdkEventButton* btn);

    void on_playlistSelectAll();
    void on_playlistSelectNone();

    void on_btnBrowseASMADir();
    void on_btnBrowseSTIL();




    bool tickUpdateBlocked;


};




#endif /* _GUI_H */


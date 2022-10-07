
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
    //Gtk::Tooltips toolTips;


    /*Window and player*/
    Gtk::Window* wndMain;
    Gtk::FileChooserDialog* fcdLoad;
    Gtk::Button* btnPlayOrPause;
    Gtk::MenuItem* mniLoadSAP;
    Gtk::Button* btnStop;
    Gtk::Button* btnPrevious;
    Gtk::Button* btnNext;
    Gtk::Label* lblSongCounter;
    Gtk::Scale* hscSeeking;
    Gtk::Label* lblElapsedTime;
    Gtk::Label* lblFilename;
    Gtk::Box* vbxPlayerBox;
    Gtk::ToggleButton* tgbAdvancedControls;
    Gtk::Button* btnNextTune;
    Gtk::Button* btnPreviousTune;
    Gtk::Label* lblTuneInfo;

    /*Browse and play*/
    Gtk::MenuItem* mniBrowseAndPlay;
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
    Gtk::MenuItem* mniAbout;
    Gtk::Dialog* dlgAbout;
    Gtk::Label* lblVersion;

    /*Quit*/
    Gtk::MenuItem* mniQuit;

    /*Preferences dialog*/
    Gtk::Dialog* dlgPreferences;
    Gtk::CheckButton* chbUseDefaultDirectory;
    Gtk::Entry* entDefaultDirectory;
    Gtk::MenuItem* mniPreferences;
    Gtk::Scale* hscSilenceLimit;
    Gtk::CheckButton* chbAlwaysFirstSubsong;
    Gtk::CheckButton* chbNormalizeSAPHeader;
    Gtk::SpinButton* spbVisualizationPeriod;
    Gtk::CheckButton* chbAdvancedControlsShownByDefault;
    Gtk::Scale* hscVisualizationSynchro;
    Gtk::Entry* entAsmaDirectory;
    Gtk::Entry* entStilFile;
    Gtk::CheckButton* chbUseStil;
    Gtk::SpinButton* spbVisualizationDecrement;
    Gtk::Button* btnBrowseASMADir;
    Gtk::Button* btnBrowseSTIL;


    /*Visualization*/
    Gtk::ProgressBar* prbChans[8];

    /*Playlist*/
    Gtk::TreeView* trvPlaylist;
    Gtk::MenuItem* mniAddFileToPlaylist;
    Gtk::MenuItem* mniAddDirectoryToPlaylist;

    Gtk::FileChooserDialog* fcdAddToPlaylist;
    Gtk::MenuItem* mniSavePlaylist;
    Gtk::MenuItem* mniLoadPlaylist;
    Gtk::FileChooserDialog* fcdPlaylist;

    Gtk::MenuItem* mniPlaylistRemoveAll;
    Gtk::MenuItem* mniPlaylistRemoveSelected;
    Gtk::MenuItem* mniPlaylistTrimSelected;
    Gtk::MenuItem* mniPlaylistPurge;

    Gtk::MenuItem* mniPlaylistSelectAll;
    Gtk::MenuItem* mniPlaylistSelectNone;
    Gtk::MenuItem* mniPlaylistSelectInvert;

    Gtk::MenuItem* mniAddPlayerFile;

    Gtk::Box* vbxPlaylist;


    /*Establishment*/
    void create(const char* pathToMainBinary);
    void connectSignals();
    void setupPlaylistView(Gtk::TreeModelColumn<Glib::ustring> *tmc0, Glib::RefPtr<Gtk::ListStore> model);

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
    Gtk::TreeViewColumn* tvcPlaylistMarkup;

    bool on_hscSeekingButtonPressed(GdkEventButton* btn);
    bool on_hscSeekingButtonReleased(GdkEventButton* btn);

    void on_playlistSelectAll();
    void on_playlistSelectNone();

    void on_btnBrowseASMADir();
    void on_btnBrowseSTIL();




    bool tickUpdateBlocked;


};




#endif /* _GUI_H */


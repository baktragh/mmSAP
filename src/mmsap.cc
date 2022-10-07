/* 
   mmSAP-win32  v3.0.0
   
   SAP (Slight atari player) player based on ASAP library
   Copyright (C) 2009-2022 Michael Kalouš <zylon@post.cz>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include "alsaplayer.h"

#include <gtkmm.h> 

#include "alsaplayer.h"
#include "gui.h"
#include "playlist.h"
#include "asma.h"

#define MMSAP_VERSION_STRING "mmSAP-win32 3.0.0"


/*Prototypes*/
void loadAndPlayFile(Glib::ustring fspec, bool shout = true);


/*Global variables*/
Preferences* preferences;
GUI* gui;

AlsaPlayer* aPlayer;
Playlist* playlist;
Asma* asma;


bool savePreferencesFlag;

/*Main*/
int main(int argc, char** argv) {

    savePreferencesFlag = true;


    /*Initialize library*/
    Gtk::Main kit(argc, argv, false);

    /*Now initialize main objects*/
    Preferences xprefs;
    preferences = &xprefs;
    GUI xgui;
    gui = &xgui;
    AlsaPlayer xplayer(preferences, gui);
    aPlayer = &xplayer;
    Playlist xplaylist;
    playlist = &xplaylist;
    Asma xasma;
    asma = &xasma;



    /*Load preferences*/
    preferences->load();
    aPlayer->updateConfig();



    /*Create GUI*/
    gui->create(argv[0]);
    gui->connectSignals();
    /*Update GUI according to preferences*/
    gui->updatePreferencesChanged(preferences, true);
    /*Playlist Tree View setup*/
    gui->setupPlaylistView(&(playlist->getColumns()->clmMarkup), playlist->getModel());
    /*Update version*/
    gui->setVersionString(MMSAP_VERSION_STRING, ASAPInfo_VERSION);


    /*Try to load sepcified tune from command line*/
    Glib::ustring rfsp = "";
    if (argc > 1) {
        rfsp = Glib::ustring(argv[1]);
    }


    if (rfsp != "") {
        loadAndPlayFile(rfsp, false);
        playlist->clearCurrentColumn();
    }


    /*Try to load last playlist and don't complain when it fails*/
    if (playlist->load(preferences->getLastPlaylist()) < 0) {
        preferences->setLastPlaylist("");
    }        /*If playlist loaded ok, update playlist file chooser path*/
    else {
        gui->fcdPlaylist->set_filename(preferences->getLastPlaylist());
    }

    /*Try to load asma if enabled*/
    if (preferences->getUseStilFile() == true) {
        asma->initialize(preferences->getAsmaDirectory(), preferences->getStilFile());
        asma->parse();
    }

    /*Show main window*/
    Gtk::Main::run(*(gui->wndMain));

    /*Save preferences*/
    if (savePreferencesFlag == true) {
        preferences->save();
    }

    return (EXIT_SUCCESS);
}

/*Event handlers**************************************************************/

/*Loading SAP*/
void on_loadSAP() {

    gui->fcdLoad->set_current_folder(gui->fcdLoad->get_current_folder()); 
    int r=gui->fcdLoad->run();
    gui->fcdLoad->hide();
    
    if (r==1) {
        Glib::RefPtr<Gio::File> pickedFileRef=gui->fcdLoad->get_file();
        loadAndPlayFile(pickedFileRef->get_path());
        playlist->clearCurrentColumn();
        gui->recentManager->add_item(pickedFileRef->get_uri());
        gui->fcdLoad->set_current_folder(pickedFileRef->get_parent()->get_path());
    }

}

/*Play or pause button*/
void on_playOrPause() {
    aPlayer->playOrPause();
}

/*Stop button*/
void on_stop() {
    aPlayer->stop();
}

/*Previous button*/
void on_previous() {
    aPlayer->previous();
}

/*Next button*/
void on_next() {
    aPlayer->next();
}

void on_seek() {
    aPlayer->setSeekRequest(gui->hscSeeking->get_value());
}

void on_handleRepeatMode() {

    int m = REPEAT_NO_PROGRESS;

    if (gui->mniRepSubsong->get_active() == true) {
        m = REPEAT_SUBSONG;
    } else if (gui->mniRepTune->get_active() == true) {
        m = REPEAT_TUNE;
    } else if (gui->mniRepPlaylist->get_active() == true) {
        m = REPEAT_PLAYLIST;
    }

    preferences->setRepeatMode(m);
}

/*About ***********************************************************************/
bool on_aboutClose(GdkEventAny* evt) {
    gui->dlgAbout->hide();
    return true;
}

void on_aboutInvoke() {
    gui->dlgAbout->run();
    gui->dlgAbout->hide();
}

/*Quit ************************************************************************/
void quit(bool fromRemote) {

    /*Correctly store main window size*/
    if (gui->tgbAdvancedControls->get_active() == true) {
        preferences->setMainWindowHeight(gui->wndMain->get_height());
    }


    /*Destroy player*/
    aPlayer->destroy();
    /*Hide main window, this will finish main loop*/
    gui->wndMain->hide();

    /*Save or not save preferences ?*/
    if (fromRemote == true) savePreferencesFlag = false;
}

void on_quitInvoke() {
    quit(false);
}

bool on_wndMainClose(GdkEventAny* evt) {
    quit(false);
    return true;
}

/*Preferences *****************************************************************/
void on_preferencesInvoke() {
    int k = gui->dlgPreferences->run();
    gui->dlgPreferences->hide();
    /*Revert dialog*/
    if (k == 0) {
        gui->updatePreferencesChanged(preferences);
    } else {
        gui->flushPreferences(preferences);
        aPlayer->updateConfig();
    }

}

bool on_preferencesClose(GdkEventAny* evt) {
    gui->dlgPreferences->hide();
    return true;

}

/*Advanced controls toggle ****************************************************/
void on_advancedControlsToggled() {

    bool b = gui->tgbAdvancedControls->get_active();
    if (b == true) {
        gui->ntbAdvancedControls->show();
        gui->wndMain->set_resizable(true);
        int h = preferences->getMainWindowHeight();
        if (h > 0) {
            gui->wndMain->resize(gui->wndMain->get_width(), h);
        }
    } else {
        preferences->setMainWindowHeight(gui->wndMain->get_height());
        gui->wndMain->set_resizable(false);
        gui->ntbAdvancedControls->hide();
    }
}

/*Channel mute change **********************************************************/
void on_channelMuteChange() {

    int mute = 0;
    int base = 1;

    for (int i = 0; i < 8; i++) {
        if (gui->chbChannelMutes[i]->get_active() == false) mute += base;
        base = base << 1;
    }

    aPlayer->setChannelsMute(mute);
}

/*Playlist operations **********************************************************/
void on_addFileToPlaylist() {
    gui->fcdAddToPlaylist->set_action(Gtk::FILE_CHOOSER_ACTION_OPEN);
    int r = gui->fcdAddToPlaylist->run();
    gui->fcdAddToPlaylist->hide();
    if (r == 1) {
        bool b = playlist->addFile(gui->fcdAddToPlaylist->get_filename());
        if (b == false) {
            gui->runMessageDialog("Error", "Error when adding file", "File could not be added");
        }
    }
}

void on_addDirectoryToPlaylist() {
    gui->fcdAddToPlaylist->set_action(Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    int r = gui->fcdAddToPlaylist->run();
    gui->fcdAddToPlaylist->hide();
    if (r == 1) {
        bool b = playlist->addDirectory(gui->fcdAddToPlaylist->get_filename());
        if (b == false) {
            gui->runMessageDialog("Error", "Error when adding directory", "Directory could not be added");
        }
    }
}

void on_playlistItemActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column) {
    /*Retrieve filespec*/
    Gtk::TreeModel::Row row = *(playlist->getModel()->get_iter(path));
    loadAndPlayFile(row.get_value(playlist->getColumns()->clmFilespec));
    /*Update current column*/
    playlist->clearCurrentColumn();
    playlist->setRowActive(row,true);

}

void on_savePlaylist() {
    gui->fcdPlaylist->set_current_folder(gui->fcdPlaylist->get_current_folder());
    gui->fcdPlaylist->set_action(Gtk::FILE_CHOOSER_ACTION_SAVE);
    int r = gui->fcdPlaylist->run();
    gui->fcdPlaylist->hide();
    if (r == 1) {
        int t = playlist->save(gui->fcdPlaylist->get_filename());
        /*Error or empty ?*/
        if (t == PLAYLIST_SAVED_ERROR) {
            gui->runMessageDialog("Error", "Error when saving playlist", "Playlist was not saved");
            return;
        }
        if (t == PLAYLIST_SAVED_EMPTY) {
            gui->runMessageDialog("Warning", "Playlist not saved", "Attempt to save empty playlist");
            return;
        }
        /*OK ?*/
        preferences->setLastPlaylist(gui->fcdPlaylist->get_filename());
    }
}

void on_loadPlaylist() {
    gui->fcdPlaylist->set_action(Gtk::FILE_CHOOSER_ACTION_OPEN);
    int r = gui->fcdPlaylist->run();
    gui->fcdPlaylist->hide();
    if (r == 1) {
        int t = playlist->load(gui->fcdPlaylist->get_filename());
        /*Error*/
        if (t < 0) {
            gui->runMessageDialog("Error", "Error when loading playlist", "Playlist was not loaded");
        }            /*OK*/
        else {
            preferences->setLastPlaylist(gui->fcdPlaylist->get_filename());
        }
    }
}

void on_addPlayerFile() {
    if (aPlayer->isEmpty() == true) return;
    bool b = playlist->addFile(aPlayer->getCurrentFilespec());
    if (b == false) {
        gui->runMessageDialog("Error", "Error when adding file", "File could not be adeed");
    }
}

void handlePlaylistContinue() {
    Glib::ustring fspec;
    bool b = playlist->continueToNext(&fspec);
    if (b == true) {
        loadAndPlayFile(fspec);
    }
}

void on_nextTune() {
    handlePlaylistContinue();
}

void on_previousTune() {
    Glib::ustring fspec;
    bool b = playlist->continueToPrevious(&fspec);
    if (b == true) {
        loadAndPlayFile(fspec);
    }
}

void on_removeAllFromPlaylist() {
    playlist->getModel()->clear();
}

void on_purgePlaylist() {
    on_removeAllFromPlaylist();
    preferences->setLastPlaylist("");
}

void on_removeSelectedFromPlaylist() {

    Glib::RefPtr<Gtk::TreeSelection> selection = gui->trvPlaylist->get_selection();
    std::vector<Gtk::TreePath> tps = (std::vector<Gtk::TreePath>)selection->get_selected_rows();
    playlist->removePaths(&tps);
}

void on_trimPlaylistSelection() {
    Glib::RefPtr<Gtk::TreeSelection> selection = gui->trvPlaylist->get_selection();
    std::vector<Gtk::TreePath> tps = (std::vector<Gtk::TreePath>)selection->get_selected_rows();
    if (tps.size() < 1) return;

    gui->on_playlistSelectInvert();
    on_removeSelectedFromPlaylist();
}

/*Load and play ****************************************************************/
void loadAndPlayFile(Glib::ustring fspec, bool shout) {

    gui->resetPlayer();

    int er = aPlayer->load(fspec);
    if (er == PLAYER_LP_LOAD_ERROR && shout == true) {
        gui->runMessageDialog("Error", "Error when loading file", aPlayer->getLastError());
        return;
    }

    if (er == PLAYER_LP_ALSA_ERROR && shout == true) {
        gui->runMessageDialog("Error", "Sound card initialization error", aPlayer->getLastError());
    }

    /*Add info from asma*/
    Glib::ustring sti = gui->txvTuneInfo->get_buffer()->get_text();
    Glib::ustring et = asma->getEntry(fspec);

    if (et.length() > 0) {
        sti += '\n';
        sti += et;
        gui->txvTuneInfo->get_buffer()->set_text(sti);
        //gui->toolTips.set_tip(*(gui->lblFilename), Glib::ustring(g_strstrip((gchar*) sti.c_str())));
    }

}

/*Drag and drop ***************************************************************/
void on_playerDrop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time) {
    /*Convert data to string*/
    Glib::ustring data = selection_data.get_data_as_string();
    Glib::ustring fspec;
    /*Convert string to single file specifier*/
    bool b = playlist->getDraggedSingleFilespec(&fspec, data);
    /*If not possible, return*/
    if (b == false) return;
    /*Try to load tune*/
    loadAndPlayFile(fspec);
    playlist->clearCurrentColumn();
}

void on_playlistDrop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time) {
    /*Convert data to string*/
    Glib::ustring data = selection_data.get_data_as_string();
    std::vector<Glib::ustring> items;
    /*Convert string to multiple file specifiers*/
    bool b = playlist->getDraggedMultipleFilespecs(&items, data);
    if (b == false) return;
    playlist->addMultipleFilespecs(&items);
}

/*Browse and play ***************************************************************/
bool on_fcdBrowseAndPlayClose(GdkEventAny* e) {
    gui->fcdBrowseAndPlay->hide();
    return true;
}

void on_fcdBrowseAndPlayFileActivated() {

     Glib::RefPtr<Gio::File> pickedFileRef = gui->fcdBrowseAndPlay->get_file();
    
    if (playlist->isFile(pickedFileRef->get_path())) {
        loadAndPlayFile(pickedFileRef->get_path(),false);
        playlist->clearCurrentColumn();
        gui->recentManager->add_item(pickedFileRef->get_uri());
    }
}

void on_browseAndPlay() {

    gui->fcdBrowseAndPlay->set_current_folder(gui->fcdBrowseAndPlay->get_current_folder());
    gui->fcdBrowseAndPlay->show();
}

void on_browseAndPlayClose() {
    gui->fcdBrowseAndPlay->hide();
}



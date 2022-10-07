#include "preferences.h"

#include "gui.h"

/*Event handlers*/
extern void on_loadSAP();
extern void on_playOrPause();
extern void on_stop();
extern void on_previous();
extern void on_next();
extern void on_seek();
extern void on_moveSlider(Gtk::ScrollType sct);
extern void on_handleRepeatMode();
extern void on_playerDrop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);
extern void on_playlistDrop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);
extern bool on_aboutClose(GdkEventAny* evt);
extern void on_aboutInvoke();
extern void on_quitInvoke();
extern void on_preferencesInvoke();
extern bool on_preferencesClose(GdkEventAny* evt);
extern bool on_wndMainClose(GdkEventAny* evt);
extern void on_advancedControlsToggled();
extern void on_channelMuteChange();
extern void on_addFileToPlaylist();
extern void on_addDirectoryToPlaylist();
extern void on_playlistItemActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
extern void on_savePlaylist();
extern void on_loadPlaylist();
extern void on_nextTune();
extern void on_previousTune();
extern void on_removeAllFromPlaylist();
extern void on_removeSelectedFromPlaylist();
extern void on_trimPlaylistSelection();
extern void on_purgePlaylist();
extern bool on_fcdBrowseAndPlayClose(GdkEventAny* evt);
extern void on_fcdBrowseAndPlayFileActivated();
extern void on_browseAndPlay();
extern void on_browseAndPlayClose();
extern void on_addPlayerFile();

void GUI::create(const char* pathToMainBinary) {

    /*Pre-initialization*/
    tickUpdateBlocked = false;

    /*Determine path and load UI from the file*/
    GFile* file = g_file_new_for_path(pathToMainBinary);
    GFile* parent = g_file_get_parent(file);
    GFile* uberParent = g_file_get_parent(parent);
    Glib::ustring uiFilespec = Glib::ustring(g_file_get_path(uberParent) + Glib::ustring("\\share\\mmsap\\mmsap2.ui"));

    /*Free GFiles allocated to determine path to the UI file*/
    g_object_unref(uberParent);
    g_object_unref(parent);
    g_object_unref(file);

    Glib::RefPtr<Gtk::Builder> xml = Gtk::Builder::create_from_file(uiFilespec);

    /*Main window and player*/
    xml->get_widget("wndMain", wndMain);
    xml->get_widget("fcdLoad", fcdLoad);
    xml->get_widget("btnPlayOrPause", btnPlayOrPause);
    xml->get_widget("mniLoadSAP", mniLoadSAP);
    xml->get_widget("btnStop", btnStop);
    xml->get_widget("btnPrevious", btnPrevious);
    xml->get_widget("btnNext", btnNext);
    xml->get_widget("hscSeeking", hscSeeking);
    xml->get_widget("lblSongCounter", lblSongCounter);
    xml->get_widget("lblElapsedTime", lblElapsedTime);
    xml->get_widget("lblFilename", lblFilename);
    xml->get_widget("tgbAdvancedControls", tgbAdvancedControls);
    xml->get_widget("btnNextTune", btnNextTune);
    xml->get_widget("btnPreviousTune", btnPreviousTune);
    xml->get_widget("lblTuneInfo", lblTuneInfo);

    /*Browse and play*/
    xml->get_widget("mniBrowseAndPlay", mniBrowseAndPlay);
    xml->get_widget("fcdBrowseAndPlay", fcdBrowseAndPlay);
    xml->get_widget("btnBrowseAndPlayClose", btnBrowseAndPlayClose);

    /*Player's drag and drop vbox*/
    xml->get_widget("vbxPlayerBox", vbxPlayerBox);
    std::list<Gtk::TargetEntry> playerTgtList;
    //vbxPlayerBox->drag_dest_set(playerTgtList, Gtk::DEST_DEFAULT_ALL, Gdk::ACTION_DEFAULT | Gdk::ACTION_MOVE | Gdk::ACTION_COPY);
    vbxPlayerBox->drag_dest_add_uri_targets();
    vbxPlayerBox->drag_dest_add_text_targets();

    /*Advanced controls*/
    xml->get_widget("ntbAdvancedControls", ntbAdvancedControls);
    xml->get_widget("txvTuneInfo", txvTuneInfo);


    /*Channel mutes*/
    xml->get_widget("chbMuteChan0", chbChannelMutes[0]);
    xml->get_widget("chbMuteChan1", chbChannelMutes[1]);
    xml->get_widget("chbMuteChan2", chbChannelMutes[2]);
    xml->get_widget("chbMuteChan3", chbChannelMutes[3]);
    xml->get_widget("chbMuteChan4", chbChannelMutes[4]);
    xml->get_widget("chbMuteChan5", chbChannelMutes[5]);
    xml->get_widget("chbMuteChan6", chbChannelMutes[6]);
    xml->get_widget("chbMuteChan7", chbChannelMutes[7]);

    /*Repeat*/
    xml->get_widget("mniRepNothing", mniRepNothing);
    xml->get_widget("mniRepSubsong", mniRepSubsong);
    xml->get_widget("mniRepTune", mniRepTune);
    xml->get_widget("mniRepPlaylist", mniRepPlaylist);


    /*About*/
    xml->get_widget("dlgAbout", dlgAbout);
    xml->get_widget("mniAbout", mniAbout);
    xml->get_widget("lblVersion", lblVersion);

    /*Quit*/
    xml->get_widget("mniQuit", mniQuit);

    /*Preferences*/
    xml->get_widget("mniPreferences", mniPreferences);
    xml->get_widget("dlgPreferences", dlgPreferences);
    xml->get_widget("chbUseDefaultDirectory", chbUseDefaultDirectory);
    xml->get_widget("entDefaultDirectory", entDefaultDirectory);
    xml->get_widget("hscSilenceLimit", hscSilenceLimit);
    xml->get_widget("chbAlwaysFirstSubsong", chbAlwaysFirstSubsong);
    xml->get_widget("spbVisualizationPeriod", spbVisualizationPeriod);
    xml->get_widget("spbVisualizationDecrement", spbVisualizationDecrement);
    xml->get_widget("chbAdvancedControlsShownByDefault", chbAdvancedControlsShownByDefault);
    xml->get_widget("hscVisualizationSynchro", hscVisualizationSynchro);
    xml->get_widget("entAsmaDirectory", entAsmaDirectory);
    xml->get_widget("entStilFile", entStilFile);
    xml->get_widget("chbUseStil", chbUseStil);
    xml->get_widget("btnBrowseASMADir", btnBrowseASMADir);
    xml->get_widget("btnBrowseSTIL", btnBrowseSTIL);
    xml->get_widget("entAlsaDevice", entAlsaDevice);
    xml->get_widget("chbAlwaysStereo", chbAlwaysStereo);
    xml->get_widget("chbNormalizeSAPHeader", chbNormalizeSAPHeader);


    /*Adjust visualization configuration controls*/
    spbVisualizationDecrement->set_increments(1.0d, 2.0d);
    spbVisualizationPeriod->set_increments(1.0d, 2.0d);
    spbVisualizationPeriod->get_adjustment()->set_lower(10.0d);

    /*Visualization*/
    xml->get_widget("prbChan1", prbChans[0]);
    xml->get_widget("prbChan2", prbChans[1]);
    xml->get_widget("prbChan3", prbChans[2]);
    xml->get_widget("prbChan4", prbChans[3]);
    xml->get_widget("prbChan5", prbChans[4]);
    xml->get_widget("prbChan6", prbChans[5]);
    xml->get_widget("prbChan7", prbChans[6]);
    xml->get_widget("prbChan8", prbChans[7]);

    /*Playlist*/

    xml->get_widget("trvPlaylist", trvPlaylist);
    xml->get_widget("mniAddFileToPlaylist", mniAddFileToPlaylist);
    xml->get_widget("mniAddDirectoryToPlaylist", mniAddDirectoryToPlaylist);
    xml->get_widget("fcdAddToPlaylist", fcdAddToPlaylist);
    xml->get_widget("mniSavePlaylist", mniSavePlaylist);
    xml->get_widget("mniLoadPlaylist", mniLoadPlaylist);
    xml->get_widget("fcdPlaylist", fcdPlaylist);
    xml->get_widget("mniPlaylistSelectNone", mniPlaylistSelectNone);
    xml->get_widget("mniPlaylistSelectAll", mniPlaylistSelectAll);
    xml->get_widget("mniPlaylistSelectInvert", mniPlaylistSelectInvert);
    xml->get_widget("mniAddPlayerFile", mniAddPlayerFile);
    xml->get_widget("mniPlaylistPurge", mniPlaylistPurge);
    xml->get_widget("mniPlaylistTrimSelected", mniPlaylistTrimSelected);
    xml->get_widget("mniPlaylistRemoveAll", mniPlaylistRemoveAll);
    xml->get_widget("mniPlaylistRemoveSelected", mniPlaylistRemoveSelected);
    xml->get_widget("vbxPlaylist", vbxPlaylist);


    std::list<Gtk::TargetEntry> playlistTgtList;
//    vbxPlaylist->drag_dest_set(playlistTgtList, Gtk::DEST_DEFAULT_ALL, Gdk::ACTION_DEFAULT | Gdk::ACTION_MOVE | Gdk::ACTION_COPY);
    vbxPlaylist->drag_dest_add_uri_targets();
    vbxPlaylist->drag_dest_add_text_targets();

}

void GUI::connectSignals() {
    btnPlayOrPause->signal_clicked().connect(sigc::ptr_fun(&on_playOrPause), true);

    mniLoadSAP->signal_activate().connect(sigc::ptr_fun(&on_loadSAP), true);
    btnStop->signal_clicked().connect(sigc::ptr_fun(&on_stop), true);
    btnPrevious->signal_clicked().connect(sigc::ptr_fun(&on_previous), true);
    btnNext->signal_clicked().connect(sigc::ptr_fun(&on_next), true);
    vbxPlayerBox->signal_drag_data_received().connect(sigc::ptr_fun(&on_playerDrop), true);
    hscSeekingConnection = hscSeeking->signal_value_changed().connect(sigc::ptr_fun(&on_seek), true);
    hscSeeking->signal_button_press_event().connect(sigc::mem_fun(*this, &GUI::on_hscSeekingButtonPressed), false);
    hscSeeking->signal_button_release_event().connect(sigc::mem_fun(*this, &GUI::on_hscSeekingButtonReleased), false);
    tgbAdvancedControls->signal_toggled().connect(sigc::ptr_fun(&on_advancedControlsToggled), true);
    btnNextTune->signal_clicked().connect(sigc::ptr_fun(&on_nextTune), true);
    btnPreviousTune->signal_clicked().connect(sigc::ptr_fun(&on_previousTune), true);

    /*Browse and play*/
    mniBrowseAndPlay->signal_activate().connect(sigc::ptr_fun(&on_browseAndPlay), true);
    fcdBrowseAndPlay->signal_file_activated().connect(sigc::ptr_fun(&on_fcdBrowseAndPlayFileActivated), true);
    fcdBrowseAndPlay->signal_delete_event().connect(sigc::ptr_fun(&on_fcdBrowseAndPlayClose), true);
    btnBrowseAndPlayClose->signal_clicked().connect(sigc::ptr_fun(&on_browseAndPlayClose), true);



    /*Repeats*/
    mniRepNothing->signal_activate().connect(sigc::ptr_fun(&on_handleRepeatMode), true);
    mniRepSubsong->signal_activate().connect(sigc::ptr_fun(&on_handleRepeatMode), true);
    mniRepTune->signal_activate().connect(sigc::ptr_fun(&on_handleRepeatMode), true);
    mniRepPlaylist->signal_activate().connect(sigc::ptr_fun(&on_handleRepeatMode), true);


    /*About*/
    mniAbout->signal_activate().connect(sigc::ptr_fun(&on_aboutInvoke), true);
    dlgAbout->signal_delete_event().connect(sigc::ptr_fun(&on_aboutClose), true);

    /*Quit*/
    mniQuit->signal_activate().connect(sigc::ptr_fun(&on_quitInvoke), true);
    wndMain->signal_delete_event().connect(sigc::ptr_fun(&on_wndMainClose), true);

    /*Preferences*/
    mniPreferences->signal_activate().connect(sigc::ptr_fun(&on_preferencesInvoke), true);
    dlgPreferences->signal_delete_event().connect(sigc::ptr_fun(&on_preferencesClose), true);
    btnBrowseASMADir->signal_clicked().connect(sigc::mem_fun(*this, &GUI::on_btnBrowseASMADir), true);
    btnBrowseSTIL->signal_clicked().connect(sigc::mem_fun(*this, &GUI::on_btnBrowseSTIL), true);

    /*Channel mutes*/
    for (int i = 0; i < 8; i++) {
        chbChannelMutes[i]->signal_toggled().connect(sigc::ptr_fun(&on_channelMuteChange), true);
    }


    /*Playlist*/
    mniAddFileToPlaylist->signal_activate().connect(sigc::ptr_fun(&on_addFileToPlaylist), true);
    mniAddDirectoryToPlaylist->signal_activate().connect(sigc::ptr_fun(&on_addDirectoryToPlaylist), true);
    trvPlaylist->signal_row_activated().connect(sigc::ptr_fun(&on_playlistItemActivated), true);
    mniSavePlaylist->signal_activate().connect(sigc::ptr_fun(&on_savePlaylist), true);
    mniLoadPlaylist->signal_activate().connect(sigc::ptr_fun(&on_loadPlaylist), true);
    mniAddPlayerFile->signal_activate().connect(sigc::ptr_fun(&on_addPlayerFile), true);

    mniPlaylistSelectAll->signal_activate().connect(sigc::mem_fun(*this, &GUI::on_playlistSelectAll), true);
    mniPlaylistSelectNone->signal_activate().connect(sigc::mem_fun(*this, &GUI::on_playlistSelectNone), true);
    mniPlaylistSelectInvert->signal_activate().connect(sigc::mem_fun(*this, &GUI::on_playlistSelectInvert), true);


    mniPlaylistRemoveAll->signal_activate().connect(sigc::ptr_fun(&on_removeAllFromPlaylist), true);
    mniPlaylistRemoveSelected->signal_activate().connect(sigc::ptr_fun(&on_removeSelectedFromPlaylist), true);
    mniPlaylistTrimSelected->signal_activate().connect(sigc::ptr_fun(&on_trimPlaylistSelection), true);
    mniPlaylistPurge->signal_activate().connect(sigc::ptr_fun(&on_purgePlaylist), true);

    vbxPlaylist->signal_drag_data_received().connect(sigc::ptr_fun(&on_playlistDrop), true);
}

void GUI::updateAfterSAPLoad(Glib::ustring filename, int songNumber, int songCount, int duration, Glib::ustring info) {

    /*Update file name label*/
    lblFilename->set_text(filename);

    /*Update tune info and tip*/
    txvTuneInfo->get_buffer()->set_text(info);
    //toolTips.set_tip(*lblFilename, info);


    /*Update counters*/
    updateAfterSubsongChange(songNumber, songCount, duration);


}

void GUI::updateAfterSubsongChange(int songNumber, int songCount, int duration) {

    if (duration > 0) {
        hscSeeking->set_range(0, duration);
        hscSeeking->set_sensitive(true);
    } else {
        hscSeeking->set_sensitive(false);
    }

    /*Update song counter*/
    char buf[16];
    memset(buf, 0, 16);
    snprintf(buf, 10, "%02d/%02d", songNumber + 1, songCount);
    lblSongCounter->set_text(buf);

    /*Reset seek scale*/
    resetSeekScale();
}

void GUI::updateTick(int minutes, int seconds, int totalMiliseconds) {

    /*Elapsed time mm:ss*/
    char buf[16];
    memset(buf, 0, 16);
    snprintf(buf, 10, "%02d:%02d", minutes, seconds);
    lblElapsedTime->set_text(buf);

    /*Elapsed time into HScale, if possible*/

    if (tickUpdateBlocked == false && totalMiliseconds > 0) {
        hscSeekingConnection.disconnect();
        hscSeeking->set_value((double) totalMiliseconds);
        hscSeekingConnection = hscSeeking->signal_value_changed().connect(sigc::ptr_fun(&on_seek), true);
    }

}

void GUI::resetSeekScale() {
    hscSeeking->set_value(0);
}

void GUI::resetVisualization() {
    for (int i = 0; i < 8; i++) {
        prbChans[i]->set_fraction(0.0f);
    }
}

void GUI::resetPlayer() {
    resetSeekScale();
    lblElapsedTime->set_text("");
    lblSongCounter->set_text("");
    lblFilename->set_text("");
    resetVisualization();
}

void GUI::runMessageDialog(Glib::ustring title, Glib::ustring mainText, Glib::ustring text) {
    Gtk::MessageDialog msg(mainText, false, Gtk::MESSAGE_WARNING);
    msg.set_modal(true);
    msg.set_secondary_text(text);
    msg.set_title(title);
    msg.set_transient_for(*wndMain);
    msg.run();
    msg.hide();
}

void GUI::updatePreferencesChanged(Preferences* prefs, bool startup) {

    /*Repeat mode*/
    switch (prefs->getRepeatMode()) {
        case REPEAT_NO_PROGRESS:
        {
            mniRepNothing->set_active(true);
            break;
        }
        case REPEAT_SUBSONG:
        {
            mniRepSubsong->set_active(true);
            break;
        }
        case REPEAT_TUNE:
        {
            mniRepTune->set_active(true);
            break;
        }
        case REPEAT_PLAYLIST:
        {
            mniRepPlaylist->set_active(true);
            break;
        }
    }
    /*Silence detector*/
    hscSilenceLimit->set_value((double) prefs->getSilenceLimit());

    /*Default directory*/
    bool b = prefs->getUseDefaultDirectory();
    chbUseDefaultDirectory->set_active(b);
    entDefaultDirectory->set_text(prefs->getDefaultDirectory());

    if (startup == true && b == true) {

        /*Setup directories that exist and are directories*/

        /*Tune loading dialog*/
        if (g_file_test(prefs->getDefaultDirectory().c_str(), G_FILE_TEST_IS_DIR) == TRUE) {
            fcdLoad->set_current_folder(prefs->getDefaultDirectory());
            fcdAddToPlaylist->set_current_folder(prefs->getDefaultDirectory());
            fcdBrowseAndPlay->set_current_folder(prefs->getDefaultDirectory());
        }
    }

    /*Always first subsong*/
    b = prefs->getAlwaysFirstSubsong();
    chbAlwaysFirstSubsong->set_active(b);

    /*Normalize SAP header*/
    b = prefs->getNormalizeSAPHeader();
    chbNormalizeSAPHeader->set_active(b);

    /*Visualization period*/
    spbVisualizationPeriod->set_value((double) prefs->getVisualizationPeriod());

    /*Visualization synchro*/
    hscVisualizationSynchro->set_value((double) prefs->getVisualizationSynchro());

    /*Visualization decrement*/
    spbVisualizationDecrement->set_value((double) prefs->getVisualizationDecrement());

    /*Advanced controls*/
    if (startup == true) {
        if (prefs->getAdvancedControlsShownByDefault() == true) {
            tgbAdvancedControls->set_active(true);
        }
    }
    chbAdvancedControlsShownByDefault->set_active(prefs->getAdvancedControlsShownByDefault());

    /*ASMA support*/
    entAsmaDirectory->set_text(prefs->getAsmaDirectory());
    entStilFile->set_text(prefs->getStilFile());
    chbUseStil->set_active(prefs->getUseStilFile());

    /*ALSA*/
    entAlsaDevice->set_text(prefs->getAlsaDevice());
    chbAlwaysStereo->set_active(prefs->getAlwaysStereo());


}

void GUI::flushPreferences(Preferences* prefs) {
    /*Silence detector*/
    prefs->setSilenceLimit((int) hscSilenceLimit->get_value());
    /*Default directory*/
    prefs->setUseDefaultDirectory(chbUseDefaultDirectory->get_active());
    prefs->setDefaultDirectory(entDefaultDirectory->get_text());
    /*Always first subsong*/
    prefs->setAlwaysFirstSubsong(chbAlwaysFirstSubsong->get_active());
    /*Normalize SAP header*/
    prefs->setNormalizeSAPHeader(chbNormalizeSAPHeader->get_active());
    /*Visualization period*/
    prefs->setVisualizationPeriod(spbVisualizationPeriod->get_value_as_int());
    /*Visualization synchro*/
    prefs->setVisualizationSynchro((int) hscVisualizationSynchro->get_value());
    /*Visualization decrement*/
    prefs->setVisualizationDecrement(spbVisualizationDecrement->get_value_as_int());

    /*Advanced controls shown by default*/
    prefs->setAdvancedControlsShownByDefault(chbAdvancedControlsShownByDefault->get_active());

    /*ASMA*/
    prefs->setAsmaDirectory(entAsmaDirectory->get_text());
    prefs->setStilFile(entStilFile->get_text());
    prefs->setUseStilFile(chbUseStil->get_active());

    /*ALSA*/
    prefs->setAlsaDevice(entAlsaDevice->get_text());
    prefs->setAlwaysStereo(chbAlwaysStereo->get_active());

}

void GUI::updateVisualization(int* volumes) {

    double d = 0;

    for (int i = 0; i < 8; i++) {
        d = ((double) *(volumes + i)) / 256;
        prbChans[i]->set_fraction(d);
    }
}

void GUI::setVersionString(const char* versionString, const char* asapVersionString) {
    lblVersion->set_label(Glib::ustring(versionString) + " ASAP: " + Glib::ustring(asapVersionString));
}

bool GUI::on_hscSeekingButtonPressed(GdkEventButton* btn) {
    tickUpdateBlocked = true;
    return false;
}

bool GUI::on_hscSeekingButtonReleased(GdkEventButton* btn) {
    tickUpdateBlocked = false;
    return false;
}

void GUI::on_playlistSelectAll() {
    trvPlaylist->get_selection()->select_all();
}

void GUI::on_playlistSelectNone() {
    trvPlaylist->get_selection()->unselect_all();
}

void GUI::on_playlistSelectInvert() {
    Gtk::TreeModel::Children ch = trvPlaylist->get_model()->children();
    Gtk::TreeModel::iterator iter = ch.begin();

    Glib::RefPtr<Gtk::TreeSelection> selection = trvPlaylist->get_selection();

    while (iter != ch.end()) {
        if (selection->is_selected(*iter) == true) {
            selection->unselect(*iter);
        } else {
            selection->select(*iter);
        }
        iter++;
    }
}

void GUI::on_btnBrowseASMADir() {
    int k = 0;
    Gtk::FileChooserDialog asmaFcd("ASMA directory", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);

    asmaFcd.select_filename(entAsmaDirectory->get_text());

    asmaFcd.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    asmaFcd.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    k = asmaFcd.run();

    if (k == Gtk::RESPONSE_OK) {
        entAsmaDirectory->set_text(asmaFcd.get_filename());
    }


}

void GUI::on_btnBrowseSTIL() {
    int k = 0;
    Gtk::FileChooserDialog stilFcd("STIL FILE", Gtk::FILE_CHOOSER_ACTION_OPEN);

    stilFcd.select_filename(entStilFile->get_text());

    stilFcd.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    stilFcd.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    k = stilFcd.run();

    if (k == Gtk::RESPONSE_OK) {
        entStilFile->set_text(stilFcd.get_filename());
    }

}

void GUI::setupPlaylistView(Gtk::TreeModelColumn<Glib::ustring> *tmc0, Glib::RefPtr<Gtk::ListStore> model) {

    static Gtk::TreeViewColumn tvc1("Markup",*tmc0); /*Markup*/
    tvc1.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
    trvPlaylist->append_column(tvc1);
    
    /*Text cell renderer*/
    Gtk::CellRenderer* cr = dynamic_cast<Gtk::CellRendererText*>(trvPlaylist->get_column_cell_renderer(0));
    tvc1.clear_attributes(*cr);
    tvc1.add_attribute(*cr,Glib::ustring("markup"),0); /*Markup taken from MODEL column 1*/
    
    trvPlaylist->set_model(model);
    
    trvPlaylist->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);

}


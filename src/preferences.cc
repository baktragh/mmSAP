#include "preferences.h"

Preferences::Preferences() {

    keyFile = NULL;

    silenceLimit = DEFAULT_SILENCE;
    repeatMode = REPEAT_NO_PROGRESS;
    useDefaultDirectory = false;
    defaultDirectory = ".";
    alwaysFirstSubsong = false;
    visualizationPeriod = 20;
    mainWindowHeight = -1;
    advancedControlsShownByDefault = false;
    visualizationSynchro = 0;
    lastPlaylist = "";
    asmaDirectory = "";
    stilFile = "";
    normalizeSAPHeader = false;
    visualizationDecrement = 10;
    lastLoadDirectory="";
    lastBrowseAndPlayDirectory="";
    lastPlaylistDirectory="";
    lastAddToPlaylistDirectory="";
}

/* Setters and getters*/

int Preferences::getSilenceLimit() {
    return silenceLimit;
}

int Preferences::getRepeatMode() {
    return repeatMode;
}

bool Preferences::getUseDefaultDirectory() {
    return useDefaultDirectory;
}

Glib::ustring Preferences::getDefaultDirectory() {
    return defaultDirectory;
}

bool Preferences::getAlwaysFirstSubsong() {
    return alwaysFirstSubsong;
}

int Preferences::getVisualizationPeriod() {
    return visualizationPeriod;
}

int Preferences::getMainWindowHeight() {
    return mainWindowHeight;
}

bool Preferences::getAdvancedControlsShownByDefault() {
    return advancedControlsShownByDefault;
}

int Preferences::getVisualizationSynchro() {
    return visualizationSynchro;
}

Glib::ustring Preferences::getLastPlaylist() {
    return lastPlaylist;
}

Glib::ustring Preferences::getAsmaDirectory() {
    return asmaDirectory;
}

Glib::ustring Preferences::getStilFile() {
    return stilFile;
}

bool Preferences::getUseStilFile() {
    return useStilFile;
}

bool Preferences::getNormalizeSAPHeader() {
    return normalizeSAPHeader;
}

int Preferences::getVisualizationDecrement() {
    return visualizationDecrement;
}

void Preferences::setRepeatMode(int mode) {
    repeatMode = mode;
}

void Preferences::setUseDefaultDirectory(bool use) {
    useDefaultDirectory = use;
}

void Preferences::setDefaultDirectory(Glib::ustring dir) {
    defaultDirectory = dir;
}

void Preferences::setSilenceLimit(int limit) {
    silenceLimit = limit;
}

void Preferences::setAlwaysFirstSubsong(bool b) {
    alwaysFirstSubsong = b;
}

void Preferences::setVisualizationPeriod(int period) {
    visualizationPeriod = period;
}

void Preferences::setMainWindowHeight(int height) {
    mainWindowHeight = height;
}

void Preferences::setAdvancedControlsShownByDefault(bool b) {
    advancedControlsShownByDefault = b;
}

void Preferences::setVisualizationSynchro(int sync) {
    visualizationSynchro = sync;
}

void Preferences::setLastPlaylist(Glib::ustring fspec) {
    lastPlaylist = fspec;
}

void Preferences::setAsmaDirectory(Glib::ustring fspec) {
    asmaDirectory = fspec;
}

void Preferences::setStilFile(Glib::ustring fspec) {
    stilFile = fspec;
}

void Preferences::setUseStilFile(bool b) {
    useStilFile = b;
}


void Preferences::setNormalizeSAPHeader(bool b) {
    normalizeSAPHeader = b;
}

void Preferences::setVisualizationDecrement(int d) {
    visualizationDecrement = d;
}

void Preferences::setLastLoadDirectory(Glib::ustring fspec) {
    lastLoadDirectory=fspec;
}

void Preferences::setLastBrowseAndPlayDirectory(Glib::ustring fspec) {
    lastBrowseAndPlayDirectory=fspec;
}

void Preferences::setLastPlaylistDirectory(Glib::ustring fspec) {
    lastPlaylistDirectory=fspec;

}

void Preferences::setLastAddToPlaylistDirectory(Glib::ustring fspec) {
    lastAddToPlaylistDirectory=fspec;

}

Glib::ustring Preferences::getLastLoadDirectory() {
    return lastLoadDirectory;
}

Glib::ustring Preferences::getLastBrowseAndPlayDirectory() {
    return lastBrowseAndPlayDirectory;
}

Glib::ustring Preferences::getLastPlaylistDirectory() {
    return lastPlaylistDirectory;
}

Glib::ustring Preferences::getLastAddToPlaylistDirectory() {
    return lastAddToPlaylistDirectory;
}


/* KeyFile handling *********************************************************/

int Preferences::getIntProperty(const char* key, int dflt, int min, int max) {
    GError* e = NULL;

    int r = g_key_file_get_integer(keyFile, "mmsap", key, &e);
    if (e != NULL) {
        g_error_free(e);
        e = NULL;
        r = dflt;
        return r;
    }

    if (r < min || r > max) r = dflt;
    return r;
}

Glib::ustring Preferences::getStringProperty(const char* key, const char* dflt) {
    GError* e = NULL;
    gchar* r = g_key_file_get_string(keyFile, "mmsap", key, &e);
    if (e != NULL) {
        g_error_free(e);
        e = NULL;
        return Glib::ustring(dflt);
    }
    return Glib::ustring(r);
}

bool Preferences::getBooleanProperty(const char* key, bool dflt) {
    GError* e = NULL;
    gboolean val = g_key_file_get_boolean(keyFile, "mmsap", key, &e);
    if (e != NULL) {
        g_error_free(e);
        e = NULL;
        return dflt;
    }
    return (val == TRUE) ? true : false;
}

void Preferences::load() {

    /*Config file specifier*/
    Glib::ustring fspec(g_get_user_config_dir());
    fspec += "/.mmsap";

#ifdef DEBUG_PRINTOUTS
    printf("Preferences::load() from %s\n", fspec.c_str());
#endif


    /*GKeyFile object initialization*/
    if (keyFile == NULL) {
        keyFile = g_key_file_new();
    } else {
        g_key_file_free(keyFile);
        keyFile = g_key_file_new();
    }

    if (keyFile == NULL) return;

    /*Try to load*/
    g_key_file_load_from_file(keyFile, fspec.c_str(), G_KEY_FILE_KEEP_COMMENTS, NULL);
    if (keyFile == NULL) return;

    /*Retrieve specific items and set to default when failed*/
    repeatMode = getIntProperty("repeat_mode", REPEAT_NO_PROGRESS, REPEAT_NO_PROGRESS, REPEAT_PLAYLIST);
    silenceLimit = getIntProperty("silence_limit", 5, 0, 15);
    useDefaultDirectory = getBooleanProperty("use_default_directory", false);
    defaultDirectory = getStringProperty("default_directory", ".");
    alwaysFirstSubsong = getBooleanProperty("always_first_subsong", false);
    visualizationPeriod = getIntProperty("visualization_period", 20, 5, 200);
    mainWindowHeight = getIntProperty("main_window_height", -1, 20, 3200);
    advancedControlsShownByDefault = getBooleanProperty("advanced_controls_shown_by_default", false);
    visualizationSynchro = getIntProperty("visualization_synchro", 0, 0, 15);
    lastPlaylist = getStringProperty("last_playlist", "");
    asmaDirectory = getStringProperty("asma_directory", "");
    stilFile = getStringProperty("stil_file", "");
    useStilFile = getBooleanProperty("use_stil_file", false);
    normalizeSAPHeader = getBooleanProperty("normalize_sap_header", false);
    visualizationDecrement = getIntProperty("visualization_decrement", 10, 5, 100);

    lastLoadDirectory = getStringProperty("last_dir_load","");
    lastBrowseAndPlayDirectory = getStringProperty("last_dir_browse_and_play","");
    lastPlaylistDirectory = getStringProperty("last_dir_playlist","");
    lastAddToPlaylistDirectory = getStringProperty("last_dir_add_to_playlist","");
    
}

void Preferences::save() {

#ifdef DEBUG_PRINTOUTS
    printf("Preferences::save()\n");
#endif

    if (keyFile == NULL) return;

    /*Specific items*/
    g_key_file_set_integer(keyFile, "mmsap", "repeat_mode", repeatMode);
    g_key_file_set_integer(keyFile, "mmsap", "silence_limit", silenceLimit);
    g_key_file_set_boolean(keyFile, "mmsap", "use_default_directory", useDefaultDirectory ? 1 : 0);
    g_key_file_set_string(keyFile, "mmsap", "default_directory", defaultDirectory.c_str());
    g_key_file_set_boolean(keyFile, "mmsap", "always_first_subsong", alwaysFirstSubsong ? 1 : 0);
    g_key_file_set_integer(keyFile, "mmsap", "visualization_period", visualizationPeriod);
    g_key_file_set_integer(keyFile, "mmsap", "main_window_height", mainWindowHeight);
    g_key_file_set_boolean(keyFile, "mmsap", "advanced_controls_shown_by_default", advancedControlsShownByDefault ? 1 : 0);
    g_key_file_set_integer(keyFile, "mmsap", "visualization_synchro", visualizationSynchro);
    g_key_file_set_string(keyFile, "mmsap", "last_playlist", lastPlaylist.c_str());
    g_key_file_set_string(keyFile, "mmsap", "asma_directory", asmaDirectory.c_str());
    g_key_file_set_string(keyFile, "mmsap", "stil_file", stilFile.c_str());
    g_key_file_set_boolean(keyFile, "mmsap", "use_stil_file", useStilFile ? 1 : 0);
    g_key_file_set_boolean(keyFile, "mmsap", "normalize_sap_header", normalizeSAPHeader ? 1 : 0);
    g_key_file_set_integer(keyFile, "mmsap", "visualization_decrement", visualizationDecrement);
    g_key_file_set_string(keyFile,"mmsap","last_dir_load",lastLoadDirectory.c_str());
    g_key_file_set_string(keyFile,"mmsap","last_dir_browse_and_play",lastBrowseAndPlayDirectory.c_str());
    g_key_file_set_string(keyFile,"mmsap","last_dir_playlist",lastPlaylistDirectory.c_str());
    g_key_file_set_string(keyFile,"mmsap","last_dir_add_to_playlist",lastAddToPlaylistDirectory.c_str());

    /*Config file specifier*/
    Glib::ustring fspec(g_get_user_config_dir());
    fspec += "/.mmsap";


    /*Saving*/
    gsize len = 0;
    GError *e = NULL;
    const char* ptr = g_key_file_to_data(keyFile, &len, &e);
    if (e != NULL) return;

    GIOChannel* f;
    e = NULL;
    f = g_io_channel_new_file(fspec.c_str(), "w", &e);

    if (f != NULL) {
        gsize count = 0;
        g_io_channel_write_chars(f, ptr, len, &count, &e);
        g_io_channel_shutdown(f, TRUE, NULL);
        g_io_channel_unref(f);
    }

    free((void*) ptr);



}

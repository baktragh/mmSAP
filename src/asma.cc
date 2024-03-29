#include "asma.h"
#include "platform.h"

Asma::Asma() {

    enabled = false;
    directory = "";
    stilFile = "";
}

void Asma::reinitialize(Glib::ustring asmaDir, Glib::ustring asmaStil,bool asmaEnabled) {

    if (asmaEnabled==false) {
        enabled=false;
        return;
    }

    /*If no change in the settings, just do nothing*/
    if (directory==asmaDir && stilFile==asmaStil && enabled==asmaEnabled) {
        enabled=true;
        return;
    }

    /*STIL not available until proven otherwise*/
    enabled = false;

    /*Check existence of asma directory and files*/
    gboolean b;

#ifdef DEBUG_PRINTOUTS
    printf("Asma::reinitialize() testing: |%s|,|%s|\n", dir.c_str(), stil.c_str());
#endif

    b = Glib::file_test(asmaDir, Glib::FILE_TEST_IS_DIR);
    if (b == false) return;

#ifdef DEBUG_PRINTOUTS
    printf("Asma::initialize() ASMA directory exist\n");
#endif

    b = Glib::file_test(asmaStil, Glib::FILE_TEST_IS_REGULAR);
    if (b == false) return;

#ifdef DEBUG_PRINTOUTS
    printf("Asma::initialize() ASMA STIL file exist\n");
#endif


    /*These files exist, create hashtable*/
    directory = asmaDir;
    stilFile = asmaStil;
    table.clear();
    stilPaths.clear();
    stilTexts.clear();

    parse();

}

void Asma::parse() {

#ifdef DEBUG_PRINTOUTS
    printf("Asma::parse()\n");
#endif

    char buf[512];
    Glib::ustring line;

    Glib::ustring tuneText = "";
    Glib::ustring tunePath = "";


    FILE* f = fopen(stilFile.c_str(), "rt");
    if (f == NULL) {
#ifdef DEBUG_PRINTOUTS
        printf("Asma::parse() |%s| not opened\n", stilFile.c_str());
#endif
        return;
    }

    while (fgets(buf, 510, f) != NULL) {

        line = Glib::ustring(g_strstrip((gchar*) buf));

#ifdef DEBUG_PRINTOUTS
        //printf("%s\n",line.c_str());
#endif

        /*Ignore blank lines*/
        if (line.length() < 1) continue;
        /*Ignore comments*/
        if (line[0] == '#') continue;

        /*Tune path !, start new tune*/
        if (line[0] == '/') {
            /*Store previous if available*/
            addEntry(tunePath, tuneText);
            /*Start new*/
            tunePath = line;
            tuneText = "";
            continue;
        }
        /*Ordinary line, add to tuneText also with newline*/
        if (tuneText.length() > 1024) tuneText = "<?1024?>";

        /*Tag beginning ?*/
        if (line.find_first_of("ARTIST:") == 0 || line.find_first_of("TITLE:") == 0
                || line.find_first_of("COMMENT:") == 0 || line.find_first_of("(#") == 0) {
            tuneText += '\n';
            tuneText += line;
        }            /*Inside tag or trailing*/
        else {
            tuneText += ' ';
            tuneText += line;

        }

    }
    fclose(f);

    /*Any trailing*/
    addEntry(tunePath, tuneText);

    enabled=true;

}

void Asma::addEntry(Glib::ustring path, Glib::ustring text) {

    if (path != "" && text != "") {

#ifdef DEBUG_PRINTOUTS
        printf("ASMA: Added entry: |%s|\n%s\n", path.c_str(), text.c_str());
#endif
        table[path] = text;
    }
}

Glib::ustring Asma::getEntry(Glib::ustring fspec) {

#ifdef DEBUG_PRINTOUTS
    printf("Asma::getEntry|%s|\n", fspec.c_str());
#endif

    if (enabled==false) return Glib::ustring("");

    Glib::ustring retVal = "";
    if (fspec.length() < 1) return retVal;

    /*Try to find asma directory at the beginning of the filespec*/
    Glib::ustring upperSpec = fspec.uppercase();
    Glib::ustring upperAsmaDir = directory.uppercase();

    int k = upperSpec.find(upperAsmaDir);
    if (k != 0) return retVal;

    /*The filespec now starts with asma, so we should remove the beginning chars*/
    Glib::ustring rawKey = fspec;
    rawKey = rawKey.erase(0, directory.length());
    Glib::ustring key;

    /*Reverse slashes*/
    k = rawKey.length();
    for (int i = 0; i < k; i++) {
        if (rawKey.at(i) == G_DIR_SEPARATOR) key += '/';
        else key += rawKey.at(i);
    }

#ifdef DEBUG_PRINTOUTS
    printf("ASMA::getEntry() key: |%s|\n", key.c_str());
#endif

    /*Look for subkeys*/
    int zl = key.length();
    int xr = zl - 1;
    gchar* entry;

    while (xr > 0) {

        if (key[xr] != '/') {

        } else {


            if (table.find(key.substr(0, xr + 1)) != table.end()) {
                retVal += table[key.substr(0, xr + 1)];
                retVal += "\n";
            }



        }
        xr--;
    }

    /*Look for main key*/
    if (table.find(key) != table.end()) {
        retVal.insert(0, Glib::ustring("\n"));
        retVal.insert(0, Glib::ustring(table[key]));
    }

#ifdef DEBUG_PRINTOUTS
    printf("ASMA::getEntry() retVal: %s \nfor %s\n", retVal.c_str(), key.c_str());
#endif

    return retVal;


}

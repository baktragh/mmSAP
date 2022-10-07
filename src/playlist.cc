#include "playlist.h"

Playlist::Playlist() {
#ifdef DEBUG_PRINTOUTS
    printf("Playlist::Playlist()\n");
#endif
    lstore = Gtk::ListStore::create(columns);
}

bool Playlist::addFile(Glib::ustring fspec) {

#ifdef DEBUG_PRINTOUTS
    printf("Playlist::addFile(%s)\n", fspec.c_str());
#endif

    bool ourFile = ASAPInfo_IsOurFile(fspec.c_str());
    if (ourFile == FALSE) return false;

    Gtk::TreeModel::Row row=*(lstore->append());
    row.set_value(columns.clmFilespec,fspec);
    row.set_value(columns.clmFilename,Glib::ustring(Glib::path_get_basename(fspec)));
    row.set_value(columns.clmCurrent,false);
    row.set_value(columns.clmMarkup,Glib::ustring(Glib::path_get_basename(fspec)));

    return true;
}

bool Playlist::addDirectory(Glib::ustring dirspec) {

#ifdef DEBUG_PRINTOUTS
    printf("Playlist::addDirectory(%s)\n", dirspec.c_str());
#endif

    /*Open directory for reading*/
    GDir* d = g_dir_open(dirspec.c_str(), 0, NULL);
    if (d == NULL) return false;

    /*Scan directory items*/
    const gchar* fn;
    gboolean b;
    Glib::ustring fspec;
    /*Must be existing regular file*/
    while ((fn = g_dir_read_name(d)) != NULL) {
        fspec = dirspec + '\\' + fn;
        b = g_file_test(fspec.c_str(), (GFileTest) (G_FILE_TEST_IS_REGULAR));
        if (b == TRUE) {
            addFile(fspec);
        }
    }
    /*Close directory*/
    g_dir_close(d);
    return true;
}

bool Playlist::addMultipleFilespecs(std::vector<Glib::ustring> *specs) {
    int l = specs->size();
    Glib::ustring spec;
    gboolean b;

    for (int i = 0; i < l; i++) {
        /*File or directory ?*/
        spec = (*specs)[i];
        b = g_file_test(spec.c_str(), (GFileTest) (G_FILE_TEST_IS_REGULAR));
        if (b == TRUE) {
            addFile(spec);
            continue;
        }

        b = g_file_test(spec.c_str(), (GFileTest) (G_FILE_TEST_IS_DIR));
        if (b == TRUE) {
            addDirectory(spec);
        }

    }
    
    return true;
}

void Playlist::clearCurrentColumn() {
    Gtk::TreeModel::Children children = lstore->children();
    Gtk::TreeModel::iterator iter = children.begin();
    Gtk::TreeModel::Row row;

    while (iter != children.end()) {
        row = *iter;
        setRowActive(row,false);
        iter++;
    }
}

int Playlist::save(Glib::ustring fspec) {

    //#ifdef DEBUG_PRINTOUTS
    printf("Playlist::save(%s)\n", fspec.c_str());
    //#endif

    Gtk::TreeModel::Children children = lstore->children();
    if (children.size() < 1) return PLAYLIST_SAVED_EMPTY;
    Gtk::TreeModel::iterator iter = children.begin();
    Gtk::TreeModel::Row row;

    FILE* f = fopen(fspec.c_str(), "wt");
    if (f == NULL) return PLAYLIST_SAVED_ERROR;

    while (iter != children.end()) {
        row = *iter;
        fprintf(f, "%s\n", row.get_value(columns.clmFilespec).c_str());
        iter++;
    }

    fclose(f);
    return PLAYLIST_SAVED_OK;

}

int Playlist::load(Glib::ustring fspec) {

#ifdef DEBUG_PRINTOUTS
    printf("Playlist::load(%s)\n", fspec.c_str());
#endif

    if (fspec == "") return -1;

    lstore->clear();
    char buf[256];
    FILE* f = fopen(fspec.c_str(), "rt");
    if (f == NULL) return -1;

    /*Basepath for relative paths*/
    Glib::ustring basepath = Glib::path_get_dirname(fspec);


    while (fgets(buf, 254, f) != NULL) {
        Glib::ustring a(g_strstrip((gchar*) buf));

        /*Not interested in blank strings or # comments*/
        if (a.length() < 1) continue;
        if (a[0] == '#') continue;

        /*Is path absolute or relative ?*/
        if (Glib::path_is_absolute(a) == true) {
            addFile(a);
        } else {
            addFile(basepath + '\\' + a);
        }
    }
    return 0;
}

bool Playlist::continueToNext(Glib::ustring* fspec) {

    Gtk::TreeModel::Children children = lstore->children();
    /*No next*/
    if (children.size() < 1) return false;


    Gtk::TreeModel::iterator iter = children.begin();
    Gtk::TreeModel::Row row;

    bool b;

    while (iter != children.end()) {
        row = *iter;
        b = row.get_value(columns.clmCurrent);
        /*We found item with 'current' column set*/
        if (b == true) {
            /*We clear the 'current' tag*/
            row.set_value(columns.clmCurrent, false);
            /*We will look for next row*/
            iter++;
            /*If not valid, we will revert it to first */
            if (!(lstore->iter_is_valid(iter) == true)) {
                iter = children.begin();
            }
            /*Appropriate row is retrieved, we will get its
             *filespec and we will set the 'current' column*/
            row = *iter;
            *fspec = row.get_value(columns.clmFilespec);
            row.set_value(columns.clmCurrent, true);
            return true;
        }
        iter++;
    }

    /*No row had 'current' tag, so we will revert to first*/
    iter = children.begin();
    row = *iter;
    *fspec = row.get_value(columns.clmFilespec);
    row.set_value(columns.clmCurrent, true);
    return true;
}

bool Playlist::continueToPrevious(Glib::ustring* fspec) {
    Gtk::TreeModel::Children children = lstore->children();
    /*No previous*/
    if (children.size() < 1) return false;

    Gtk::TreeModel::iterator iter = children.begin();
    Gtk::TreeModel::Row row;

    bool b;

    while (iter != children.end()) {
        row = *iter;
        b = row.get_value(columns.clmCurrent);
        /*We found item with 'current' tag*/
        if (b == true) {
            /*Deactivate current row*/
            setRowActive(row, false);
            /*Path of item with 'current' column set*/
            Gtk::TreePath tp = lstore->get_path(iter);

            /*Try to go one step back*/
            bool c = tp.prev();
            if (c == true) {
                iter = lstore->get_iter(tp);
                row = *iter;
            } else {
                row = children[children.size() - 1];
            }

            /*So we have iter with 'previous' row*/

            setRowActive(row, true);
            *fspec = row.get_value(columns.clmFilespec);
            return true;
        }

        iter++;
    }
    /*No row had 'current' tag, so we will revert to first*/
    iter = children.begin();
    row = *iter;
    *fspec = row.get_value(columns.clmFilespec);
    setRowActive(row, true);
    return true;

}

void Playlist::setRowActive(Gtk::TreeModel::Row& row, bool active) {

    /*Make active*/
    if (active==true) {
        row.set_value(columns.clmCurrent,true);
        row.set_value(columns.clmMarkup,Glib::ustring("<b>"+row.get_value(columns.clmFilename)+"</b>"));
    }
    /*Make inactive*/
    else {
        row.set_value(columns.clmCurrent,false);
        row.set_value(columns.clmMarkup,row.get_value(columns.clmFilename));
    }
}

void Playlist::removePaths(std::vector<Gtk::TreePath> *pathvect) {

    std::vector<Gtk::TreeRowReference> refs;
    int l = pathvect->size();

    for (int i = 0; i < l; i++) {
        refs.push_back(Gtk::TreeRowReference(lstore, (*pathvect)[i]));

    }
    for (int i = 0; i < l; i++) {
        lstore->erase(*(lstore->get_iter(refs[i].get_path())));
    }

}

Glib::RefPtr<Gtk::ListStore> Playlist::getModel() {
    return lstore;
}

ColumnModel* Playlist::getColumns() {
    return &columns;
}

/*Drag and drop related functionality *************************************************/

bool Playlist::getDraggedSingleFilespec(Glib::ustring* fspec, Glib::ustring dragString) {
#ifdef DEBUG_PRINTOUTS
    printf("dragString: |%s|\n", dragString.c_str());
#endif
    /*Slice dragString into lines and fill vector*/
    std::vector<Glib::ustring> vect;
    sliceDragString(&vect, dragString);

    /*If vector has no elements, no file detected*/
    if (vect.size() < 1) return false;

    /*Get first element, remove url*/
    Glib::ustring xfspec = urlToFilespec(vect[0]);
    /*Check if it is file, if not,return*/
    if (isFile(xfspec) == false) return false;

    *fspec = xfspec;
    return true;


}

bool Playlist::getDraggedMultipleFilespecs(std::vector<Glib::ustring> *items, Glib::ustring dragString) {

#ifdef DEBUG_PRINTOUTS
    printf("dragString: |%s|\n", dragString.c_str());
#endif

    items->clear();
    sliceDragString(items, dragString);

    int l = items->size();

    /*If vector has no elements, no file detected*/
    if (l < 1) return false;

    /*Browse all elements in vector and convert urls to filespecs*/
    for (int i = 0; i < l; i++) {
        (*items)[i] = urlToFilespec((*items)[i]);
    }

    return true;
}

Glib::ustring Playlist::urlToFilespec(Glib::ustring url) {
#ifdef DEBUG_PRINTOUTS
    printf("url: |%s|\n", url.c_str());
#endif

    Glib::ustring retVal;

    /*Remove file:/// prefix and convert those messy ESCAPE sequences*/
    int p = url.find("file://");
    if (p == 0) {
        //retVal=url.substr(7,url.length()-1);
        retVal = Glib::filename_from_uri(url);
    } else {
        retVal = url;
    }



#ifdef DEBUG_PRINTOUTS
    printf("url2: |%s|\n", retVal.c_str());
#endif

    return retVal;


}

void Playlist::sliceDragString(std::vector<Glib::ustring>* v, Glib::ustring dragString) {

    if (v == NULL) return;

    /*Clear the vector*/
    v->clear();

    /*Browse dragString charPerChar and look for \r and \n's and produce substrings*/
    int l = dragString.length();

    gunichar current;
    int alfanumcount = 0;
    Glib::ustring tmp;

    for (int i = 0; i < l; i++) {
        current = dragString[i];
        if (!g_unichar_iscntrl(current) == TRUE) {
            alfanumcount++;
        } else {
            /*At least one previous character was alfanum - slice*/
            if (alfanumcount > 0) {
                tmp = Glib::ustring(dragString.substr(i - alfanumcount, alfanumcount));
                v->push_back(tmp);
            }
            /*Just continue*/
            alfanumcount = 0;
        }
    }

    /*Ending*/
    int i = l;
    if (alfanumcount > 0) {
        tmp = dragString.substr(i - alfanumcount, alfanumcount);
        v->push_back(tmp);
        alfanumcount = 0;
    }

}

bool Playlist::isFile(Glib::ustring filespec) {
    gboolean b = g_file_test(filespec.c_str(), (GFileTest) (G_FILE_TEST_IS_REGULAR));
    if (b == TRUE) return true;
    return false;
}

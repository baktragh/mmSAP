
#ifndef _PLAYLIST_H
#define _PLAYLIST_H

#include "platform.h"
#include "preferences.h"
#include "columnmodel.h"
#include "asap.h"
#include <gtkmm.h>
#include <glibmm.h>

#define PLAYLIST_SAVED_OK 0
#define PLAYLIST_SAVED_ERROR 1
#define PLAYLIST_SAVED_EMPTY 2


#include <sys/stat.h>

/*Playlist routines*/

class Playlist {
public:
    Playlist();

    /*Get single file specifier from DnD result*/
    bool getDraggedSingleFilespec(Glib::ustring* fspec, Glib::ustring dragString);
    bool getDraggedMultipleFilespecs(std::vector<Glib::ustring> *items, Glib::ustring dragString);

    bool addFile(Glib::ustring fspec);
    bool addDirectory(Glib::ustring dirspec);
    bool addMultipleFilespecs(std::vector<Glib::ustring> *specs);

    void removePaths(std::vector<Gtk::TreePath> *pathvect);

    bool continueToNext(Glib::ustring* fspec);
    bool continueToPrevious(Glib::ustring* fspec);

    int load(Glib::ustring fspec);
    int save(Glib::ustring fspec);
    void clearCurrentColumn();
    Glib::RefPtr<Gtk::ListStore> getModel();
    ColumnModel* getColumns();
    bool isFile(Glib::ustring filespec);
    void setRowActive(Gtk::TreeModel::Row& row, bool active);

private:
    /*List store related*/
    Glib::RefPtr<Gtk::ListStore> lstore;
    ColumnModel columns;

    /*Drag and drop related*/
    Glib::ustring urlToFilespec(Glib::ustring url);
    void sliceDragString(std::vector<Glib::ustring>* v, Glib::ustring dragString);

};


#endif /* _PLAYLIST_H */


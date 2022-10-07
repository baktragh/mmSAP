#ifndef _PLAYLISTMODEL_H
#define	_PLAYLISTMODEL_H

#include <gtkmm.h>

class ColumnModel:public Gtk::TreeModel::ColumnRecord {
public:
    ColumnModel();
    /*Visible columns*/
    Gtk::TreeModelColumn<Glib::ustring> clmMarkup;

    /*Invisible columns*/
    Gtk::TreeModelColumn<Glib::ustring> clmFilespec;
    Gtk::TreeModelColumn<Glib::ustring> clmFilename;
    Gtk::TreeModelColumn<bool> clmCurrent;
    
};


#endif	/* _PLAYLISTMODEL_H */
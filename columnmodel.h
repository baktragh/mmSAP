#ifndef _PLAYLISTMODEL_H
#define _PLAYLISTMODEL_H

#include <gtkmm.h>

class ColumnModel : public Gtk::TreeModel::ColumnRecord {
public:
    ColumnModel();
    Gtk::TreeModelColumn<Glib::ustring> clmFilespec;
    Gtk::TreeModelColumn<Glib::ustring> clmFilename;
    Gtk::TreeModelColumn<bool> clmCurrent;
};


#endif /* _PLAYLISTMODEL_H */


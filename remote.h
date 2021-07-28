#ifndef _REMOTE_H
#define _REMOTE_H

#include <gtkmm.h>
#include <glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>


#include "platform.h"

class Remote {
public:
    Remote();
    void init();
    bool sendFilespecMessage(Glib::ustring fspec);
    bool sendTerminateMessage();
    void destroy();
    static DBusHandlerResult on_filter(DBusConnection *connection, DBusMessage *message, void *user_data);
private:
    bool valid;
    DBusGConnection* bus;
    GError* error;
};


#endif /* _REMOTE_H */


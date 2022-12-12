#ifndef _ASMA_H
#define _ASMA_H

#include <stdio.h>
#include <glibmm.h>

class Asma {
public:
    Asma();
    void reinitialize(Glib::ustring asmaDir, Glib::ustring asmaStil,bool asmaEnabled);
    Glib::ustring getEntry(Glib::ustring fspec);

private:
    bool enabled;
    std::map<std::string, std::string> table;
    Glib::ustring directory;
    Glib::ustring stilFile;
    std::vector<Glib::ustring> stilTexts;
    std::vector<Glib::ustring> stilPaths;
    void addEntry(Glib::ustring path, Glib::ustring text);
    void parse();
};



#endif /* _ASMA_H */


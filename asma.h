#ifndef _ASMA_H
#define _ASMA_H

#include <stdio.h>
#include <glibmm.h>

class Asma {
public:
    Asma();
    void initialize(Glib::ustring dir, Glib::ustring stil);
    void parse();
    Glib::ustring getEntry(Glib::ustring fspec);

private:
    bool valid;
    std::map<std::string, std::string> table;
    Glib::ustring asmaDir;
    Glib::ustring stilFile;
    std::vector<Glib::ustring> stilTexts;
    std::vector<Glib::ustring> stilPaths;
    void addEntry(Glib::ustring path, Glib::ustring text);
};



#endif /* _ASMA_H */


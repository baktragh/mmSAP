#include "asma.h"
#include "platform.h"

Asma::Asma() {
    valid=false;
    asmaDir="";
    stilFile="";
    table=NULL;
}

void Asma::initialize(Glib::ustring dir,Glib::ustring stil) {
     valid=false;
    
    /*Check existence of asma directory and files*/
    gboolean b;

	#ifdef DEBUG_PRINTOUTS
	printf("Asma::initialize() testing: |%s|,|%s|\n",dir.c_str(),stil.c_str());
	#endif

    b=g_file_test(dir.c_str(),(GFileTest)(G_FILE_TEST_IS_DIR));
    if (b==false) return;

	#ifdef DEBUG_PRINTOUTS
	printf("Asma::initialize() ASMA directory exist\n");
	#endif
    
    b=g_file_test(stil.c_str(),(GFileTest)(G_FILE_TEST_IS_REGULAR));
    if (b==false) return;

	#ifdef DEBUG_PRINTOUTS
	printf("Asma::initialize() ASMA STIL file exist\n");
	#endif
	
    
    /*These files exist, create hashtable*/
    if(table!=NULL) {
        g_hash_table_unref(table);
        table=NULL;
    };
    table=g_hash_table_new(g_str_hash,g_str_equal);
    
    if (table==NULL) return;
    
    asmaDir=dir;
    stilFile=stil;
    stilPaths.clear();
    stilTexts.clear();
    
    valid=true;
    
}

void Asma::parse() {
    
    #ifdef DEBUG_PRINTOUTS
    printf("Asma::parse()\n");
    #endif
    
    if (valid==false) return;
    
    char buf[512];
    Glib::ustring line;
    
    Glib::ustring tuneText="";
    Glib::ustring tunePath="";
    
    
    FILE* f = fopen(stilFile.c_str(),"rt");
    if (f==NULL) {
		#ifdef DEBUG_PRINTOUTS
		printf("Asma::parse() |%s| not opened\n",stilFile.c_str());
		#endif
		return;
	}
    
    while(fgets(buf,510,f)!=NULL) {
        
        line=Glib::ustring(g_strstrip((gchar*)buf));
        
        #ifdef DEBUG_PRINTOUTS
        //printf("%s\n",line.c_str());
        #endif
        
        /*Ignore blank lines*/
        if (line.length()<1) continue;
        /*Ignore comments*/
        if (line[0]=='#') continue;
        
        /*Tune path !, start new tune*/
        if (line[0]=='/') {
            /*Store previous if available*/
            addEntry(tunePath,tuneText);
            /*Start new*/
            tunePath=line;
            tuneText="";
            continue;
        }
        /*Ordinary line, add to tuneText also with newline*/
        if (tuneText.length()>1024) tuneText="<?1024?>";
        
        /*Tag beginning ?*/
        if (line.find_first_of("ARTIST:")==0 || line.find_first_of("TITLE:")==0
                || line.find_first_of("COMMENT:")==0 || line.find_first_of("(#")==0) {
            tuneText+='\n';
            tuneText+=line;
        }
        
        /*Inside tag or trailing*/
        else {
            tuneText+=' ';
            tuneText+=line;
            
        }
        
    }
    fclose(f);
    
    /*Any trailing*/
    addEntry(tunePath,tuneText);
    
}

void Asma::addEntry(Glib::ustring path,Glib::ustring text) {
    
    if (path!="" && text!="") {
        stilPaths.push_back(path);
        stilTexts.push_back(text);
        #ifdef DEBUG_PRINTOUTS
        printf("ASMA: Added entry: %s\n",path.c_str());
        printf("ASMA: Entry contents begin:\n");
        printf("%s\n",text.c_str());
        printf("ASMA: Entry contents end:\n");
        #endif
        g_hash_table_insert(table,(void*)(stilPaths[stilPaths.size()-1].c_str()),(void*)(stilTexts[stilTexts.size()-1].c_str()));
    }
}

Glib::ustring Asma::getEntry(Glib::ustring fspec) {
    
    #ifdef DEBUG_PRINTOUTS
    printf("Asma::getEntry(%s)\n",fspec.c_str());
    #endif
    
    Glib::ustring retVal="";
    
    if (valid==false) return retVal;
    if (fspec.length()<1) return retVal;
    
    /*Try to find asma directory at the beginning of the filespec*/
    int k = fspec.find(asmaDir);
    if (k!=0) return retVal;
    
    /*The filespec now starts with asma, so we should remove the beginning chars*/
    Glib::ustring key=fspec;
    key=key.erase(0,asmaDir.length());
    
    #ifdef DEBUG_PRINTOUTS
    printf("   key: |%s|\n",key.c_str());
    #endif
    
    /*Look for subkeys*/
    int zl=key.length();
    int xr=zl-1;
    gchar* entry;
    
    while (xr>0) {
        
        if (key[xr]!='/') {
            
        }
        else {
            entry=(gchar*)g_hash_table_lookup(table,key.substr(0,xr+1).c_str());
            
            #ifdef DEBUG_PRINTOUTS
            printf("subkey: |%s|\n",key.substr(0,xr+1).c_str());
            #endif
            
            if (entry!=NULL) {
                retVal+=Glib::ustring(entry);
                retVal+='\n';
            }
        }
        xr--;
    }
    
    /*Look for main key*/
    entry=(gchar*) g_hash_table_lookup(table, key.c_str());

    if (entry != NULL) {
        retVal.insert(0,Glib::ustring("\n"));
        retVal.insert(0,Glib::ustring(entry));
    }

    #ifdef DEBUG_PRINTOUTS
    printf("retVal: %s \n",retVal.c_str());
    #endif
    
    return retVal;
    
    
}

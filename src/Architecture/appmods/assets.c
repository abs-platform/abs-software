#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <regex.h>

#include "assets.h"

#define printf_dbg printf 

int getManifestFromAPK(char *apkname, unsigned char **manifest) 
{
    FILE *apk;
    int retval;
    JZFileHeader header;
    char filename[1024];
    unsigned char *data;
    if(!(apk = fopen(apkname, "rb"))) {
        printf_dbg("Error opening APK file\n");
        return 0;
    }
    while(1) {
        if(jzReadLocalFileHeader(apk, &header, filename, sizeof(filename))) {
            printf_dbg("Couldn't read local file header!\n");
            goto errorClose;
        }
        if(!strcmp(filename, "AndroidManifest.xml")) {
            printf_dbg("AndroidManifest found!\n");
            break;
        }
    }
    if((data = (unsigned char *)malloc(header.uncompressedSize)) == NULL) {
        printf("Couldn't allocate memory!\n");
        goto errorClose;
    }    
    if(jzReadData(apk, &header, data) != Z_OK) {
        printf_dbg("Couldn't read file data!\n");
        goto errorClose;
    }
    *manifest = data;
    printf_dbg("AndroidManifest extracted correctly!\n"); 
    return header.uncompressedSize;
 
errorClose:
    fclose(apk);
    return 0;
}

int LEW(const char *arr, int cb, int off) 
{
    return (cb > off + 3) ? ( ((arr[off+3] << 24) & 0xff000000) | ((arr[off+2] << 16) & 0xff0000)
          | ((arr[off+1] << 8) & 0xff00) | (arr[off] & 0xFF)) : 0;
}

char *compXmlString(const char* xml, int cb, int sitOff, int stOff, int strInd) 
{
    int strOff = stOff + LEW(xml, cb, sitOff + strInd * 4);
    int strLen = ((xml[strOff+1] << 8) & 0xff00) | (xml[strOff] & 0xff);
    char *chars;
    if(strInd < 0 | cb < strOff + 2) { 
        return "";
    } else {
        chars = malloc(strLen + 1);
        chars[strLen] = 0;
        int i;
        for (i = 0; i < strLen; i++) {
            if (cb < strOff + 2 + i * 2) {
                chars[i] = 0;
                break;
            }
            chars[i] = xml[strOff + 2 + i*2];
        }
        return chars;
    }
}

/* converted from a Java function on this link */
/* http://stackoverflow.com/questions/2097813/
how-to-parse-the-androidmanifest-xml-file-inside-an-apk-package */
char *decompressXML(const char *xml, int cb) 
{

    int endDocTag = 0x00100101;
    int startTag =  0x00100102;
    int endTag =    0x00100103;

    int numbStrings = LEW(xml, cb, 4 * 4);
    int sitOff = 0x24;
    int stOff = sitOff + numbStrings*4;
    int xmlTagOff = LEW(xml, cb, 3 * 4);

    int i;
    for (i = xmlTagOff; i < cb - 4; i += 4) {
        if (LEW(xml, cb, i) == startTag) { 
            xmlTagOff = i;  
            break;
        }
    }
    int off = xmlTagOff;
    int indent = 0;
    int startTagLineNo = -2;

    char *result = malloc(MAX_XML_SIZE);

    while (off < cb) {
        int tag0 = LEW(xml, cb, off);
        int lineNo = LEW(xml, cb, off + 2*4);
        int nameSi = LEW(xml, cb, off + 5*4);
        int nameNsSi = LEW(xml, cb, off + 4*4);
      
        if (tag0 == startTag) { 
            int tag6 = LEW(xml, cb, off + 6*4);  
            int numbAttrs = LEW(xml, cb, off + 7*4);  
            off += 9*4;  
            char *name = compXmlString(xml, cb, sitOff, stOff, nameSi);
            startTagLineNo = lineNo;
            char *sb;
            char *concat = "";
            int ii;
            for (ii = 0; ii < numbAttrs; ii++) {
                int attrNameNsSi = LEW(xml, cb, off);
                int attrNameSi = LEW(xml, cb, off + 1*4);
                int attrValueSi = LEW(xml, cb, off + 2*4);
                int attrFlags = LEW(xml, cb, off + 3*4);  
                int attrResId = LEW(xml, cb, off + 4*4); 
                off += 5*4; 
                char *attrName = compXmlString(xml, cb, sitOff, stOff, attrNameSi);
                char *attrValue = (attrValueSi != -1) ? 
                    compXmlString(xml, cb, sitOff, stOff, attrValueSi) : "0x00000000";
                asprintf(&concat, "%s %s=\"%s\"", concat, attrName, attrValue);    
            }
            asprintf(&result, "%s<%s%s>\n",result, name, concat);
            indent++;
        } else if (tag0 == endTag) { // XML END TAG
            indent--;
            off += 6*4;  // Skip over 6 words of endTag data
            char *name = compXmlString(xml, cb, sitOff, stOff, nameSi);
            asprintf(&result, "%s</%s>\n",result, name);
        } else if (tag0 == endDocTag) {  // END OF XML DOC TAG
            break;
        } else {
            printf_dbg("Unrecognized tag code\n");
            break;
        }
    } 
    return result;
} 

char **getPermissionsFromManifest(unsigned char *data, int size, int *results)
{
    char * source = decompressXML((const char *)data, size);
    char * tofind = "<permission label=\"([^\"]+)";
    size_t maxMatches = 5;
      
    regex_t regexCompiled;
    regmatch_t groupArray[2];
    unsigned int m;
    char * cursor;

    char **matches = malloc(maxMatches * sizeof(char *));
     
    if (regcomp(&regexCompiled, tofind, REG_EXTENDED)) {
        printf_dbg("Could not compile REGEX.\n");
        return NULL;
    };

    cursor = source;
    for (m = 0; m < maxMatches; m ++) {
        if (regexec(&regexCompiled, cursor, 2, groupArray, 0)) {
            break;  // No more matches
        }     
        unsigned int offset = 0;
        offset = groupArray[0].rm_eo;
        char cursorCopy[strlen(cursor) + 1];
        strcpy(cursorCopy, cursor);
        cursorCopy[groupArray[1].rm_eo] = 0;
      
        asprintf(&matches[m], "%s", cursorCopy + groupArray[1].rm_so);
        cursor += offset;
    }
    *results = m;
    regfree(&regexCompiled);
    return matches;
}

sqlite3 *openDb(char *dbName)
{       
    int rc;
    sqlite3 *db;
    rc = sqlite3_open(dbName, &db);
    if(rc) {
        printf_dbg("Can't open database: %s\n", sqlite3_errmsg(db));
        return NULL;
    } else {
        printf_dbg("Opened database successfully\n");
        return db;
    }
}

int createPermissionsTable(sqlite3 *db)
{
    int rc;
    char *zErrMsg = 0;

    rc = sqlite3_open("permissions.db", &db);
    if(rc) {
        printf_dbg("Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        printf_dbg("Opened database successfully\n");
    }

    char *sql = "CREATE TABLE PERMISSIONS (       " \
                "ID             INTEGER NOT NULL, " \
                "NAME           TEXT    NOT NULL );";

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

    if(rc != SQLITE_OK) {
        printf_dbg("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } else {
        printf_dbg("Table created successfully\n");
        return 0;
    }
}

int insertPermissionDb(sqlite3 *db, char *permissionName)
{
    int rc = 0;
    char *query = NULL;
    sqlite3_stmt *stmt;

    rc = sqlite3_open("permissions.db", &db);
    if(rc) {
        printf_dbg("Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        printf_dbg("Opened database successfully\n");
    }

    asprintf(&query, "insert into PERMISSIONS (ID, NAME) values (1, 'digital_write');");        

    sqlite3_prepare_v2(db, query, strlen(query), &stmt, NULL);                             

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE) {
        printf("ERROR inserting data: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_finalize(stmt);
    free(query);
    return 0;
}

int deletePermissionDb(sqlite3 *db, char *permissionName)
{
    sqlite3_stmt *stmt;

    int rc;

    rc = sqlite3_open("permissions.db", &db);
    if(rc) {
        printf_dbg("Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        printf_dbg("Opened database successfully\n");
    }

    rc = sqlite3_prepare_v2(db, "DELETE FROM PERMISSIONS         " \
                                    "WHERE NAME = ?", -1, &stmt, NULL );
    if(rc != SQLITE_OK) {
        //throw string(sqlite3_errmsg(db));
        return -1;
    }

    rc = sqlite3_bind_text(stmt, 1, permissionName, -1, 0);    
    if(rc != SQLITE_OK) {                 
        //string errmsg(sqlite3_errmsg(db)); 
        sqlite3_finalize(stmt);            
        return -1;
    }

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_ROW && rc != SQLITE_DONE) {
        //string errmsg(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    if(rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        printf_dbg("Done deleting\n");
        return 1;
    }

    sqlite3_finalize(stmt);

    return 0;
}

int matchPermissionDb(sqlite3 *db, char *permissionName)
{

    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_open("permissions.db", &db);
    if(rc) {
        printf_dbg("Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        printf_dbg("Opened database successfully\n");
    }

    rc = sqlite3_prepare_v2(db, "SELECT ID"                        \
                                " FROM PERMISSIONS"                \
                                " WHERE NAME = ?", -1, &stmt, NULL );
    if(rc != SQLITE_OK) {
        return -1;
    }

    rc = sqlite3_bind_text(stmt, 1, "digital_write", -1, 0);    
    if(rc != SQLITE_OK) {                 
        //string errmsg(sqlite3_errmsg(db)); 
        sqlite3_finalize(stmt);            
        return -1;
    }

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_ROW && rc != SQLITE_DONE) {
        //string errmsg(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    if(rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        printf_dbg("No match found\n");
        return 1;
    }

    char *a = (char *) sqlite3_column_text(stmt, 0);
    int tim = sqlite3_column_int(stmt, 1);
    int exx = sqlite3_column_int(stmt, 2);

    printf_dbg("NAME: %s, TIME: %d, EXPIRES: %d\n", a, tim, exx);

    sqlite3_finalize(stmt);

    return 0;
}
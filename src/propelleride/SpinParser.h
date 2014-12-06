#pragma once

#include <QString>
#include <QStringList>
#include <QFile>
#include <QList>
#include <QMap>
#include <QRegExp>
#include <QDir>
#include <QDebug>
#include <QTextStream>

class SpinParser
{
public:
    SpinParser();
    virtual ~SpinParser();

    void clearDB();

    /*
     *   DATA DEFINITIONS
     */
    typedef enum {
        K_NONE,
        K_CONST,
        K_PUB,
        K_PRI,
        K_OBJECT,
        K_TYPE,
        K_VAR,
        K_DAT,
        K_ENUM,
        K_KINDS
    } SpinKind;

    /*
     * Parse and get a spin project tree list.
     * All object instances will be listed with sub-objects.
     * The list will be indented according to relative position.
     * Calling this function will also populate the tag list
     */
    QStringList spinFileTree(QString file, QString libpath);

    /* build a tag item */
    QString tagItem(QStringList tabs, int field);

    /* parse a file for autocomplete */
    QStringList spinSymbols(QString file, QString objname);

    /* parse a file for autocomplete constants */
    QStringList spinConstants(QString file, QString objname);

    /* parse a file for autocomplete methods */
    QStringList spinMethods(QString file, QString objname);

    /* parse a file for autocomplete variables */
    QStringList spinVars(QString objname);

    /* parse a file for autocomplete dat labels*/
    QStringList spinDat(QString objname);

    /* parse a file for autocomplete objects */
    QStringList spinObjects(QString objname);

    typedef struct {
        QString name;
        QString file;
        QString declaration;
        int     type;
    } Tags;

private:

    typedef struct sKindOption {
        bool enabled;           /* are tags for kind enabled? */
        int letter;             /* kind letter */
        QString name;           /* kind name */
        QString description;    /* displayed in --help output */
    } kindOption;

    typedef struct {
        QString token;
        SpinParser::SpinKind kind;
        int skip;
    } KeyWord;

    kindOption SpinKinds[K_KINDS];
    QList<KeyWord> spin_keywords;

    /* this holds the spin project file list */
    QStringList spinFiles;

    /* this holds the current working spin file */
    QString     currentFile;

    /* this holds the current working tree node */
    QString     objectNode;

    /*
     * This holds a list of all project symbols.
     * The key is a name-path key such as root/obj/subobj/subsubobj/name
     * The items in the string list contain tag information.
     * A tag is composed as:
     * symbol\tfile\tdeclaration\tsymboltype
     */
    QMap<QString, QString> db;

    QString     libraryPath;

private:

    void setKind(kindOption *kind, bool en, const char letter, const char *type, const char *desc);

    const char *extract_name (char const *pos, QString &name);
    int  tokentype(QString tmp);
    int  match_keyword (const char *p, KeyWord const *kw, QString &tag);
    int  spintype(char const *p);
    void match_constant (QString p, int line);
    void match_dat (QString p, int line);
    void match_object (QString p, int line);
    void match_pri (QString p, int line);
    void match_pub (QString p, int line);
    void match_var (QString p, int line);
    int objectInfo(QString tag, QString &name, QString &file);
    QString checkFile(QString fileName);
    void findSpinTags (QString fileName, QString objnode);

    QString shortFileName(QString fileName);
};

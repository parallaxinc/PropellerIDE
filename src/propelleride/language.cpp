#include "language.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QVariantList>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QRegExp>
#include <QColor>
#include <QIcon>

QHash<QString, LanguageData> Language::_data = QHash<QString, LanguageData>();
QHash<QString, QString> Language::_lookup = QHash<QString, QString>();

Language::Language() 
{
    _language = "";
    load();
}

Language::~Language()
{
}

QRegularExpression Language::buildTokenizer(QStringList tokens)
{
    QString tokenstring = tokens.join("|");
    tokenstring = "("+tokenstring+")";

    if (language().case_sensitive)
        return QRegularExpression(tokenstring);
    else
        return QRegularExpression(tokenstring, 
                QRegularExpression::CaseInsensitiveOption);
}

QStringList Language::matchWholeWord(QStringList list)
{
    QStringList newlist;
    foreach(QString s, list)
    {
        s.prepend("\\b");
        s.append("\\b");
        newlist << s;
    }
    return newlist;
}

QStringList Language::buildWordList(QJsonArray keyarray)
{
    QStringList slist;
    QVariantList vlist = keyarray.toVariantList();

    foreach(QVariant v, vlist)
    {
        slist.append(v.toString());
    }

    return slist;
}

QStringList Language::mergeList(QStringList list)
{
    list.removeDuplicates();
    QString specialchars = "* + ? [ ] ( ) | .";
    QString mergedlist = list.join(" ");

    foreach (QString s, specialchars.split(" "))
    {
        mergedlist = mergedlist.replace(s, "\\"+s);
    }

    return mergedlist.split(QRegExp("\\s+"));
}

ProjectParser * Language::buildParser(QJsonArray projectparser)
{
    ProjectParser * parser = new ProjectParser();
    parser->setCaseInsensitive(true);

    foreach (QJsonValue r, projectparser)
    {
        QList<ProjectParser::Pattern> patterns;

        foreach (QJsonValue pattern, r.toObject()["pattern"].toArray())
        {
            ProjectParser::Pattern p;
            p.regex = pattern.toObject()["regex"].toString();
            foreach (QVariant v, pattern.toObject()["capture"].toArray().toVariantList())
            {
                QMetaType::Type t = (QMetaType::Type) v.type();
                if (t == QMetaType::Double)
                {
                    p.capture << v.toInt();
                }
                else if (t == QMetaType::QString)
                {
                    p.capture << v.toString();
                }
            }

            patterns.append(p);
        }

        parser->addRule(r.toObject()["name"].toString(), patterns,
            QIcon(r.toObject()["icon"].toString()),
            QColor(r.toObject()["color"].toString()));
    }

    return parser;
}

void Language::load()
{
    load("","");
}

void Language::loadKey(QString key)
{
    key = key.toLower();
    if (_data.contains(key))
    {
        _language = key;
    }
    else
    {
        load();
    }

}

void Language::loadExtension(QString ext)
{
    ext = ext.toLower();
    if (_lookup.contains(ext))
    {
        _language = _lookup[ext];
    }
    else
    {
        load();
    }
}

void Language::load(QString name, QString filename)
{ 
    QFile file(filename);
    name = name.toLower();
    _language = name;

    if (filename.isEmpty())
    {
        _data[_language] = LanguageData();
        return;
    }
    else if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "Unable to open language definition file:" << filename;
        _data[_language] = LanguageData();
        return;
    }

    QString text = file.readAll();
    file.close();

    LanguageData data;

    QJsonDocument d = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject lang = d.object();
    QJsonObject syntax = lang["syntax"].toObject();

    data.name = lang["name"].toString();

    foreach (QVariant extvar, lang["extension"].toArray().toVariantList())
    {
        QString ext = extvar.toString().toLower();
        data.extensions.append(ext);
        _lookup[ext] = name;
    }

    data.builder = 0;

    foreach (QVariant steplistvar, lang["buildsteps"].toArray().toVariantList())
    {
        QStringList steplist = steplistvar.toStringList();
        QStringList buildsteps;

        foreach (QString s, steplist)
        {
            buildsteps << s;
        }

        data.buildsteps << buildsteps;
    }

    qDebug()    << "Loading language definition:" << qPrintable(data.name);
    qDebug()    << "    Extensions: " << data.extensions;
    qDebug()    << "    Build steps:";
    foreach (QStringList sl, data.buildsteps)
        qDebug() << "      " << sl;

    data.includes  = lang["includes"].toBool();

    data.numbers   = buildWordList(syntax["number"].toArray());
    data.functions = buildWordList(syntax["function"].toArray());
    data.comments  = mergeList(buildWordList(syntax["comment"].toArray()));
    data.strings   = mergeList(buildWordList(syntax["string"].toArray()));

    data.enable_blocks   = syntax["enable_blocks"].toArray().first().toBool();

    if (data.enable_blocks)
    {
        data.blocks = mergeList(buildWordList(syntax["enable_blocks"].toArray().last().toArray()));
    }

    data.case_sensitive  = syntax["case_sensitive"].toBool();

    foreach(QJsonValue m, syntax["mode"].toObject())
    {
        QStringList slist = buildWordList(m.toObject()["keywords"].toArray());
        slist = mergeList(slist);
        slist = matchWholeWord(slist);

        data.keywords.append(slist);

        slist = buildWordList(m.toObject()["operators"].toArray());
        slist = mergeList(slist);

        data.operators.append(slist);
    }

    if (data.parser)
        delete data.parser;

    data.parser = buildParser(lang["project"].toArray());

    _data[_language] = data;
}

QStringList Language::languages()
{
    QStringList langs = _data.keys();
    langs.removeAll("");
    return langs;
}

QStringList Language::allExtensions()
{
    QStringList exts = _lookup.keys();
    exts.removeAll("");
    return exts;
}

LanguageData Language::language()
{
    if (_data.contains(_language))
        return _data[_language];
    else
        return _data[""];
}

QString Language::key()
{
    return _language;
}

QString Language::name()
{
    return language().name;
}

QStringList Language::extensions()
{
    return language().extensions;
}

ProjectParser * Language::parser()
{
    return language().parser;
}


int Language::builder()
{
    return language().builder;
}

void Language::setBuilder(int index)
{
    if (!_data.contains(_language))
        return;

    if (!builders())
        _data[_language].builder = -1;

    if (index < 0)
        _data[_language].builder = 0;
    else if (index > builders()-1)
        _data[_language].builder = builders()-1;
    else
        _data[_language].builder = index;
}

int Language::builders()
{
    return language().buildsteps.size();
}

QStringList Language::listBuildSteps()
{
    return language().buildsteps[language().builder];
}

QList<QStringList> Language::listAllBuildSteps()
{
    return language().buildsteps;
}

QStringList Language::listKeywords()
{
    return language().keywords;
}

QStringList Language::listOperators()
{
    return language().operators;
}

QStringList Language::listNumbers()
{
    return language().numbers;
}

QStringList Language::listStrings()
{
    return language().strings;
}

QStringList Language::listComments()
{
    return language().comments;
}

QStringList Language::listFunctions()
{
    return language().functions;
}

QStringList Language::listBlocks()
{
    return language().blocks;
}

bool Language::isCaseSensitive()
{
    return language().case_sensitive;
}

bool Language::hasIncludes()
{
    return language().includes;
}


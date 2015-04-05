#include "Language.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QVariantList>
#include <QStringList>
#include <QFile>
#include <QString>
#include <QRegExp>
#include <QColor>
#include <QIcon>

Language::Language() 
{
    loadLanguage(":/languages/spin.json");
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
    return list.join(" ").split(QRegExp("\\s"));
}

QJsonObject Language::loadLanguage(QString filename)
{ 
    QString val;
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    lang = d.object();
    syntax = lang["syntax"].toObject();

    case_sensitive = false;
    enable_blocks = false;

    numbers = buildWordList(syntax["number"].toArray());
    functions = buildWordList(syntax["function"].toArray());
    comments = buildWordList(syntax["comment"].toArray());
    strings = buildWordList(syntax["string"].toArray());

    enable_blocks = syntax["enable_blocks"].toArray().first().toBool();
    case_sensitive = syntax["case_sensitive"].toBool();
    escape_char = syntax["escape"].toString();

    modes = syntax["mode"].toObject();
    foreach(QJsonValue m, modes)
    {
        QStringList slist = buildWordList(
                m.toObject()["keywords"].toArray());
        slist = mergeList(slist);

        slist = matchWholeWord(slist);

        keywords.append(slist);

        slist = buildWordList(
                m.toObject()["operators"].toArray());
        slist = mergeList(slist);

        operators.append(slist);
    }

    parser.clearRules();
    parser.setCaseInsensitive(true);

    foreach (QJsonValue r, lang["project"].toArray())
    {
        QList<Parser::Pattern> patterns;

        foreach (QJsonValue pattern, r.toObject()["pattern"].toArray())
        {
            Parser::Pattern p;
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

//            qDebug() << p.regex;
//            qDebug() << p.capture;
            patterns.append(p);
        }

        parser.addRule(r.toObject()["name"].toString(), patterns,
            QIcon(r.toObject()["icon"].toString()),
            QColor(r.toObject()["color"].toString()));

    }

    return lang;
}

QStringList Language::listKeywords()
{
    return keywords;
}

QStringList Language::listOperators()
{
    return operators;
}

QStringList Language::listNumbers()
{
    return numbers;
}

QStringList Language::listStrings()
{
    return strings;
}

QStringList Language::listComments()
{
    return comments;
}

QStringList Language::listFunctions()
{
    return functions;
}

Parser * Language::getParser()
{
    return &parser;
}

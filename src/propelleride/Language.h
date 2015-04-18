#pragma once

#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QString>

#include "projectparser.h"

class Language
{
private:
    bool case_sensitive;
    bool enable_blocks;
    QString escape_char;
    

    QStringList keywords;
    QStringList operators;
    QStringList numbers;
    QStringList strings;
    QStringList functions;
    QStringList comments;

    QStringList matchWholeWord(QStringList list);
    QStringList buildWordList(QJsonArray keyarray);
    QStringList mergeList(QStringList list);

public:
    ProjectParser parser;

    void buildParser(QJsonArray projectparser);
    void loadLanguage(QString filename);
    QStringList listKeywords();
    QStringList listOperators();
    QStringList listNumbers();
    QStringList listStrings();
    QStringList listComments();
    QStringList listFunctions();
    Language();
    ProjectParser * getParser(QString language = QString("spin"));
};

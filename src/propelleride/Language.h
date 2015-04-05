#pragma once

#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QString>

#include "parser.h"

class Language
{
private:
    QJsonObject lang;
    QJsonObject syntax;
    QJsonObject modes;

    bool case_sensitive;
    bool enable_blocks;
    QString escape_char;
    
    Parser parser;

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
    QJsonObject loadLanguage(QString filename);
    QStringList listKeywords();
    QStringList listOperators();
    QStringList listNumbers();
    QStringList listStrings();
    QStringList listComments();
    QStringList listFunctions();
    Language();
    Parser * getParser();
};

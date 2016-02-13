#pragma once

#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QStringList>
#include <QString>

#include <ProjectParser>

class Language
{
    ProjectParser parser;

    bool case_sensitive;
    bool enable_blocks;
    QString escape_char;

    QStringList keywords;
    QStringList operators;
    QStringList numbers;
    QStringList strings;
    QStringList functions;
    QStringList comments;
    QStringList blocks;

    QStringList matchWholeWord(QStringList list);
    QStringList buildWordList(QJsonArray keyarray);
    QStringList mergeList(QStringList list);

public:

    void buildParser(QJsonArray projectparser);
    void loadLanguage(QString filename);
    QStringList listKeywords();
    QStringList listOperators();
    QStringList listNumbers();
    QStringList listStrings();
    QStringList listComments();
    QStringList listFunctions();
    QStringList listBlocks();

    bool isCaseSensitive();
    QRegularExpression buildTokenizer(QStringList rules);

    Language();
    ProjectParser * getParser(QString language = QString("spin"));
};

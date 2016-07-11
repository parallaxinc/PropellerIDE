#pragma once

#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QStringList>
#include <QString>

#include <ProjectParser>

struct LanguageData
{
    ProjectParser * parser;

    bool case_sensitive;
    bool enable_blocks;

    QStringList keywords;
    QStringList operators;
    QStringList numbers;
    QStringList strings;
    QStringList functions;
    QStringList comments;
    QStringList blocks;
};

class Language
{
    static QHash<QString, LanguageData> _data;
    QString _language;

    QStringList matchWholeWord(QStringList list);
    QStringList buildWordList(QJsonArray keyarray);
    QStringList mergeList(QStringList list);
    ProjectParser * buildParser(QJsonArray projectparser);
    LanguageData language();

public:

    ~Language();
    Language();
    void load(QString filename);
    QStringList languages();
    ProjectParser * parser();

    QStringList listKeywords();
    QStringList listOperators();
    QStringList listNumbers();
    QStringList listStrings();
    QStringList listComments();
    QStringList listFunctions();
    QStringList listBlocks();

    bool isCaseSensitive();
    QRegularExpression buildTokenizer(QStringList rules);

};

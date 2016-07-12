#pragma once

#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QStringList>
#include <QString>

#include <ProjectParser>

class LanguageData
{
public:
    LanguageData()
    {
        parser = new ProjectParser();

        case_sensitive = false;
        enable_blocks = false;
    }

    QString name;
    QStringList extensions;

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
    static QHash<QString, QString> _lookup;
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
    void load();
    void loadKey(QString name);
    void loadExtension(QString ext);
    void load(QString name, QString filename);
    static QStringList languages();
    static QStringList extensions();

    QString key();
    QString name();
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

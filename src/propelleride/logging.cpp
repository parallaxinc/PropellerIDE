#include "logging.h"



Q_LOGGING_CATEGORY(logmain,             "main")
Q_LOGGING_CATEGORY(logmainwindow,       "mainwindow")
Q_LOGGING_CATEGORY(logeditor,           "editor")

Q_LOGGING_CATEGORY(logfilemanager,      "filemanager")
Q_LOGGING_CATEGORY(logfile,             "file")

Q_LOGGING_CATEGORY(logbuildmanager,     "buildmanager")

Q_LOGGING_CATEGORY(loglanguage,         "language")
Q_LOGGING_CATEGORY(loghighlighter,      "highlighter")

Q_LOGGING_CATEGORY(logcolorscheme,      "colorscheme")

enum AnsiColor
{
    AnsiBlack,
    AnsiBlue,
    AnsiGreen,
    AnsiCyan,
    AnsiRed,
    AnsiPurple,
    AnsiBrown,
    AnsiLightGray,
    AnsiDarkGray,
    AnsiLightBlue,
    AnsiLightGreen,
    AnsiLightCyan,
    AnsiLightRed,
    AnsiLightPurple,
    AnsiYellow,
    AnsiWhite
};

QHash<AnsiColor, QString> initAnsiColors()
{
    QHash<AnsiColor, QString> hash;

    hash[AnsiBlack] = "0;30";
    hash[AnsiBlue] = "0;34";
    hash[AnsiGreen] = "0;32";
    hash[AnsiCyan] = "0;36";
    hash[AnsiRed] = "0;31";
    hash[AnsiPurple] = "0;35";
    hash[AnsiBrown] = "0;33";
    hash[AnsiLightGray] = "0;37";
    hash[AnsiDarkGray] = "1;30";
    hash[AnsiLightBlue] = "1;34";
    hash[AnsiLightGreen] = "1;32";
    hash[AnsiLightCyan] = "1;36";
    hash[AnsiLightRed] = "1;31";
    hash[AnsiLightPurple] = "1;35";
    hash[AnsiYellow] = "1;33";
    hash[AnsiWhite] = "1;37";

    return hash;
}

static const QHash<AnsiColor, QString> ansiColors = initAnsiColors();

QString wrapColor(AnsiColor color, QString text)
{
#ifdef Q_OS_WIN
    return text;
#else
    return "\033[" + ansiColors[color] + "m" + text + "\033[0m";
#endif
}

void message(AnsiColor color,
        QString text,
        const QMessageLogContext &context,
        const QString &msg)
{
#ifdef QT_MESSAGELOGCONTEXT
    fprintf(stderr, "[%s] %s(%i): %s\n",
            qPrintable(wrapColor(color, text)),
            qPrintable(wrapColor(AnsiLightBlue, context.file)),
            context.line,
            qPrintable(msg));
#else
    Q_UNUSED(context);

    fprintf(stderr, "[%s] %s\n",
            qPrintable(wrapColor(color, text)),
            qPrintable(msg));
#endif
}

void messageHandler(QtMsgType type,
        const QMessageLogContext &context,
        const QString &msg)
{
    switch (type) {
        case QtDebugMsg:    message(AnsiLightGreen, "DEBUG", context, msg);
                            break;
#ifdef QtInfoMsg
        case QtInfoMsg:     message(AnsiWhite,      "INFO ", context, msg);
                            break;
#endif
        case QtWarningMsg:  message(AnsiYellow,     "WARN ", context, msg);
                            break;

        case QtCriticalMsg: message(AnsiLightRed,   "ERROR", context, msg);
                            break;

        case QtFatalMsg:    message(AnsiLightRed,   "FATAL", context, msg);
                            abort();
    }
}


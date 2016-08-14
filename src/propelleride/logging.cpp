#include "logging.h"

#include <QFile>
#include <QDateTime>

QFile logfile;

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
    QString timestamp = QDateTime(QDateTime::currentDateTime()).toString();
    QString output;

#ifdef QT_MESSAGELOGCONTEXT
    output = QString("%1 [%2] %3 (%4): %5\n")
        .arg(timestamp)
        .arg(wrapColor(color, text))
        .arg(wrapColor(AnsiLightBlue, context.file))
        .arg(context.line)
        .arg(msg);
#else
    Q_UNUSED(context);

    output = QString("%1 [%2] %3\n")
        .arg(timestamp)
        .arg(wrapColor(color, text))
        .arg(msg);
#endif
    fprintf(stderr, "%s", qPrintable(output));
    fflush(stderr);

    if (logfile.fileName().isEmpty())
        return;

    if (!logfile.isOpen() && !logfile.open(QIODevice::WriteOnly | QIODevice::Append))
        return;

    output = QString("%1 [%2] %3\n")
            .arg(timestamp)
            .arg(text)
            .arg(msg);

    logfile.write(qPrintable(output));
    logfile.flush();
}

void messageHandler(QtMsgType type,
        const QMessageLogContext &context,
        const QString &msg)
{
    switch (type) {
        case QtDebugMsg:    message(AnsiLightGreen, "DEBUG", context, msg);
                            break;
#if QT_VERSION >= 0x050500
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

void setLogFileName(const QString & filename)
{
    logfile.close();
    logfile.setFileName(filename);
    logfile.remove();
}

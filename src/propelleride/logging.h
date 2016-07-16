#pragma once

#include <Qt>
#include <QMessageLogContext>
#include <QString>

void messageHandler(QtMsgType type,
        const QMessageLogContext &context,
        const QString &msg);

void setLogFileName(const QString & filename);

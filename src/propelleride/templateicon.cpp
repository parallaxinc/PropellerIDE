#include "templateicon.h"

#include <QDebug>
#include <QFileInfo>

TemplateIcon::TemplateIcon(QString filename,
        QWidget * parent)
: QWidget(parent)
{
    ui.setupUi(this);

    _filename = filename;

    QFileInfo fi(filename);

    ui.filename->setText(fi.fileName().replace("_"," ").remove(".spin"));

    QString imagename = fi.absolutePath() + "/" + fi.completeBaseName() + ".png";

    if (QFileInfo(imagename).exists())
    {
        QImage image(imagename);
        image = image.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui.icon->setPixmap(QPixmap::fromImage(image));
    }
    else
        ui.icon->setPixmap(QPixmap(":/icons/edit-selectall.png"));
}

TemplateIcon::~TemplateIcon()
{
}

void TemplateIcon::enterEvent(QEvent * e)
{
    Q_UNUSED(e);
    setColor(QColor("#97C9FD"));
}

void TemplateIcon::leaveEvent(QEvent * e)
{
    Q_UNUSED(e);
    setColor(QColor("#FFFFFF"));
}

void TemplateIcon::mousePressEvent(QMouseEvent * e)
{
    Q_UNUSED(e);
    setColor(QColor("#2D83DE"));
    emit templateSelected(_filename);
}

void TemplateIcon::setColor(QColor color)
{
    QPalette p = palette();
    p.setColor(QPalette::Window, color);
    setPalette(p);
}

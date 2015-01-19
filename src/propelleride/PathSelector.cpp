#include "PathSelector.h"

PathSelector::PathSelector(
        QString const & labelname,
        QString const & defaultpath,
        QString const & errormessage,
        const char * slot, 
        QWidget *parent) :
    QWidget(parent)
{
    this->defaultpath = defaultpath;
    this->key = labelname;

    label = new QLabel(key);
    label->setMinimumWidth(70);

    lineEdit = new QLineEdit(this);
    lineEdit->setMinimumWidth(200);
    lineEdit->setToolTip(errormessage);

    layout = new QHBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    browseButton = new QPushButton(tr("Browse"), this);
    layout->addWidget(browseButton);

    this->setLayout(layout);

    connect(browseButton, SIGNAL(clicked()), parent, slot);

    lineEdit->setText(defaultpath);
    if ( QSettings().contains(key) )
    {
        load();
    }
    else
    {
        save();
    }

    oldvalue = lineEdit->text();
}

PathSelector::~PathSelector()
{
    delete layout;
    delete label;
    delete lineEdit;
    delete browseButton;
}

void PathSelector::browsePath(
        QString const & pathlabel, 
        QString const & pathregex,  
        bool isfolder
        )
{
    QString folder = lineEdit->text();
    QString pathname;

    if (isfolder) 
        pathname = QFileDialog::getExistingDirectory(this,
                pathlabel, folder, QFileDialog::ShowDirsOnly);
    else
        pathname = QFileDialog::getOpenFileName(this,
                pathlabel, folder, pathregex);


    QString s = QDir::fromNativeSeparators(pathname);

    if(s.length() == 0)
    {
        qDebug() << "browsePath(" << pathlabel << "): " << "No selection";
        return;
    }

    if (isfolder)
        if(!s.endsWith("/"))
            s += "/";

    lineEdit->setText(s);

    qDebug() << "browsePath(" << pathlabel << "): " << s;
}

void PathSelector::restore()
{
    lineEdit->setText(oldvalue);
}

void PathSelector::save()
{
    QSettings settings;
    settings.beginGroup("Paths");
    settings.setValue(key,lineEdit->text());
    settings.endGroup();
}

void PathSelector::load()
{
    QSettings settings;
    settings.beginGroup("Paths");
    QString s = settings.value(key, defaultpath).toString();
    settings.endGroup();

    s = QDir::fromNativeSeparators(s);

    if(!s.isEmpty())
    {
        lineEdit->setText(s);
    }
    else
    {
        lineEdit->setText(defaultpath);
    }
}

QString const & PathSelector::get()
{
    static const QString & string = lineEdit->text();
    return string;
}

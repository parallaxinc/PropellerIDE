#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QSettings>

class PathSelector : public QWidget
{
    Q_OBJECT

public:
    explicit PathSelector(
        QString const & labelname,
        QString const & defaultpath,
        QString const & errormessage,
        const char * slot, 
        QWidget *parent = 0);

    ~PathSelector();

    void browsePath(
        QString const & pathlabel, 
        QString const & pathregex,  
        bool isfolder
        );

    void save();
    void load();
    void restore();
    QString const & get();
    
private:
    QHBoxLayout * layout;
    QLabel      * label;
    QLineEdit   * lineEdit;
    QPushButton * browseButton;
    QString     defaultpath;
    QString     key;
    QString     oldvalue;

};

#pragma once

#include <QString>
#include <QComboBox>

#include "colorscheme.h"
#include "pathselector.h"

#include "ui_preferences.h"

class Preferences : public QDialog
{
    Q_OBJECT

    Ui::Preferences ui;

    QString defaultTheme;

    ColorScheme * currentTheme;

    void setupThemes();
    void setupFonts();
    void setupColors();
    void setupLanguages();

    void updateAll();

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

    void    adjustFontSize(float ratio);

signals:    
    void    updateColors();
    void    updateFonts(const QFont &);

    void    restored();

public slots:
    void updateColor(int key, const QColor & color);
    void updateFont(const QFont & font);
    void showPreferences();
    void setFontSize(int size);

private slots:
    void loadTheme(int index);
    void loadTheme(QString filename);

    void buttonBoxClicked(QAbstractButton * button);

    void restore();
    void accept();
    void reject();
    void load();
    void save();
};

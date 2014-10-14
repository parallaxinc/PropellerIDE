#ifndef EDITFRAME_H
#define EDITFRAME_H

#include "qtversion.h"

#include "editor.h"
#include "spinparser.h"

class EditFrame : public QWidget
{
    Q_OBJECT
public:
    explicit EditFrame(QWidget *parent);
    virtual ~EditFrame();
    void    initSpin(SpinParser *parser);
    void    setTabWidget(QTabWidget *tab);

    Editor *getEditor();
    void    setEditor(int num, QString shortName, QString fileName, QString text);

    void    showMethods();

signals:

public slots:
    void    showMethod(int index);
    void    hideMethods();
    void    methodSortAlpha(bool enable);

private:

    QFrame     *frame;
    Editor     *editor;
    QTabWidget *editorTabs;
    int         tabNumber;

    QStringList methodList;
    QStringList sortedList;

    QVBoxLayout vlayout;
    QHBoxLayout hlayout;

    QComboBox   *cbMethods;
    QPushButton *closeBtn;
    QCheckBox   *alphaBtn;
};

#endif // EDITFRAME_H

#include "editframe.h"
#include "spinparser.h"

/*
 * Lots of interesting new code here, but it doesn't work.
 * For some reason it crashes.
 * Leave it out of the build for now.
 */
EditFrame::EditFrame(QWidget *parent) : tabNumber(0)
{
    editor = new Editor(parent);

    cbMethods = new QComboBox(this);
    connect(cbMethods, SIGNAL(highlighted(int)), this, SLOT(showMethod(int)));

    alphaBtn = new QCheckBox(this);
    alphaBtn->setText("Sort Alphabetically");
    alphaBtn->setToolTip("Sort Methods Alphabetically");
    connect(alphaBtn, SIGNAL(clicked(bool)), this, SLOT(methodSortAlpha(bool)));

    closeBtn = new QPushButton(this);
    closeBtn->setText("X");
    closeBtn->setToolTip("Close Method Browser");
    connect(closeBtn, SIGNAL(clicked()), this, SLOT(hideMethods()));

    hlayout.addWidget(cbMethods);
    hlayout.addWidget(alphaBtn);
    hlayout.addWidget(closeBtn);

    vlayout.addLayout(&hlayout);
    vlayout.addWidget(editor);

    setLayout(&vlayout);

    hideMethods();
}

EditFrame::~EditFrame()
{
    delete editor;
}

void EditFrame::initSpin(SpinParser *parser)
{
    editor->initSpin(parser);
}

void EditFrame::showMethods()
{
    QString file = editorTabs->tabToolTip(tabNumber);

    cbMethods->clear();
    cbMethods->setAutoCompletion(true);
    cbMethods->setAutoCompletionCaseSensitivity(Qt::CaseInsensitive);

    QStringList list = editor->getSpinParser()->spinMethods(file, "");
    foreach(QString s, list) {
        if (s.indexOf("PUB",Qt::CaseInsensitive) > 0 ||
            s.indexOf("PRI",Qt::CaseInsensitive) > 0) {
            s = s.mid(s.indexOf("\t")+1);
            QVariant mstr = QVariant(s.mid(s.indexOf("\t")+1));
            s = s.mid(3); // skip PUB/PRI
            if(s.indexOf(":") > 0) {
                methodList.append(s.mid(0,s.indexOf(":")).trimmed()+"::"+mstr.toString());
                cbMethods->addItem(s.mid(0,s.indexOf(":")).trimmed(),QVariant(mstr));
            }
            else if(s.indexOf("|") > 0) {
                methodList.append(s.mid(0,s.indexOf("|")).trimmed()+"::"+mstr.toString());
                cbMethods->addItem(s.mid(0,s.indexOf("|")).trimmed(),QVariant(mstr));
            }
            else {
                methodList.append(s.trimmed()+"::"+mstr.toString());
                cbMethods->addItem(s.trimmed(),QVariant(mstr));
            }
        }
    }
    cbMethods->show();
    alphaBtn->show();
    closeBtn->show();
}

void EditFrame::hideMethods()
{
    cbMethods->hide();
    alphaBtn->hide();
    closeBtn->hide();
}

void EditFrame::showMethod(int index)
{
    QVariant vs = cbMethods->itemData(index, Qt::UserRole);
    QString  s  = vs.toString();
    QRegExp  rs = QRegExp("\\b"+s+"\\b");
    int pos = rs.indexIn(editor->toPlainText());
    QTextCursor cur = editor->textCursor();
    if(pos > -1) {
        cur.setPosition(pos);
        editor->setTextCursor(cur);
    }
}

void EditFrame::methodSortAlpha(bool enable)
{
    sortedList = methodList;
    if(enable) sortedList.sort();

    cbMethods->clear();
    foreach(QString s, sortedList) {
        QStringList list = s.split("::");
        cbMethods->addItem(list[0],QVariant(list[1]));
    }
    cbMethods->show();
    alphaBtn->show();
    closeBtn->show();
}

Editor *EditFrame::getEditor()
{
    return editor;
}

void    EditFrame::setTabWidget(QTabWidget *tab)
{
    editorTabs = tab;
}

void    EditFrame::setEditor(int num, QString shortName, QString fileName, QString text)
{
    editor->setPlainText(text);
    editor->setHighlights(shortName);
    editorTabs->setTabText(num,shortName);
    editorTabs->setTabToolTip(num,fileName);
    editorTabs->setCurrentIndex(num);
    tabNumber = num;
}

#include "terminal.h"
#include "properties.h"
#include "qextserialport.h"

Terminal::Terminal(QWidget *parent) : QDialog(parent)
{
    QSettings settings(publisherKey, PropellerIdeGuiKey);
    QVariant echov = settings.value(terminalEchoOn,true);

    QVBoxLayout *termLayout = new QVBoxLayout(this);
    termLayout->setContentsMargins(4,4,4,4);
    termEditor = new Console(parent);
    termEditor->setReadOnly(false);
    //connect(this, SIGNAL(accepted()), this, SLOT(keyPressEvent(QKeyEvent *)));
    termLayout->addWidget(termEditor);
    QPushButton *cls = new QPushButton(tr("Clear"),this);
    connect(cls,SIGNAL(clicked()), this, SLOT(clearScreen()));

    echoOnBox.setText("Echo On");
    echoOnBox.setCheckable(true);
    echoOnBox.setChecked(echov.toBool());
    connect(&echoOnBox, SIGNAL(clicked()), this, SLOT(echoClicked()));

    portLabel.setMargin(2);
    portLabel.setStyleSheet("QLabel { background-color: white; }");

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QHBoxLayout *butLayout = new QHBoxLayout();
    termLayout->addLayout(butLayout);

    butLayout->addWidget(cls);
    butLayout->addWidget(&echoOnBox);
    butLayout->addWidget(&portLabel);
    butLayout->addWidget(buttonBox);
    setLayout(termLayout);
    this->setWindowFlags(Qt::Tool);
    portListener = NULL;
    resize(700,500);
}

Console *Terminal::getEditor()
{
    return termEditor;
}

void Terminal::setPortListener(PortListener *listener)
{
    portListener = listener;
    QextSerialPort *port = listener->getPort();
    if(port) {
        if(port->portName().isEmpty() == false)
            portLabel.setText(port->portName());
    }
    else {
        portLabel.setText("");
    }
}

QString Terminal::getPortName()
{
    return portLabel.text();
}

void Terminal::setPortName(QString name)
{
    portLabel.setText(name);
}

void Terminal::setPosition(int x, int y)
{
    int width = this->width();
    int height = this->height();
    this->setGeometry(x,y,width,height);
}

void Terminal::accept()
{
/*
    QString text = termEditor->toPlainText();
    QStringList list = text.split("\n");
*/
    done(QDialog::Accepted);
}

void Terminal::reject()
{
    done(QDialog::Rejected);
}

void Terminal::clearScreen()
{
    termEditor->setPlainText("");
}

void Terminal::echoClicked()
{
    QSettings settings(publisherKey, PropellerIdeGuiKey);
    settings.setValue(terminalEchoOn,this->echoOnBox.isChecked());
}

void Terminal::print(QByteArray str)
{
    QString text;
    QTextCursor cur;
    int size = str.length();
    char *buff = str.data();
    if(!echoOnBox.isChecked())
        return;

    for(int n = 0; n < size; n++) {
        cur = termEditor->textCursor();
        cur.movePosition(QTextCursor::End,QTextCursor::MoveAnchor);

        text = termEditor->toPlainText();
        int tlen = text.length();
        if(!isprint(buff[n]))
            continue;
        switch(buff[n]) {
            case '\b':
                termEditor->setPlainText(text.mid(0,tlen-1));
                break;
            case '\r':
                cur.insertText("\n");
                break;
            default:
                cur.insertText(QString(str.at(n)));
                break;
        }
        termEditor->setTextCursor(cur);
    }
}

void Terminal::copyFromFile()
{
    termEditor->copy();
}
void Terminal::cutFromFile()
{
    termEditor->cut();
}
void Terminal::pasteToFile()
{
    termEditor->paste();
}

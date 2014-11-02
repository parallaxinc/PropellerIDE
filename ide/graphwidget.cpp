#include "graphwidget.h"
#include <QToolBar>
#include <QToolBox>

GraphWidget::GraphWidget(QWidget *graph, QToolBar *tools, QWidget *parent) : QWidget(parent)
{

    mainLayout = new QVBoxLayout(this);
    if(tools) {
        mainLayout->addWidget(tools);
    }
    mainLayout->addWidget(graph);
    setLayout(mainLayout);
    setContentsMargins(0,0,0,0);
    setStyleSheet("QWidget { background-color: #f0e0c0; }");
}

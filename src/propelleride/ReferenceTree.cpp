#include "ReferenceTree.h"


ReferenceTree::ReferenceTree(
        const QString & name,
        ColorScheme::Color colorkey,
        QWidget *parent) :
    QTreeView(parent)
{
    this->setToolTip(name);

    color = colorkey;
    currentTheme = &Singleton<ColorScheme>::Instance();
}

ReferenceTree::~ReferenceTree()
{
}

void ReferenceTree::updateColors()
{
    QFont font = this->font();
    font.setItalic(true);
    this->setFont(font);

    QPalette p = this->palette();
    p.setColor(QPalette::Text, currentTheme->getColor(ColorScheme::SyntaxText));
    p.setColor(QPalette::Base, currentTheme->getColor(color));
    this->setPalette(p);
}

void ReferenceTree::updateFonts()
{
    this->setFont(currentTheme->getFont());
}

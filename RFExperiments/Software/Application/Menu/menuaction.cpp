#include "menuaction.h"

#include <QLabel>

MenuAction::MenuAction(const QString &l)
{
    subline = nullptr;
    auto label = new QLabel(l, this);
    label->setAlignment(Qt::AlignCenter);
    layout.addWidget(label);
    setLayout(&layout);
}

void MenuAction::AddSubline(const QString &l)
{
    if(!subline) {
        subline = new QLabel(this);
        subline->setAlignment(Qt::AlignCenter);
        layout.addWidget(subline);
    }
    QFont f( "Arial", 8);
    subline->setFont( f);
    subline->setText(l);
}

void MenuAction::RemoveSubline()
{
    if(subline) {
        layout.removeWidget(subline);
        delete subline;
        subline = nullptr;
    }
}

void MenuAction::userSelected()
{
    emit triggered();
}

#include "menuaction.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QStackedLayout>

MenuAction::MenuAction(const QString &l)
{
    auto layout = new QVBoxLayout;
    auto label = new QLabel(l, this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    setLayout(layout);
}

void MenuAction::userSelected()
{
    emit triggered();
}

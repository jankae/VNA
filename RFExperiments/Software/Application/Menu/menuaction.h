#ifndef MENULABEL_H
#define MENULABEL_H

#include "menuitem.h"

class MenuAction : public MenuItem
{
    Q_OBJECT
public:
    MenuAction(const QString &l);

signals:
    void triggered();
public slots:
    void userSelected() override;
};

#endif // MENULABEL_H

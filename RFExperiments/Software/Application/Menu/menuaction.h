#ifndef MENULABEL_H
#define MENULABEL_H

#include "menuitem.h"
#include <QVBoxLayout>
#include <QLabel>

class MenuAction : public MenuItem
{
    Q_OBJECT
public:
    MenuAction(const QString &l);
    void AddSubline(const QString &l);
    void RemoveSubline();
signals:
    void triggered();
public slots:
    void userSelected() override;
private:
    QVBoxLayout layout;
    QLabel *subline;
};

#endif // MENULABEL_H

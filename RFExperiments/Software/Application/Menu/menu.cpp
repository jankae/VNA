#include "menu.h"
#include <QKeyEvent>
#include "menuaction.h"
#include <iostream>

using namespace std;

Menu::Menu(QStackedLayout &layout)
    : m_containingLayout(layout)
{
    m_layout = new QVBoxLayout;
    setLayout(m_layout);
    m_widgetCount = 0;
    setFixedSize(180, 800);
    parent = nullptr;
    layout.addWidget(this);
}

void Menu::addItem(MenuItem *i)
{
    if(m_widgetCount >= maxWidgets) {
        throw runtime_error("Menu already at maximum capacity");
    }
    m_layout->addWidget(i, 1);
    items.push_back(i);
    m_widgetCount++;
}

void Menu::addMenu(Menu *m, QString name)
{
    auto menuLabel = new MenuAction(name);
    submenus.push_back(SubmenuEntry(menuLabel, m, m_widgetCount));
    connect(menuLabel, &MenuAction::triggered, [=]() {
       m->m_containingLayout.setCurrentWidget(m);
    });
    addItem(menuLabel);
    m->parent = this;
}

void Menu::finalize()
{
    m_layout->addStretch(maxWidgets - m_widgetCount);
}

void Menu::keyPressEvent(QKeyEvent *event)
{
    // check if softkey pressed
    int index = -1;
    switch(event->key()) {
    case Qt::Key_F1: index = 0; break;
    case Qt::Key_F2: index = 1; break;
    case Qt::Key_F3: index = 2; break;
    case Qt::Key_F4: index = 3; break;
    case Qt::Key_F5: index = 4; break;
    case Qt::Key_F6: index = 5; break;
    case Qt::Key_F7: index = 6; break;
    case Qt::Key_F8: index = 7; break;
    }
    if(index >= 0) {
        auto w = m_layout->itemAt(index);
        w->widget()->setFocus();
        items[index]->userSelected();
        event->accept();
    } else if(event->key() == Qt::Key_Escape) {
        if(parent) {
            // got a parent menu
            parent->m_containingLayout.setCurrentWidget(parent);
        }
        event->accept();
    }
}


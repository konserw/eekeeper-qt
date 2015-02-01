#ifndef SPELLBROWSERWIDGET_H
#define SPELLBROWSERWIDGET_H

#include <QWidget>

namespace Ui {
class SpellBrowserWidget;
}

class SpellBrowserWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit SpellBrowserWidget(QWidget *parent = 0);
    ~SpellBrowserWidget();
    
private:
    Ui::SpellBrowserWidget *ui;
};

#endif // SPELLBROWSERWIDGET_H

#ifndef ITEMBROWSERWIDGET_H
#define ITEMBROWSERWIDGET_H

#include <QWidget>

namespace Ui {
  class ItemBrowserWidget;
}

class ItemBrowserWidget : public QWidget
{
  Q_OBJECT
  
public:
  explicit ItemBrowserWidget(QWidget *parent = 0);
  ~ItemBrowserWidget();
  
private:
  Ui::ItemBrowserWidget *ui;
};

#endif // ITEMBROWSERWIDGET_H

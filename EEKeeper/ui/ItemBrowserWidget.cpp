#include "ItemBrowserWidget.h"
#include "ui_ItemBrowserWidget.h"

ItemBrowserWidget::ItemBrowserWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ItemBrowserWidget)
{
  ui->setupUi(this);
}

ItemBrowserWidget::~ItemBrowserWidget()
{
  delete ui;
}

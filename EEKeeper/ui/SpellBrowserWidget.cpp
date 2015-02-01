#include "SpellBrowserWidget.h"
#include "ui_SpellBrowserWidget.h"

SpellBrowserWidget::SpellBrowserWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpellBrowserWidget)
{
    ui->setupUi(this);
}

SpellBrowserWidget::~SpellBrowserWidget()
{
    delete ui;
}

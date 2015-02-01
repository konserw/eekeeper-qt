#include "SaveGameNameDialog.h"
#include "ui_SaveGameNameDialog.h"

#include <QDir>

SaveGameNameDialog::SaveGameNameDialog(QWidget *parent, QString strGameName) :
    QDialog(parent),
    ui(new Ui::SaveGameNameDialog)
{
    ui->setupUi(this);

    while (strGameName.endsWith(QDir::separator()))
        strGameName.chop(1);

    ui->lineEditSaveGameName->setText(strGameName);
}

SaveGameNameDialog::~SaveGameNameDialog()
{
    delete ui;
}

void SaveGameNameDialog::on_buttonBox_accepted()
{
    m_bOverwrite = ui->checkBoxOverwrite->isChecked();
    m_strSaveGameName = ui->lineEditSaveGameName->text();
}

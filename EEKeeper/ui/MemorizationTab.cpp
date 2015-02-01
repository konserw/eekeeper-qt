#include "MemorizationTab.h"
#include "ui_MemorizationTab.h"

MemorizationTab::MemorizationTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MemorizationTab)
{
    ui->setupUi(this);
}

MemorizationTab::~MemorizationTab()
{
    delete ui;
}

void MemorizationTab::SetData(MEMINFO *pMemInfo, int nCount)
{
    // clear the table
    ui->tableWidget->clear();

    QString strType, strNum, strLevel;

    for (int i=0; i<nCount; i++)
    {
        switch(pMemInfo[i].wType)
        {
        case INF_CRE_ST_INNATE :
            strType = "Innate";
            break;
        case INF_CRE_ST_WIZARD :
            strType = "Wizard";
            break;
        case INF_CRE_ST_PRIEST :
            strType = "Priest";
            break;
        default :
            strType = "Unknown";	// Would be bad to have this happen.
            break;
        }

        strNum   = QString::number(pMemInfo[i].wNumMemorizable);
        strLevel = QString::number(pMemInfo[i].wLevel+1);

        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(strType) );
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(strLevel) );
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(strNum) );
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString::number(pMemInfo[i].wType) ) );
    }

    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Type") ) );
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Level") ) );
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Max Can Learn") ) );
}

void MemorizationTab::on_pushButtonMaxAdd_clicked()
{
    ModifyMax(1);
}

void MemorizationTab::on_pushButtonMaxMinus_clicked()
{
    ModifyMax(-1);
}

void MemorizationTab::ModifyMax(int mod)
{
    if (ui->tableWidget->selectedItems().empty())
        return;

    // grab the correct row value
    bool ok;
    int nRow = ui->tableWidget->currentRow();
    QTableWidgetItem *pMax = ui->tableWidget->item(nRow, 2);
    int nMax = pMax->text().toInt(&ok);

    if (!ok)
        return;

    // update the field
    nMax += mod;
    nMax  = VALIDATE_RANGE(nMax, 0, 100);
    pMax->setText(QString::number(nMax));
}

void MemorizationTab::GetData(MEMINFO *pMemInfo, int nCount)
{
    int nListCount = ui->tableWidget->rowCount();

    for (int i=0; i<nCount && i<nListCount; i++)
    {
        pMemInfo[i].wLevel = ui->tableWidget->item(i, 1)->text().toUInt() - 1;
        pMemInfo[i].wNumMemorizable = ui->tableWidget->item(i, 2)->text().toUInt();
        pMemInfo[i].wType = ui->tableWidget->verticalHeaderItem(i)->text().toUInt();
    }
}

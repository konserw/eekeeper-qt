#include "ProficienciesTab.h"
#include "ui_ProficienciesTab.h"

#include "EEKeeper.h"

ProficienciesTab::ProficienciesTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProficienciesTab)
{
    ui->setupUi(this);

    // PROFICIENCIES
    ui->tableProficiencies->horizontalHeader()->setVisible(true);
    ui->tableProficiencies->setColumnWidth(0, 200);
    ui->tableProficiencies->setColumnWidth(1, 75);
}

ProficienciesTab::~ProficienciesTab()
{
    delete ui;
}

void ProficienciesTab::FillList(const CValueList &vlProf)
{
    int nCount = 0;
    ui->tableProficiencies->setRowCount(0);

    // add all our profs to the table
    for (CValueList::const_iterator it = vlProf.begin(); it != vlProf.end(); ++it) {
        TValueItem item = (*it);

        // create our data
        PROFDATA pData;
        pData.chProf = item.dwValue;
        pData.nFirstClass = 0;
        pData.nSecondClass = 0;

        // figure out where we'll be inserting
        int nRow = 0;
        while (nRow < nCount && item.strText.compare(ui->tableProficiencies->item(nRow, 0)->text()) > 0) {
            ++nRow;
        }


        // add our proficiencies to the table
        ui->tableProficiencies->insertRow(nRow);
        ui->tableProficiencies->setItem(nRow, 0, new QTableWidgetItem(item.strText) );
        ui->tableProficiencies->setItem(nRow, 1, new QTableWidgetItem(QString("") ) );
        ui->tableProficiencies->setItem(nRow, 2, new QTableWidgetItem(QString("") ) );

        // add our data
        QTableWidgetItem *pHeaderItem = new QTableWidgetItem(QTableWidgetItem::UserType);
        pHeaderItem->setData(Qt::UserRole, pData);
        ui->tableProficiencies->setVerticalHeaderItem(nRow, pHeaderItem);

        ++nCount;
    }
}

void ProficienciesTab::SetProfs(CProfDataPlist &list)
{
    for (CProfDataPlist::iterator it = list.begin(); it != list.end(); ++it) {
        CProfData *pData = (*it);
        UpdateCount(pData->m_chProf, pData->m_nFirstClass, pData->m_nSecondClass);

        // we delete here because the list was populated via new's for us
        delete pData;
    }

    // clear the list
    list.clear();
}

void ProficienciesTab::UpdateCount(quint8 chProf, int nFirstClass, int nSecondClass)
{
    int nCount = ui->tableProficiencies->rowCount();
    for (int i=0; i<nCount; i++)
    {
        PROFDATA pData;
        if (GetProfData(i, pData) && pData.chProf == chProf)
        {
            QTableWidgetItem *pFirstClassItem = ui->tableProficiencies->item(i, 1);
            QTableWidgetItem *pSecondClassItem = ui->tableProficiencies->item(i, 2);
            QString tmp;

            if (nFirstClass < 1) {
                pFirstClassItem->setText("");
            } else {
                tmp = QString::number(nFirstClass);
                pFirstClassItem->setText(tmp);
            }

            if (nSecondClass < 1)
                pSecondClassItem->setText("");
            else {
                tmp = QString::number(nSecondClass);
                pSecondClassItem->setText(tmp);
            }

            pData.nFirstClass = nFirstClass;
            pData.nSecondClass = nSecondClass;
            ui->tableProficiencies->verticalHeaderItem(i)->setData(Qt::UserRole, pData);

            return;
        }
    }
}

void ProficienciesTab::ModifyCount(int nModFirstClass, int nModSecondClass)
{
    if (ui->tableProficiencies->selectedItems().empty())
        return;

    PROFDATA pData;
    if (GetProfData(ui->tableProficiencies->currentRow(), pData)) {
        pData.nFirstClass += nModFirstClass;
        pData.nSecondClass += nModSecondClass;

        pData.nFirstClass = VALIDATE_RANGE(pData.nFirstClass, 0, 5);
        pData.nSecondClass = VALIDATE_RANGE(pData.nSecondClass, 0, 5);

        UpdateCount(pData.chProf, pData.nFirstClass, pData.nSecondClass);
    }
}

bool ProficienciesTab::GetProfData(int row, PROFDATA &pData)
{
    pData = ui->tableProficiencies->verticalHeaderItem(row)->data(Qt::UserRole).value<PROFDATA>();
    return true;
}

void ProficienciesTab::GetProfs(CProfDataPlist &list)
{
    CProfData *pProfData;
    int nCount = ui->tableProficiencies->rowCount();

    for (int i=0; i < nCount; i++)
    {
        PROFDATA pItemData;
        if (!GetProfData(i, pItemData))
            continue;

        // only store the profs that have values
        if (pItemData.nFirstClass || pItemData.nSecondClass)
        {
            pProfData = new CProfData;
            pProfData->m_chProf = pItemData.chProf;
            pProfData->m_nFirstClass = pItemData.nFirstClass;
            pProfData->m_nSecondClass = pItemData.nSecondClass;

            list.append(pProfData);
        }
    }
}

void ProficienciesTab::on_pushButtonAddFirst_clicked()
{
    ModifyCount(1, 0);
}

void ProficienciesTab::on_pushButtonAddSecond_clicked()
{
    ModifyCount(0, 1);
}

void ProficienciesTab::on_pushButtonSubFirst_clicked()
{
    ModifyCount(-1, 0);
}

void ProficienciesTab::on_pushButtonSubSecond_clicked()
{
    ModifyCount(0, -1);
}

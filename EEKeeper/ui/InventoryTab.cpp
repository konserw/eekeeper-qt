#include "InventoryTab.h"
#include "ui_InventoryTab.h"

#include "EEKeeper.h"
#include <QMessageBox>

InventoryTab::InventoryTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InventoryTab)
{
    ui->setupUi(this);

    // TABLE INVENTORY
    ui->tableInventory->horizontalHeader()->setVisible(true);
    ui->tableInventory->setColumnWidth(0, 40);
    ui->tableInventory->setColumnWidth(1, 80);
    ui->tableInventory->setColumnWidth(2, 48);
    ui->tableInventory->setColumnWidth(3, 128);
}

InventoryTab::~InventoryTab()
{
    delete ui;
}

void InventoryTab::ClearItems()
{
    int nCount = ui->tableInventory->rowCount();
    for (int i=0; i<nCount; i++)
    {
        QString blank("");
        ui->tableInventory->setItem(i, 0, NULL);
        ui->tableInventory->item(i, 2)->setText(blank);
        ui->tableInventory->item(i, 3)->setText(blank);
        ui->tableInventory->item(i, 4)->setText(blank);
    }

    memset(&m_infItm[0], 0, sizeof(INF_ITM)*INF_NUM_ITEMSLOTS);
    memset(&m_infItems[0], 0, sizeof(INF_CRE_ITEM)*INF_NUM_ITEMSLOTS);
}

void InventoryTab::SetItems(INF_CRE_ITEM *pItems)
{
    QImage   hBitmap;
    QString  strName;
    QString  strQty;
    QString  strNotFound;
    char     szResName[9];
    CResInfo *pResInfo;
    int i;

    // clear out our view and copy in the new items
    ClearItems();
    memcpy(&m_infItems[0], pItems, sizeof(INF_CRE_ITEM)*INF_NUM_ITEMSLOTS);

    // fill the view
    ui->tableInventory->setRowCount(INF_NUM_ITEMSLOTS-1);
    for(i=0; i<INF_NUM_ITEMSLOTS-1; i++)
    {
        ui->tableInventory->setItem(i, 1, new QTableWidgetItem(QString(_itemPos[i].pszPos)));

        if (m_infItems[i].chResName[0])
        {
            pResInfo = EEKeeper::GetItem(m_infItems[i].chResName, &m_infItm[i]);
            if (!pResInfo)
            {
                if (!strNotFound.isEmpty())
                    strNotFound += ",";
                memcpy(szResName, m_infItems[i].chResName, 8);
                szResName[8] = '\x0';
                strNotFound += szResName;
                continue;
            }

            if (EEKeeper::GetItemBitmap(pResInfo, m_infItm[i], 0, qRgba(0, 0, 0, 0), hBitmap))
            {
                QIcon icon(QPixmap::fromImage(hBitmap));
                ui->tableInventory->setItem(i, 0, new QTableWidgetItem(icon, ""));
            }

            if (!_infTlk.GetString(m_infItm[i].dwIdentifiedItemName, strName))
                _infTlk.GetString(m_infItm[i].dwGenericItemName, strName);

            strQty = QString("%1/%2/%3")
                    .arg(m_infItems[i].wQuantity1)
                    .arg(m_infItems[i].wQuantity2)
                    .arg(m_infItems[i].wQuantity3);

            ui->tableInventory->setItem(i, 2, new QTableWidgetItem(strQty));
            ui->tableInventory->setItem(i, 3, new QTableWidgetItem(strName));

            memcpy(szResName, m_infItems[i].chResName, 8);
            szResName[8] = '\x0';
            ui->tableInventory->setItem(i, 4, new QTableWidgetItem(QString(szResName)));
        }
    }

    // update our buttons
    //UpdateButtonStates();

    if (!strNotFound.isEmpty())
    {
        QString message = tr("Some items assigned to this character were not found in the "
                             "database. If you save this character, those items will not be saved.\n\n"
                             "Cannot find: ") + strNotFound;

        QMessageBox::warning(
                    this,
                    tr("Unknown items"),
                    message
                    );
    }
}

void InventoryTab::GetItems(INF_CRE_ITEM *pItems)
{
    memcpy(pItems, &m_infItems[0], sizeof(INF_CRE_ITEM)*INF_NUM_ITEMSLOTS);
}

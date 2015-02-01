#include "SpellTab.h"
#include "ui_SpellTab.h"

#include <QMessageBox>

SpellTab::SpellTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpellTab),
    m_wSpellType(0),
    m_bInnates(false)
{
    ui->setupUi(this);

    // resize some of our fields
    ui->tableWidget->setColumnWidth(3, 150);

    // hide our data field
    ui->tableWidget->hideColumn(5);
}

SpellTab::~SpellTab()
{
    delete ui;
}

void SpellTab::ClearList()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
}

bool SpellTab::AddSpellToList(SPELLDATA *pData)
{
    CResInfo *pResInfo;
    CInfBifFile *pBifFile;
    QString strSpell;
    INF_SPL spl;
    char szSpellIcon[10];
    QIcon icon;

    memset(szSpellIcon, 0, 10);
    pResInfo = _infKey.GetResInfo(RESTYPE_SPL, pData->szResName);
    if (pResInfo)
    {
        pBifFile = _infKey.GetBifFile(pResInfo);
        if (pBifFile && pBifFile->IsValid() && pBifFile->GetData(pResInfo->wLocator, &spl) != -1)
        {
            _infTlk.GetString(spl.dwGenericSpellName, strSpell);
            memcpy(szSpellIcon, spl.chSpellIcon, 8);
            _spellBitmaps.GetIcon(QString(szSpellIcon), icon);
        }
        else
            return(false);
    }
    else
        return(false);

    QString strLevel = QString::number(pData->wLevel+1);
    QString strMem   = QString::number(pData->nTimesMemorized);
    int nIndex = ui->tableWidget->rowCount();

    ui->tableWidget->insertRow(nIndex);
    ui->tableWidget->setItem(nIndex, 0, new QTableWidgetItem(icon, ""));
    ui->tableWidget->setItem(nIndex, 1, new QTableWidgetItem(strLevel));
    ui->tableWidget->setItem(nIndex, 2, new QTableWidgetItem(strMem));
    ui->tableWidget->setItem(nIndex, 3, new QTableWidgetItem(strSpell));
    ui->tableWidget->setItem(nIndex, 4, new QTableWidgetItem(pData->szResName));

    SpellTabData pTabData;
    pTabData.chLevel = (uchar)pData->wLevel;
    pTabData.chMem = (uchar)pData->nTimesMemorized;
    pTabData.chTimesCanCast = (uchar)pData->nTimesCanCast;
    pTabData.strName = strSpell;
    pTabData.pInfo = pResInfo;

    QTableWidgetItem *pHeader = new QTableWidgetItem(QTableWidgetItem::UserType);
    pHeader->setData(Qt::UserRole, pTabData);
    ui->tableWidget->setItem(nIndex, 5, pHeader);

    return(true);
}

void SpellTab::SetSpells(quint16 wType, int nSpellCount, SPELLDATA *pData)
{
    ClearList();
    m_wSpellType = wType;

    if (m_wSpellType == INF_CRE_ST_INNATE) {
        m_bInnates = true;
    }

    QString strNotFound("");
    for (int i=0; i<nSpellCount; i++)
    {
        if (!AddSpellToList(pData+i)) {
            if (strNotFound.isEmpty())
                strNotFound += pData[i].szResName;
            else
                strNotFound += QString(", ") + pData[i].szResName;
        }
    }

    // sort by level (secondary sort, name) and resize by contents
    ui->tableWidget->sortByColumn(3, Qt::AscendingOrder);
    ui->tableWidget->sortByColumn(1, Qt::AscendingOrder);
    ui->tableWidget->resizeColumnsToContents();

    UpdateButtonStates();

    // inform the user if we're unable to find some spells
    if (!strNotFound.isEmpty())
    {
        QMessageBox::warning(
                    this,
                    tr("Unknown spells"),
                    tr("Some spells assigned to this character were not found in the database. If you save this character those spells will not be saved.")
                    + QString("\n\nCannot find: ") + strNotFound
                    );
    }
}

void SpellTab::UpdateButtonStates()
{
    //ASSERT(GetParentFrame());

    ui->pushButtonAddSpell->setEnabled(true);

    if (ui->tableWidget->rowCount() == 0)
    {
        ui->pushButtonMemAdd->setEnabled(false);
        ui->pushButtonMemMinus->setEnabled(false);
        ui->pushButtonRemoveSpell->setEnabled(false);
    }
    else
    {
        ui->pushButtonMemAdd->setEnabled(true);
        ui->pushButtonMemMinus->setEnabled(true);
        ui->pushButtonRemoveSpell->setEnabled(true);
    }

    // do we need to show our kit button?
    ui->pushButtonKit->setVisible(m_bInnates);
    ui->pushButtonKit->setEnabled(m_bInnates);

    //_comSpellBrowser.SetSelected(m_strGameName,m_strCharName,m_pParentView);
}

void SpellTab::ModifySpellMem(int mod)
{
    // make sure we have something selected
    if (ui->tableWidget->selectedItems().empty())
        return;

    int nRow = ui->tableWidget->currentRow();
    QTableWidgetItem *pHeader = ui->tableWidget->item(nRow, 5);
    if (!pHeader)
        return;

    // decrement our data and return it to the header
    SpellTabData data = pHeader->data(Qt::UserRole).value<SpellTabData>();
    data.chMem += mod;
    data.chMem = VALIDATE_RANGE(((signed char)data.chMem), 0, TABSPELL_MAXMEM);
    pHeader->setData(Qt::UserRole, data);

    ui->tableWidget->item(nRow, 2)->setText(QString::number(data.chMem));
}

int SpellTab::GetSpellCount()
{
    return ui->tableWidget->rowCount();
}

void SpellTab::GetSpells(SPELLDATA *pData)
{
    // Resort it back to level ascending order.
    //m_lcSpells.SortItems(SpellTabListComp,TABSPELLSORT_LVL_ASC);

    SpellTabData pSpellData;
    int nCount = GetSpellCount();

    for (int i=0; i<nCount; i++)
    {
        pSpellData = ui->tableWidget->item(i, 5)->data(Qt::UserRole).value<SpellTabData>();

        pData[i].wLevel = pSpellData.chLevel;
        pData[i].wType = m_wSpellType;
        pData[i].nTimesCanCast = pSpellData.chTimesCanCast;
        pData[i].nTimesMemorized = pSpellData.chMem;

        qstrncpy(pData[i].szResName, pSpellData.pInfo->strName.toLatin1().constData(), 9);
        pData[i].szResName[8] = '\x0';
    }
}

void SpellTab::on_pushButtonAddSpell_clicked()
{

}

void SpellTab::on_pushButtonRemoveSpell_clicked()
{

}

void SpellTab::on_pushButtonMemMinus_clicked()
{
    ModifySpellMem(-1);
}

void SpellTab::on_pushButtonMemAdd_clicked()
{
    ModifySpellMem(1);
}

void SpellTab::on_pushButtonKit_clicked()
{

}

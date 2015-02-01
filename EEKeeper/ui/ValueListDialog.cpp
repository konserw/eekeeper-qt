#include "ValueListDialog.h"
#include "ui_ValueListDialog.h"

#include "EEKeeper.h"
#include "ValueItemDialog.h"
#include <QMessageBox>

ValueListDialog::ValueListDialog(const CValueList &vl, quint32 dwMin, quint32 dwMax, const QString &strTitle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ValueListDialog),
    m_vl(vl),
    m_dwMin(dwMin),
    m_dwMax(dwMax),
    m_nFieldWidth(2)
{
    ui->setupUi(this);

    // ensure correct min/max
    if (m_dwMin > m_dwMax) {
        quint32 dwTmp = m_dwMin;
        m_dwMin = m_dwMax;
        m_dwMax = dwTmp;
    }

    // determine field width
    if (m_dwMax > 0x00FFFFFF) {
        m_nFieldWidth = 8;
    } else if (m_dwMax > 0x0000FFFF) {
        m_nFieldWidth = 6;
    } else if (m_dwMax > 0x000000FF) {
        m_nFieldWidth = 4;
    }

    // give a title
    this->setWindowTitle(tr("Edit List - %1").arg(strTitle));

    // must be modal on the application level
    this->setWindowModality(Qt::ApplicationModal);

    // add a 3rd, hidden column for our data
    ui->tableWidget->insertColumn(2);
    ui->tableWidget->hideColumn(2);

    // fill in our list
    if (!m_vl.isEmpty()) {
        TValueItem vi;
        int nRow = 0;
        QString strHex;
        for (CValueList::iterator it = m_vl.begin(); it != m_vl.end(); ++it) {
            vi = (*it);
            EEKeeper::MakeHexString(vi.dwValue, strHex, m_nFieldWidth);

            // show the data in a human-readable way
            ui->tableWidget->insertRow(nRow);
            ui->tableWidget->setItem(nRow, 0, new QTableWidgetItem(vi.strText) );
            ui->tableWidget->setItem(nRow, 1, new QTableWidgetItem(strHex) );

            // store our data
            QTableWidgetItem *pData = new QTableWidgetItem(QTableWidgetItem::UserType);
            pData->setData(Qt::UserRole, vi);
            ui->tableWidget->setItem(nRow, 2, pData);

            ++nRow;
        }

        // select our top row
        if (nRow) {
            ui->tableWidget->selectRow(0);
        }
    }

    // reset the UI status
    this->ResetStatus();
}

ValueListDialog::~ValueListDialog()
{
    delete ui;
}

void ValueListDialog::FillValueList(CValueList &vl)
{
    // clear the old list and fill with our new list
    vl.clear();
    vl.append(m_vl);
}

void ValueListDialog::ResetStatus()
{
    bool bEnabled = !m_vl.isEmpty();

    ui->tableWidget->setEnabled(bEnabled);
    ui->pushButtonDelete->setEnabled(bEnabled);
    ui->pushButtonEdit->setEnabled(bEnabled);
}

int ValueListDialog::ExecuteValueItemDialog(TValueItem &item, bool bAdd)
{
    // make sure we have a value selected if we're not adding
    if (!bAdd && ui->tableWidget->selectedItems().isEmpty())
        return QDialog::Rejected;

    // create the correct dialog box
    ValueItemDialog *pDialog = NULL;
    if (bAdd) {
        pDialog = new ValueItemDialog(this, m_dwMin, m_dwMax);
    } else {
        int nRow = ui->tableWidget->currentRow();
        TValueItem vi = ui->tableWidget->item(nRow, 2)->data(Qt::UserRole).value<TValueItem>();
        pDialog = new ValueItemDialog(this, m_dwMin, m_dwMax, vi.strText, vi.dwValue);
    }

    // ensure it was created
    if (!pDialog)
        return QDialog::Rejected;

    // fill our value item
    int nRet = pDialog->exec();
    if (nRet == QDialog::Accepted) {
        pDialog->GetDisplayText(item.strText);
        item.dwValue = pDialog->GetValue();
    }

    // delete the dialog
    delete pDialog;
    pDialog = NULL;

    return nRet;
}

void ValueListDialog::on_pushButtonAdd_clicked()
{
    // show the user the value item dialog and get their response
    TValueItem vi;
    if (this->ExecuteValueItemDialog(vi, true) == QDialog::Accepted) {
        // no need for empty values
        if (vi.strText.isEmpty())
            return;

        QString strHex;
        EEKeeper::MakeHexString(vi.dwValue, strHex, m_nFieldWidth);

        // see if it's already in there and warn
        QList<QTableWidgetItem *> matches = ui->tableWidget->findItems(vi.strText, Qt::MatchFixedString);
        if (!matches.isEmpty() ) {
            if ( QMessageBox::question(
                        this,
                        tr("Name already in the list"),
                        tr("'%1' is already in the list. Would you like to update the value instead?").arg(vi.strText)
                     ) == QMessageBox::No ) {
                return;
            }

            // keep on keeping on!
            int nRow = matches.first()->row();
            if (nRow == -1)
                return;

            // update our current index
            int nIndex = m_vl.indexOf(ui->tableWidget->item(nRow, 2)->data(Qt::UserRole).value<TValueItem>());
            if (nIndex == -1)
                m_vl.append(vi);
            else
                m_vl.replace(nIndex, vi);

            // update the value and the data
            ui->tableWidget->item(nRow, 1)->setText(strHex);
            ui->tableWidget->item(nRow, 2)->setData(Qt::UserRole, vi);

            return;
        }

        // add the row and info
        int nRow = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(nRow);
        ui->tableWidget->setItem(nRow, 0, new QTableWidgetItem(vi.strText));
        ui->tableWidget->setItem(nRow, 1, new QTableWidgetItem(strHex) );

        // add our data
        QTableWidgetItem *pData = new QTableWidgetItem(QTableWidgetItem::UserType);
        pData->setData(Qt::UserRole, vi);
        ui->tableWidget->setItem(nRow, 2, pData);
        m_vl.append(vi);

        // re-sort
        ui->tableWidget->sortByColumn(0, Qt::AscendingOrder);
    }

    // reset the UI status
    this->ResetStatus();
}

void ValueListDialog::on_pushButtonEdit_clicked()
{
    // show the user the value item dialog and get their response
    TValueItem vi;
    if (this->ExecuteValueItemDialog(vi) == QDialog::Accepted) {
        // we don't care about empty values
        if (vi.strText.isEmpty())
            return;

        // we need to know the currently selected item to update it
        int nRow = ui->tableWidget->currentRow();
        QString strHex;
        EEKeeper::MakeHexString(vi.dwValue, strHex, m_nFieldWidth);

        // update our current index
        int nIndex = m_vl.indexOf(ui->tableWidget->item(nRow, 2)->data(Qt::UserRole).value<TValueItem>());
        if (nIndex == -1)
            return; // orops, not found
        else
            m_vl.replace(nIndex, vi);

        // update the values
        ui->tableWidget->item(nRow, 0)->setText(vi.strText);
        ui->tableWidget->item(nRow, 1)->setText(strHex);
        ui->tableWidget->item(nRow, 2)->setData(Qt::UserRole, vi);

        // order
        ui->tableWidget->sortByColumn(0, Qt::AscendingOrder);
    }

    // reset the UI status
    this->ResetStatus();
}

void ValueListDialog::on_pushButtonDelete_clicked()
{
    // make sure we have something selected
    if (ui->tableWidget->selectedItems().isEmpty())
        return;

    // delete the selected value from the list and table
    int nRow = ui->tableWidget->currentRow();
    TValueItem vi = ui->tableWidget->item(nRow, 2)->data(Qt::UserRole).value<TValueItem>();

    m_vl.removeOne(vi);
    ui->tableWidget->removeRow(nRow);

    // update the status
    this->ResetStatus();

    // select the row that falls into place
    int nRowCount = ui->tableWidget->rowCount();
    if (nRowCount > 0) {
        if (nRow >= nRowCount)
            nRow = nRowCount - 1;

        ui->tableWidget->selectRow(nRow);
    }
}

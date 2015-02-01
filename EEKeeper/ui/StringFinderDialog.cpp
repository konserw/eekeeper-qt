#include "StringFinderDialog.h"
#include "ui_StringFinderDialog.h"

#include "EEKeeper.h"
#include <QIntValidator>
#include <QMessageBox>
#include <QRegExpValidator>

StringFinderDialog::StringFinderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StringFinderDialog)
{
    ui->setupUi(this);

    // setup the initial widths
    ui->tableStrings->setColumnWidth(0, 300);
    //ui->tableStrings->setColumnWidth(1, 85);

    // add a validator to our max length
    ui->lineMaxLength->setValidator(new QIntValidator(0, 1000));
}

StringFinderDialog::~StringFinderDialog()
{
    delete ui;
}

void StringFinderDialog::on_pushButtonFindStrings_clicked()
{
    qDebug() << "Finding strings.";

    quint32 dwMaxLength = ui->lineMaxLength->text().isEmpty() ? 0 : ui->lineMaxLength->text().toUInt();
    QString strTextMatch = ui->lineTextMatch->text().trimmed();

    // warn about a potentially large search field
    if (strTextMatch.isEmpty() || !dwMaxLength || dwMaxLength > 100)
    {
        if ( QMessageBox::warning(
                    this,
                    tr("Potentially a lot of matches"),
                    tr("You have not selected any criteria, or a large maximum length. "
                       "This will generate a lot of entries in the list. The string file is large "
                       "(more than 7MB) -- this operation might take a while and may take a large "
                       "chunk of memory.\n\nContinue filling the list?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No ) == QMessageBox::No ) {
            return;
        }
    }

    // clear the current list
    ui->tableStrings->clearContents();
    ui->tableStrings->setRowCount(0);

    QString strText;
    char szRef[100];
    int nItem = 0;
    QRegExp rx("^[A-za-z0-9\\s]+$");

    for (quint32 dwIndex=0; dwIndex < _infTlk.m_tlkHeader.dwStringCount; dwIndex++)
    {
        //if (_infTlk.m_pEntries[dwIndex].wUnknown != 0x03)
          //  continue;

        // Good idea to pick a length when filtering. The lengths are available
        // without having to hit the disk (fast).
        if (dwMaxLength && _infTlk.m_pEntries[dwIndex].dwLength > dwMaxLength)
            continue;

        if (!_infTlk.GetString(dwIndex, strText))
            continue;

        strText = strText.trimmed();
        if (strText.isEmpty())
            continue;

        // make sure it's alphanumberic, if needed
        if (ui->checkBoxAlphanumeric->isChecked())
        {
            if (!rx.exactMatch(strText)) {
                continue;
            }
        }

        if (!strTextMatch.isEmpty() && !strText.contains(strTextMatch, Qt::CaseInsensitive))
            continue;

        EEKeeper::MakeHexString(dwIndex, szRef, 8);
        ui->tableStrings->insertRow(nItem);
        ui->tableStrings->setItem(nItem, 0, new QTableWidgetItem(strText) );
        ui->tableStrings->setItem(nItem, 1, new QTableWidgetItem(QString(szRef) ) );
        ++nItem;
    }

    // update our strings found field
    ui->labelNumStringsFound->setText(QString::number(nItem));
}

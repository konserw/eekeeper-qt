#include "ValueItemDialog.h"
#include "ui_ValueItemDialog.h"

#include "EEKeeper.h"
#include <QRegExpValidator>

ValueItemDialog::ValueItemDialog(QWidget *parent, quint32 dwMin, quint32 dwMax, const QString &strDisplayText, quint32 dwValue) :
    QDialog(parent),
    ui(new Ui::ValueItemDialog),
    m_strDisplayText(strDisplayText),
    m_dwValue(dwValue)
{
    ui->setupUi(this);

    // are we adding or editing?
    if (strDisplayText.isEmpty()) {
        this->setWindowTitle(tr("Add List Item") );
    } else {
        this->setWindowTitle(tr("Edit List Item: %1").arg(m_strDisplayText) );
    }

    // setup the UI
    ui->lineEditDisplayText->setText(strDisplayText);
    ui->lineEditValue->setText(QString::number(dwValue, 16).toUpper() );
    this->SetRange(dwMin, dwMax);
}

ValueItemDialog::~ValueItemDialog()
{
    delete ui;
}

void ValueItemDialog::SetRange(quint32 nMin, quint32 nMax)
{
    // ensure correct ordering
    if (nMin > nMax) {
        quint32 nTmp = nMin;
        nMin = nMax;
        nMax = nTmp;
    }

    // how many hex values should we allow?
    int nFieldWidth = 2;
    if (nMax > 0x00FFFFFF) {
        nFieldWidth = 8;
    } else if (nMax > 0x0000FFFF) {
        nFieldWidth = 6;
    } else if (nMax > 0x000000FF) {
        nFieldWidth = 4;
    }

    // get the hex strings for the ranges
    QString strMinHex, strMaxHex;
    EEKeeper::MakeHexString(nMin, strMinHex, nFieldWidth);
    EEKeeper::MakeHexString(nMax, strMaxHex, nFieldWidth);

    // show the range
    QString strRange = tr("%1 - %2").arg(strMinHex).arg(strMaxHex);
    ui->labelRange->setText(strRange);

    // add a validator
    ui->lineEditValue->setValidator(new QRegExpValidator(QRegExp(QString("[0-9A-Fa-f]{2,%1}").arg(nFieldWidth) ) ) );
}

void ValueItemDialog::on_buttonBox_accepted()
{
    m_strDisplayText = ui->lineEditDisplayText->text();
    m_dwValue = ui->lineEditValue->text().toUInt(NULL, 16);
}

void ValueItemDialog::on_buttonBox_rejected()
{
}

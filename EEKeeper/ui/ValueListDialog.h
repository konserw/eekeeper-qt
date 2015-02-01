#ifndef VALUELISTDIALOG_H
#define VALUELISTDIALOG_H

#include "ValueList.h"
#include <QDialog>

namespace Ui {
class ValueListDialog;
}

class ValueListDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ValueListDialog(const CValueList &vl, quint32 dwMin = 0, quint32 dwMax = 0xFFFFFFFF, const QString &strTitle = "", QWidget *parent = 0);
    ~ValueListDialog();

    // Clears the given value list and fills it with the new values we obtained
    void FillValueList(CValueList &vl);
    
private slots:
    void on_pushButtonAdd_clicked();

    void on_pushButtonEdit_clicked();

    void on_pushButtonDelete_clicked();

private:
    Ui::ValueListDialog *ui;
    CValueList  m_vl;
    quint32     m_dwMin;
    quint32     m_dwMax;
    int         m_nFieldWidth;

    // Resets the UI status (enables what should be enabled)
    void ResetStatus();

    // Opens the ValueItemDialog for Adding or Editting
    int  ExecuteValueItemDialog(TValueItem &item, bool bAdd = false);
};

#endif // VALUELISTDIALOG_H

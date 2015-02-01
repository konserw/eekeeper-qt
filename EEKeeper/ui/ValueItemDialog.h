#ifndef VALUEITEMDIALOG_H
#define VALUEITEMDIALOG_H

#include <QDialog>

namespace Ui {
class ValueItemDialog;
}

class ValueItemDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ValueItemDialog(QWidget *parent = 0, quint32 dwMin = 0, quint32 dwMax = 0, const QString &strDisplayText = "", quint32 dwValue = 0);
    ~ValueItemDialog();

    void SetRange(quint32 nMin, quint32 nMax);

    void GetDisplayText(QString &strDisplayText) { strDisplayText = m_strDisplayText; }
    quint32 GetValue() const { return m_dwValue; }
    
private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ValueItemDialog *ui;

    QString m_strDisplayText;
    quint32 m_dwValue;
};

#endif // VALUEITEMDIALOG_H

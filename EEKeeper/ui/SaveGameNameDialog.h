#ifndef SAVEGAMENAMEDIALOG_H
#define SAVEGAMENAMEDIALOG_H

#include <QDialog>

namespace Ui {
class SaveGameNameDialog;
}

class SaveGameNameDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SaveGameNameDialog(QWidget *parent = 0, QString strGameName = "");
    ~SaveGameNameDialog();

    bool CanOverwrite() const { return m_bOverwrite; }
    const QString &SaveGameName() const { return m_strSaveGameName; }
    
private slots:
    void on_buttonBox_accepted();

private:
    Ui::SaveGameNameDialog *ui;
    QString m_strSaveGameName;
    bool m_bOverwrite;
};

#endif // SAVEGAMENAMEDIALOG_H

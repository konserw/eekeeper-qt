#ifndef INSTALLATIONDIRECTORY_H
#define INSTALLATIONDIRECTORY_H

#include <QDialog>

namespace Ui {
class InstallationDirectory;
}

class InstallationDirectory : public QDialog
{
    Q_OBJECT
    
public:
    explicit InstallationDirectory(QWidget *parent = 0);
    ~InstallationDirectory();
    
private slots:
    void on_buttonOk_clicked();

    void on_buttonCancel_clicked();

    void on_buttonBrowse_clicked();

private:
    Ui::InstallationDirectory *ui;
};

#endif // INSTALLATIONDIRECTORY_H

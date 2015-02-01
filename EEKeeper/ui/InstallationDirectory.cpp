#include "InstallationDirectory.h"
#include "ui_InstallationDirectory.h"
#include "EEKeeper.h"

#include <QFileDialog>
#include <QMessageBox>

InstallationDirectory::InstallationDirectory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallationDirectory)
{
    ui->setupUi(this);

    // ensure that we set the current index of the language selector
    if (!_strLanguage.isEmpty() ) {
        ui->comboLanguage->setCurrentText(_strLanguage);
    }

    // show the currently selected installation directory
    if (!_strInstallPath.isEmpty() ) {
        ui->lineDirectory->setText(_strInstallPath);
    }
}

InstallationDirectory::~InstallationDirectory()
{
    delete ui;
}

void InstallationDirectory::on_buttonOk_clicked()
{
    // make the changes, if needed, and warn the user to restart the application to take effect
    bool hasChanged = _strInstallPath.compare(ui->lineDirectory->text() ) != 0 || _strLanguage.compare(ui->comboLanguage->currentText() ) != 0;
    if (hasChanged) {
        _strInstallPath = ui->lineDirectory->text();
        _strLanguage = ui->comboLanguage->currentText();

        // ensure the _strInstallPath ends with a QDir::seperator
        if (!_strInstallPath.endsWith(QDir::separator()))
            _strInstallPath = _strInstallPath + QDir::separator();

        QMessageBox::warning(
                    this,
                    tr("Installation directory and/or language changed"),
                    tr("You must restart EE Keeper for the new installation directory and/or language to take effect.")
        );
    }

    accept();
}

void InstallationDirectory::on_buttonCancel_clicked()
{
    // we do nothing!
    reject();
}

void InstallationDirectory::on_buttonBrowse_clicked()
{
    // open up a browse window
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Baldur's Gate Enhanced Edition Installation Directory"));

    if (!dirName.isEmpty() ) {
        QDir dir(dirName); // open the directory

        // determining the directory of installation varies based on the OS.
        // WIN: they select the folder and it must contain the Baldur.exe
        // MAC: they select the folder containing 'Baldur's Gate - Enhanced Edition.app'
        // LINUX: TBD (if released)
#if defined(Q_OS_WIN)
        if (!dir.exists("Baldur.exe"))
#elif defined(Q_OS_MAC)
        if (!dir.cd("Baldur's Gate - Enhanced Edition.app/Contents/Resources"))
#else
        if (0) // unsupported LINUX
#endif
        {
            QMessageBox::critical(
                        this,
                        tr("Error opening BG:EE installation directory"),
                        tr("The directory selected doesn't appear to contain a BG:EE installation.")
            );

            return;
        }

        // update the directory display
        ui->lineDirectory->setText(dir.absolutePath());
    }
}

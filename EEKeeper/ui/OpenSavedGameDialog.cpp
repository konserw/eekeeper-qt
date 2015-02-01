#include "OpenSavedGameDialog.h"
#include "ui_OpenSavedGameDialog.h"


#include "EEKeeper.h"

#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

const QString OpenSavedGameDialog::strQuickSave = QString("Quick-Save");
const QString OpenSavedGameDialog::strAutoSave = QString("Auto-Save");

OpenSavedGameDialog::OpenSavedGameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenSavedGameDialog),
    savedir(NULL),
    m_saveDirectory(""),
    m_saveName("")
{
    ui->setupUi(this);

    // grab our saved information
    if (!_strDocumentsPath.isEmpty() ) {
        QDir dir(_strDocumentsPath);

        if (!dir.exists("save") ) {
            QMessageBox::warning(
                        this,
                        tr("Error loading documents path"),
                        tr("The current documents path does not have any saved games. Please set a new documents path for BG:EE.")
            );
        } else {
            savedir = new QDir(dir);
            updateTableWidget(QString("save"));
            ui->textDir->setText(_strDocumentsPath);
        }
    }

    // hide the data column
    ui->tableWidget->hideColumn(2);

    // reset our UI
    reset();
}

OpenSavedGameDialog::~OpenSavedGameDialog()
{
    delete ui;

    // close our directory
    delete savedir;
}

void OpenSavedGameDialog::on_buttonOK_clicked()
{
    accept();
}

void OpenSavedGameDialog::on_buttonCancel_clicked()
{
    reject();
}

void OpenSavedGameDialog::on_toolOpenSavedGame_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Saved Game Directory"));

    if (!dirName.isEmpty()) {
        QDir dir(dirName);

        // ensure that the save directory exists within this sub-directory
        if (!dir.exists("save")) {
            QMessageBox::critical(this, tr("Error"), tr("Save directory not found!"));
            return;
        }

        // delete our old pointer
        delete savedir;
        savedir = new QDir(dir);

        // reset the multiplayer and black pits options
        reset();

        // update our text field
        ui->textDir->setText(dirName);

        // update the table widget
        updateTableWidget(QString("save"));

        // save this directory in our global
        _strDocumentsPath = savedir->absolutePath();

        if (!_strDocumentsPath.endsWith(QDir::separator()))
            _strDocumentsPath = _strDocumentsPath + QDir::separator();
    }
}

void OpenSavedGameDialog::on_checkMultiplayer_stateChanged(int checkedState)
{
    if (checkedState == Qt::Checked) {
        ui->checkBlackPits->setChecked(false);
        updateTableWidget(QString("mpsave"));
    } else {
        // load single-player games
        updateTableWidget(QString("save"));
    }
}

void OpenSavedGameDialog::on_checkBlackPits_stateChanged(int checkedState)
{
    if (checkedState == Qt::Checked) {
        ui->checkMultiplayer->setChecked(false);
        updateTableWidget(QString("bpsave"));
    } else {
        // load single-player games
        updateTableWidget(QString("save"));
    }
}

void OpenSavedGameDialog::reset() {
    // uncheck both MP and BP
    ui->checkMultiplayer->setChecked(false);
    ui->checkBlackPits->setChecked(false);

    // enable our buttons, if we have good folders
    if (!savedir) {
        ui->checkMultiplayer->setEnabled(false);
        ui->checkBlackPits->setEnabled(false);
        ui->tableWidget->setEnabled(false);
    } else {
        ui->checkMultiplayer->setEnabled(savedir->exists("mpsave"));
        ui->checkBlackPits->setEnabled(savedir->exists("bpsave"));
        ui->tableWidget->setEnabled(savedir->exists("save"));
    }
}

void OpenSavedGameDialog::updateTableWidget(const QString &subdir) {
    if (!savedir) {
        // cannot do anything
        return;
    }

    // go to the given subdirectory
    if (!savedir->cd(subdir)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to load the save-files!"));
        return;
    }

    // clear our table and current save game
    ui->tableWidget->clearContents();
    m_saveDirectory = QString("");
    m_saveName = QString("");

    // get all the files in the subdirectory and cd back out
    QFileInfoList list = savedir->entryInfoList(QDir::AllDirs, QDir::Time);
    int row = 0;
    for (QFileInfoList::iterator it = list.begin(); it != list.end(); ++it) {
        // grab everything after the first dash
        QFileInfo file = (*it);
        QString name = file.baseName().section("-", 1);

        // we don't care about empty folders or the quick/auto-save folders
        if (name.isEmpty() || name == strQuickSave || name == strAutoSave) {
            continue;
        }

        // add the option to the table
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(name));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(file.lastModified().toString()));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(file.canonicalFilePath()));

        // go to the next row
        ++row;
    }

    // show the table if we have any rows
    ui->tableWidget->setEnabled((row > 0));

    // jump back up
    savedir->cdUp();
}

void OpenSavedGameDialog::on_tableWidget_clicked(const QModelIndex &index)
{
    m_saveDirectory = ui->tableWidget->item(index.row(), 2)->data(QTableWidgetItem::Type).toString() + QDir::separator();
    m_saveName = ui->tableWidget->item(index.row(), 0)->data(QTableWidgetItem::Type).toString();

    QImage img;
    QString tmp(m_saveDirectory);
    //Normally tmp would be null, due to geting the image info from the in-game files, but in this case, we need to set it to the game dir
    //we can then add a new variable for the BMP name, load that into the QLabel on the Open dialog so that people can be familiar with the game they just saved.
    QString tmpBMP("BALDUR.BMP");
    if (EEKeeper::GetResourceBitmap(tmp,img,0,0,tmpBMP) ) {
        ui->labelGameBMP->setPixmap(QPixmap::fromImage(img));
    } else {
        QPixmap empty;
        ui->labelGameBMP->setPixmap(empty);
    }
}

void OpenSavedGameDialog::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    // update and "click" ok
    on_tableWidget_clicked(index);
    on_buttonOK_clicked();
}

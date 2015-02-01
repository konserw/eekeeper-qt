#include "SavedGameWidget.h"
#include "ui_SavedGameWidget.h"

#include "EEKeeper.h"
#include "InfGame.h"
#include "StringFinderDialog.h"

SavedGameWidget::SavedGameWidget(QWidget *parent, QString &directory) :
    QWidget(parent),
    ui(new Ui::SavedGameWidget),
    m_dir(directory),
    m_game(),
    m_current(NULL),
    m_open(true)
{
    ui->setupUi(this);

    // load the game
    if (!m_game.Read(m_dir + "BALDUR.gam")) {
        qWarning() << "    Failed to open" << m_dir + "BALDUR.gam";
        m_open = false;
        ui->scrollBarCharacter->setMaximum(0);
    } else {
        // create our sheets
        CharacterSheetWidget *sheet = NULL;
        for (int i=0; i<m_game.GetPartyCount(); i++) {
            sheet = new CharacterSheetWidget(ui->widgetSheet, m_game.GetPartyCre(i));
            ui->widgetSheet->addWidget(sheet);
        }

        // adjust the size of the scroll bar
        ui->scrollBarCharacter->setMaximum(m_game.GetPartyCount() - 1);

        // setup the first character
        on_scrollBarCharacter_valueChanged(0);
    }
}

SavedGameWidget::~SavedGameWidget()
{
    // delete our character sheets
    if (m_open) {
        CharacterSheetWidget *sheet = NULL;
        for (int i=0; i<m_game.GetPartyCount(); i++) {
            sheet = (CharacterSheetWidget *)ui->widgetSheet->widget(i);
            delete sheet;
            sheet = NULL;
        }
    }

    delete ui;
}

void SavedGameWidget::on_scrollBarCharacter_valueChanged(int value)
{
    if (!m_open)
        return;

    // update with the requested character
    m_current = m_game.GetPartyCre(value);

    // GET BITMAP
    QImage img;
    QString tmp("");
    m_current->GetResLargePortrait(tmp);

    if (EEKeeper::GetResourceBitmap(tmp, img) ) {
        ui->labelPortrait->setPixmap(QPixmap::fromImage(img));
    } else {
        QPixmap empty;
        ui->labelPortrait->setPixmap(empty);
    }

    // NAME
    m_current->GetName(tmp);
    ui->lineName->setText(tmp);
    ui->lineName->setEnabled(!m_current->IsNPC());

    // get the current index
    int index = ((CharacterSheetWidget *)ui->widgetSheet->currentWidget())->GetTabIndex();

    // update the index
    ui->widgetSheet->setCurrentIndex(value);

    // update the tab index
    if (index >= 0)
        ((CharacterSheetWidget *)ui->widgetSheet->currentWidget())->SetTabIndex(index);

    // show the widget
    ui->widgetSheet->show();
}

bool SavedGameWidget::HasChanged()
{
    if (!m_open)
        return false;

    Update();
    return m_game.HasChanged();
}

void SavedGameWidget::Update()
{
    if (!m_open)
        return;

    int nCount = ui->widgetSheet->count();
    for (int i=0; i<nCount; i++) {
        ((CharacterSheetWidget*)(ui->widgetSheet->widget(i) ) )->Update();
    }
}

bool SavedGameWidget::Write(const QString &strName, bool bOverwrite)
{
    qDebug() << tr("   Saving game: %1").arg(strName);
    if (!m_open)
        return false;

    // update and save
    Update();

    QDir originalDir(m_dir);
    if (strName.isEmpty() || strName.compare(originalDir.dirName().section('-', 1), Qt::CaseInsensitive) == 0)
        return m_game.Write(m_dir + "BALDUR.gam");

    // go up a directory
    QDir dir(m_dir);
    if (!dir.cdUp()) {
        qDebug() << tr("    Failed to go up a directory.");
        return false;
    }

    // figure out the highest number save
    QFileInfoList saveDirs = dir.entryInfoList(QDir::AllDirs, QDir::Name);
    if (saveDirs.isEmpty()) {
        qDebug() << tr("    No saved games found.");
        return false;
    }

    // see if this dirname already exists
    QFileInfo   info;
    QString     curSaveName;
    QString     strDirname;
    bool        bHaveDir = false;
    for (QFileInfoList::iterator it = saveDirs.begin(); it != saveDirs.end(); ++it) {
        info = *it;
        curSaveName = info.baseName().section('-', 1);
        if (curSaveName.compare(strName, Qt::CaseInsensitive) == 0) {
            // see if the user wants to overwrite
            if (!bOverwrite) {
                qDebug() << "   Save game name already exists. Please select to overwrite to save at this location.";
                return false;
            }

            // we have a dir
            dir = info.absoluteDir();
            strDirname = info.baseName();
            bHaveDir = true;
            break;
        }
    }

    if (!bHaveDir) {
        bool bOk;
        uint dwNumber = saveDirs.last().baseName().section('-', 0, 0).toUInt(&bOk);

        if (!bOk) {
            qDebug() << tr("    Unable to determine last saved game number: %1").arg(saveDirs.last().baseName().section('-', 0, 0));
            return false;
        }

        // ensure no auto/quick save hijinks
        ++dwNumber;
        if (dwNumber <= 1)
            dwNumber = 2;

        // attempt to create the new directory
        strDirname = QString("%1-%2")
                .arg(dwNumber, 9, 10, QChar('0'))
                .arg(strName);

        if (!dir.mkdir(strDirname)) {
            qDebug() << tr("    Failed to create directory: %1").arg(strDirname);
            return false;
        }
    }

    // move into the new directory
    if (!dir.cd(strDirname)) {
        qDebug() << tr("    Failed to move into directory: %1").arg(strDirname);
        return false;
    }

    // delete everything in the current directory
    QFileInfoList files = dir.entryInfoList(QDir::Files);
    for (QFileInfoList::iterator it = files.begin(); it != files.end(); ++it) {
        info = *it;
        if (!dir.remove(info.fileName())) {
            qDebug() << tr("    Failed to clear the new save directory.");
            return false;
        }
    }

    // copy in all the old stuff
    QString     strNewFile;
    files = originalDir.entryInfoList(QDir::Files);
    for (QFileInfoList::iterator it = files.begin(); it != files.end(); ++it) {
        info = *it;
        strNewFile = dir.absolutePath().append(QDir::separator()).append(info.fileName());
        if (!QFile::copy(info.absoluteFilePath(), strNewFile)) {
            qDebug() << tr("   Failed to copy %1 to %2").arg(info.absoluteFilePath()).arg(strNewFile);
            return false;
        }
    }

    // finally, update our m_dir and save
    m_dir = dir.absolutePath();
    if (!m_dir.endsWith(QDir::separator()))
        m_dir += QDir::separator();

    return m_game.Write(m_dir + "BALDUR.gam");
}

void SavedGameWidget::on_toolStrRef_clicked()
{
    StringFinderDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
    }
}

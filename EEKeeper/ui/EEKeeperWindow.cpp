#include "EEKeeperWindow.h"
#include "ui_EEKeeperWindow.h"
#include "OpenSavedGameDialog.h"
#include "InstallationDirectory.h"
#include "SaveGameNameDialog.h"
#include "SavedGameWidget.h"
#include "ValueListDialog.h"

#include "EEKeeper.h"
#include "InfKey.h"
#include "InfTlk.h"
#include "SpellBitmaps.h"

#include <QMessageBox>
#include <QProgressDialog>
#include <QSettings>

EEKeeperWindow::EEKeeperWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EEKeeperWindow),
    m_games()
{
    ui->setupUi(this);
    readSettings();

    // default to en_US for language
    if (_strLanguage.isEmpty())
        _strLanguage = "en_US";

    // attempt to find the install path, if not set
    if (_strInstallPath.isEmpty() )
        EEKeeper::FindInstallPath(_strInstallPath);
}

EEKeeperWindow::~EEKeeperWindow()
{
    // clear our saved game tabs and delete all the
    // open saved games
    ui->tabWidget->clear();
    qDeleteAll(m_games);

    delete ui;
}

void EEKeeperWindow::show() {
    QMainWindow::show();

    // setup our checks
    ui->actionToolbar->setChecked(ui->mainToolBar->isVisible());
    ui->actionStatus_Bar->setChecked(ui->statusBar->isVisible());

    // load our resources
    this->loadResources();
}

void EEKeeperWindow::closeEvent(QCloseEvent *event)
{
    // save our current settings
    QSettings settings("EEKeeper");

    settings.beginGroup("EEKeeperWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.endGroup();

    //settings.beginGroup("menuBar");
    //settings.endGroup();

    // Editor settings
    settings.beginGroup("editorSettings");
    settings.setValue("strInstallPath", _strInstallPath);
    settings.setValue("strDocumentsPath", _strDocumentsPath);
    settings.setValue("strLanguage", _strLanguage);
    settings.setValue("bUseKnownSpellLimit", _bUseKnownSpellLimit);
    settings.setValue("bUseMemSpellLimit", _bUseMemSpellLimit);
    settings.setValue("bMemSpellsOnSave", _bMemSpellsOnSave);
    settings.setValue("bDisableSpellIcons", _bDisableSpellIcons);
    settings.setValue("nKnownSpellLimit", _nKnownSpellLimit);
    settings.setValue("nMemSpellLimit", _nMemSpellLimit);
    settings.setValue("bDefaultOpenSinglePlayer", _bDefaultOpenSinglePlayer);
    settings.setValue("bUseGridLines", _bUseGridLines);
    settings.setValue("bIgnoreDataVersions", _bIgnoreDataVersions);
    settings.setValue("bAllowQuicksaves", _bAllowQuicksaves);
    settings.setValue("bAllowCHROverwrite", _bAllowCHROverwrite);
    settings.setValue("bAllowCREOverwrite", _bAllowCREOverwrite);
    settings.endGroup();

    // continue to close the window
    QMainWindow::closeEvent(event);
}

void EEKeeperWindow::readSettings()
{
    // grab our current settings
    QSettings settings("EEKeeper");

    settings.beginGroup("EEKeeperWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    settings.endGroup();

    //settings.beginGroup("menuBar");
    //settings.endGroup();

    settings.beginGroup("editorSettings");
    _strInstallPath = settings.value("strInstallPath").toString();
    _strDocumentsPath = settings.value("strDocumentsPath").toString();
    _strLanguage = settings.value("strLanguage").toString();
    _bUseKnownSpellLimit = settings.value("bUseKnownSpellLimit").toBool();
    _bUseMemSpellLimit = settings.value("bUseMemSpellLimit").toBool();
    _bMemSpellsOnSave = settings.value("bMemSpellsOnSave").toInt();
    _bDisableSpellIcons = settings.value("bDisableSpellIcons").toBool();
    _nKnownSpellLimit = settings.value("nKnownSpellLimit").toInt();
    _nMemSpellLimit = settings.value("nMemSpellLimit").toInt();
    _bDefaultOpenSinglePlayer = settings.value("bDefaultOpenSinglePlayer").toBool();
    _bUseGridLines = settings.value("bUseGridLines").toBool();
    _bIgnoreDataVersions = settings.value("bIgnoreDataVersions").toBool();
    _bAllowQuicksaves = settings.value("bAllowQuicksaves").toBool();
    _bAllowCHROverwrite = settings.value("bAllowCHROverwrite").toBool();
    _bAllowCREOverwrite = settings.value("bAllowCREOverwrite").toBool();
    settings.endGroup();

    // ensure that _strInstallPath ends with a QDir::seperator
    if (!_strInstallPath.isEmpty() && !_strInstallPath.endsWith(QDir::separator()))
        _strInstallPath = _strInstallPath + QDir::separator();

    if (!_strDocumentsPath.isEmpty() && !_strDocumentsPath.endsWith(QDir::separator()))
        _strDocumentsPath = _strDocumentsPath + QDir::separator();
}

void EEKeeperWindow::on_actionOpen_Saved_Game_triggered()
{
    OpenSavedGameDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString saveDirectory = dialog.getSaveDirectory();
        QString saveName = dialog.getSaveName();
        qDebug() << "Save game folder:" << saveDirectory;

        if (m_games.contains(saveName)) {
            QMessageBox::warning(
                        this,
                        tr("Saved game already opened"),
                        tr("The save game is already opened. Simply click the tab to change back to it.")
            );

            return;
        }

        SavedGameWidget *game = new SavedGameWidget(ui->tabWidget, saveDirectory);
        if (!game) {
            qCritical() << "    Failed to allocate memory for the game widget.";
        }

        if (!game->IsOpen()) {
            QMessageBox::critical(
                        this,
                        tr("Open saved game failed"),
                        tr("Failed to open the saved game: ") + saveDirectory + "BALDUR.gam"
            );

            delete game;
            return;
        }

        // add the game to our tab widget
        ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(game, saveName));
        m_games.insert(saveName, game);

        // show the widget and the game
        ui->tabWidget->show();
        game->show();

        // we can save now!
        ResetStatus(true);
    }
}

void EEKeeperWindow::on_actionToolbar_toggled(bool toggled)
{
    ui->mainToolBar->setVisible(toggled);
}

void EEKeeperWindow::on_actionStatus_Bar_toggled(bool toggled)
{
    ui->statusBar->setVisible(toggled);
}

void EEKeeperWindow::on_actionInstallation_Directory_triggered()
{
    InstallationDirectory dialog(this);
    dialog.exec();
}

bool EEKeeperWindow::loadResources()
{
    // assume success
    _bResourcesLoaded = true;
    QString tlkDIR = _strInstallPath + "lang" + QDir::separator() + _strLanguage + QDir::separator() + "dialog.tlk";

    // load Dialog.tlk based on the given language
    if (!_infTlk.Open(tlkDIR)) {
        QMessageBox::critical(
                    this,
                    tr("Cannot load Dialog.tlk!"),
                    tr("EE Keeper was unable to load the text resources. EE Keeper must be able to read this file to work correctly. Make sure the Installation Directory is correct and restart EE Keeper.")
        );

        _bResourcesLoaded = false;
    }

    // we must have our resources correctly loaded to run anything
    ui->actionOpen_Saved_Game->setEnabled(_bResourcesLoaded);
    ui->actionOpen_Character_File->setEnabled(_bResourcesLoaded);
    ui->actionOpen_Creature_File->setEnabled(_bResourcesLoaded);
    ui->actionCreature_Browser->setEnabled(_bResourcesLoaded);
    ui->actionSpell_Browser->setEnabled(_bResourcesLoaded);
    ui->actionItem_Browser->setEnabled(_bResourcesLoaded);

    // load in all the big stuff!!
    if (_bResourcesLoaded) {
        QProgressDialog progress(
                    tr("Reading chitin.key..."),
                    "",
                    0,
                    100,
                    this,
                    Qt::Dialog
        );

        QString copysplash = "EEKeeper is copyright (c) 2013 by the EEKeeper team and is based\n"
                "on Shadow Keeper, copyright (C) 2000 Aaron O'Neil.\n\n"
                "Powered by the Qt GUI Toolkit, copyright (C) 2013 Digia Plc\n"
                "and/or subsidiary(-ies) and other contributors.\n\n";

        progress.setWindowModality(Qt::WindowModal);
        progress.setLabelText(copysplash + "Reading chitin.key...");
        progress.setCancelButton(NULL);
        progress.show();

        if (!_infKey.Read(_strInstallPath + QDir::separator() + "chitin.key", &progress)) {
            qDebug() << "Failed to read chitin.key:" << _strInstallPath + QDir::separator() + "chitin.key";
            return false;
        }

        // change up our progress information
        progress.setLabelText(copysplash + "Loading bitmaps...");

        if (!_spellBitmaps.LoadImages(_bDisableSpellIcons, &progress)) {
            qDebug() << "Failed to load bitmaps!";
            return false;
        }

        progress.hide();

        QString vlType = "ALIGN";
        if (!ProcessList(vlType, _vlAlignment))
            return false;

        if (!ProcessList((vlType = "ANIMATE"), _vlAnimations))
            return false;

        if (!ProcessList((vlType = "CLASS"), _vlClass, false, 0, 0, 50))
            return false;

        if (!ProcessList((vlType = "RACE"), _vlRace, false, 0, 0, 50))
            return false;

        if (!ProcessList((vlType = "GENDER"), _vlGender))
            return false;

        if (!ProcessList((vlType = "EA"), _vlEnemyAlly))
            return false;

        if (!ProcessList((vlType = "STATE"), _vlState))
            return false;

        if (!ProcessList((vlType = "SPELL"), _vlSpells))
            return false;

        //if (!ProcessList((vlType = "KIT"),_vlKit))
        //{
        //	return false;
        //}

        if (!Process2da((vlType = "HATERACE"), &_2daHateRace))
            return false;

        if (!Process2da((vlType = "WEAPPROF"), &_2daWeapProf))
            return false;

        if (!Process2da((vlType = "KITLIST"), &_2daKitList))
            return false;

        //if (!Process2da((vlType = "KITLIST"),&_2daKit))
        //{
        //	AddToLog("!! Error reading KITLIST.2DA\n\n");
        //	EndDialog(IDCANCEL);
        //	return false;
        //}

        qDebug() << "Creating racial enemy list.";
        if (!CreateRacialEnemyList())
            return false;
        qDebug() << "   List created." << endl;

        qDebug() << "Creating weapon proficiency list.";
        if (!CreateWeaponProfList())
            return false;
        qDebug() << "   List created." << endl;

        qDebug() << "Creating kit ability list.";
        if (!CreateKitList())
            return false;
        qDebug() << "   List created." << endl;

        //if (!CreateKits())
        //{
        //	AddToLog("   Failed creating list.\n\n");
        //	EndDialog(IDCANCEL);
        //	return false;
        //}
        //AddToLog("   List created.\n\n");*/

        // LOAD LISTS
        qDebug() << "Loading user-defined lists.";
        if (!_vlKit.Load(ULF_KITS)) {
            qDebug() << QString("   Failed to load Kits list: %1!").arg(ULF_KITS);
        }

        if (!_vlAffects.Load(ULF_AFFECTS)) {
            qDebug() << QString("   Failed to load Affects list: %1!").arg(ULF_AFFECTS);
        }
        qDebug() << "   Lists loaded.";
    }

    return _bResourcesLoaded;
}

bool EEKeeperWindow::ProcessList(QString &pszResource, CValueList &vl, bool bUseRange, quint32 nMin, quint32 nMax, int nBracketThreshold)
{
    qDebug() << "Reading resource:" << (pszResource + ".IDS");

    CResInfo *pResInfo = _infKey.GetResInfo(RESTYPE_IDS, pszResource);
    if (!pResInfo)
    {
        qDebug() << "   Unable to locate resource.";
        return(false);
    }

    CInfBifFile *pFile = _infKey.GetBifFile(pResInfo);
    if (!pFile || !pFile->IsValid())
    {
        QString strFile;
        _infKey.GetBifFile(pResInfo, strFile);
        qDebug() << "   Unable to locate bif file.";
        qDebug() << "      Filename:" << strFile;
        qDebug() << "      Index:" << pResInfo->wBifKeyIndex;
        qDebug() << "      Is Valid:" << (pFile ? pFile->IsValid() : 0);

        return(false);
    }

    int nSize, nOffset;
    if (!pFile->GetDataOffsetAndSize(pResInfo, nOffset, nSize))
    {
        qDebug() << "   Unable to locate resource data.";
        return(false);
    }

    char *pszBuf = new char[nSize+1];
    if (!pszBuf)
        return(false);
    pszBuf[nSize] = '\x0';

    pFile->Seek(nOffset);
    pFile->Read(pszBuf, nSize);

    // If the first two bytes of the buffer are 0xFF then the text is encrypted
    // with an XOR key.
    char *pszUsePtr;
    int nUseSize;
    if ((quint8)(pszBuf[0]) == 0xFF && (quint8)(pszBuf[1]) == 0xFF)
    {
        nUseSize = nSize-2;
        pszUsePtr = pszBuf+2;

        for (int i=0;i<nUseSize;i++)
            pszUsePtr[i] ^= _xorKey[i % XOR_KEY_LEN];
    }
    else
    {
        nUseSize = nSize;
        pszUsePtr = pszBuf;
    }

    int nBufIndex;
    QString strLine;
    TValueItem vi;

    // Eat up that first line. I'm not sure if this is supposed to be the count of
    // lines or not. It was close, but not quite right for the one type I looked at.
    nBufIndex = 0;
    GetLine(pszUsePtr, nBufIndex, strLine);
    while(nBufIndex < nUseSize)
    {
        GetLine(pszUsePtr, nBufIndex, strLine);
        SplitLine(strLine, vi.dwValue, vi.strText);

        if(vi.strText == "Mask Good" || vi.strText == "Mask Chaotic" || vi.strText == "Mask Geneutral" || vi.strText == "Mask Evil" || vi.strText == "Mask Lawful" || vi.strText == "Mask Lcneutral")
            continue;

        if (pResInfo->strName == "KIT")
        {
            qDebug() << "Reading from kits.";
            //vi.dwValue += 0x3FFFC000;//1073725440;
        }


        // If we are using a limiting range make, check it. Rather than assiging
        // the values to different variables and checking before adding to the
        // list I'm just adding them directly to the list. If they don't qualify
        // to be added I'm not incrementing the index count so it'll be overwritten
        // next time through.
        if (!bUseRange || (vi.dwValue >= nMin && vi.dwValue <= nMax))
        {
            if (nBracketThreshold >= 0 && vi.dwValue > (quint32)nBracketThreshold)
                vi.strText = QString("[ ") + vi.strText + " ]";

            vl.append(vi);
        }
    }

    delete [] pszBuf;

    qDebug() << "   Done reading resource." << endl;
    return(true);
}

bool EEKeeperWindow::GetLine(const char *pszBuf, int &nStart, QString &strLine)
{
    strLine.clear();

    while(pszBuf[nStart] && pszBuf[nStart] != '\r' && pszBuf[nStart] != '\n')
    {
        strLine += pszBuf[nStart];
        nStart++;
    }

    // Read the newline that should follow a CR.
    if (pszBuf[nStart] && (pszBuf[nStart] == '\r' || pszBuf[nStart] == '\n'))
    {
        nStart++;		// Skip the \r.
        if (pszBuf[nStart] && (pszBuf[nStart] == '\n' || pszBuf[nStart] == '\r'))
            nStart++;	// Skip the \n.
    }

    return(true);
}

bool EEKeeperWindow::SplitLine(QString &strLine, quint32 &nValue, QString &strText)
{
    strLine = strLine.trimmed();

    int nIndex = strLine.indexOf(QChar(' '));
    if (nIndex == -1)
        return(false);

    nValue = strLine.left(nIndex).toUInt(NULL, (strLine.left(2) == "0x" ? 16 : 10));
    strText = strLine.mid(nIndex+1);

    // Get rid of any underscores, replace with spaces. Also check for all caps.
    int nLen = strText.length();
    bool bAllUpper = strText.compare(strText.toUpper(), Qt::CaseSensitive) == 0;

    QChar ch;
    nIndex = 0;

    while(nIndex < nLen)
    {
        ch = strText.at(nIndex);

        if (ch == QChar('_'))
            strText.replace(nIndex, 1, QChar(' '));

        nIndex++;
    }

    if (bAllUpper)
    {
        nIndex = 0;

        while(nIndex < nLen)
        {
            ch = strText.at(nIndex);
            if (nIndex > 0 && ch.isUpper() && strText.at(nIndex-1).isLetterOrNumber() )
                strText.replace(nIndex, 1, ch.toLower());

            nIndex++;
        }
    }

    strText = strText.trimmed();
    return(true);
}

bool EEKeeperWindow::Process2da(QString &strResource, CInf2DA *p2DA)
{
    qDebug() << "Reading" << strResource + ".2da";

    quint8 *pBuf;
    int nLen;

    if (!_infKey.GetResData(RESTYPE_2DA, strResource, pBuf, nLen, true))
    {
        qDebug() << "   Unable to read resource.";
        return(false);
    }

    if (!p2DA->Parse(pBuf))
    {
        qDebug() << "   Unable to parse resource.";
        delete [] pBuf;
        return(false);
    }

    delete [] pBuf;
    qDebug() << "   Done reading resource." << endl;

    return(true);
}

bool EEKeeperWindow::CreateRacialEnemyList()
{
    int nRows = _2daHateRace.GetRows();
    TValueItem vi;
    int nValue;

    for (int i=0; i<nRows ; i++)
    {
        _2daHateRace.GetData(i, 1, nValue);
        _infTlk.GetString(nValue, vi.strText);
        _2daHateRace.GetData(i, 2, vi.dwValue);

        _vlRacialEnemy.append(vi);
    }

    return(true);
}

bool EEKeeperWindow::CreateWeaponProfList()
{
    int nRows = _2daWeapProf.GetRows();
    TValueItem vi;
    quint32 nStrRef, nValue;

    for (int i=0; i<nRows; i++)
    {
        _2daWeapProf.GetData(i, 2, nStrRef);
        _2daWeapProf.GetData(i, 1, nValue);

        // There are a bunch of entries labled "Extra" that I want to avoid
        // putting into the list. They all have a max value for their text string.
        //
        // 89 is the start of the new proficiencies. It looks like the older ones
        // from BG1 are still in it as well and I want to skip those.
        if (nValue < 89 || nStrRef == 0xFFFFFFFF || nStrRef == 0)
            continue;

        _infTlk.GetString(nStrRef, vi.strText);
        if (vi.strText.isEmpty())
            continue;

        vi.dwValue = nValue;
        _vlProfs.append(vi);
    }

    return(true);
}

bool EEKeeperWindow::CreateKitList()
{
    int nRows = _2daKitList.GetRows();
    TValueItem vi;
    QString strAbilityRef;
    int nStrRef;

    for (int i=0; i<nRows; i++)
    {
        // The table appears to have bad data in it. Not all of the kits have a
        // valid link to another 2da file -- they just seem to be missing. I'm
        // making sure to only put kits in the list that have a valid link.
        _2daKitList.GetData(i, 5, strAbilityRef);
        if (!_infKey.GetResInfo(RESTYPE_2DA, strAbilityRef)) {
            continue;
        }

        _2daKitList.GetData(i, 3, nStrRef);

        if (nStrRef == 25359)
            nStrRef = 24229;


        _infTlk.GetString(nStrRef, vi.strText);
        _2daKitList.GetData(i, 0, vi.dwValue);
        _vlKitList.append(vi);
    }

    return(true);
}

void EEKeeperWindow::DeleteTab(int index)
{
    // grab the requested game
    SavedGameWidget *gameWidget = (SavedGameWidget*)(ui->tabWidget->widget(index));
    if (!gameWidget)
        return;

    // find the widget in our internal list
    QString key = m_games.key(gameWidget);
    if (key.isEmpty()) {
        qDebug() << "Unable to find the gameWidget in the list";
        return;
    }

    // ensure we remove the widget from our internal list
    if (m_games.remove(key) == 0) {
        qDebug() << "Unable to remove the gameWidget from the list";
        return;
    }

    // finally remove the tab from the widget and delete it
    ui->tabWidget->removeTab(index);
    delete gameWidget;

    // go to the next available tab
    index -= 1;
    index = index < 0 ? 0 : index;
    ui->tabWidget->setCurrentIndex(index);
    ui->tabWidget->setVisible(ui->tabWidget->count() != 0);

    ResetStatus(ui->tabWidget->count() != 0);

    return;
}

void EEKeeperWindow::ResetStatus(bool bEnabled)
{
    ui->actionSave_Game->setEnabled(bEnabled);
    ui->actionAffects->setEnabled(!bEnabled);
    ui->actionKits->setEnabled(!bEnabled);
}

void EEKeeperWindow::SaveGameDialog(SavedGameWidget *gameWidget)
{
    if (gameWidget == NULL)
        return;

    SaveGameNameDialog dialog(this, gameWidget->GameName());
    if (dialog.exec() == QDialog::Accepted && !gameWidget->Write(dialog.SaveGameName(), dialog.CanOverwrite())) {
        QMessageBox::critical(
                    this,
                    tr("Save failed"),
                    tr("A critical error has occurred, and the game failed to save correctly.\n"
                       "If you are overwriting a game, please ensure 'Overwrite' is checked.")
        );
    }
}

void EEKeeperWindow::on_tabWidget_tabCloseRequested(int index)
{
    // grab the requested game
    SavedGameWidget *gameWidget = (SavedGameWidget*)(ui->tabWidget->widget(index));
    if (!gameWidget)
        return;

    // if it's not open, just delete and close the tab
    if (!gameWidget->IsOpen()) {
        DeleteTab(index);
    }

    // see if we've changed
    if (gameWidget->HasChanged()) {
        int nResponse = QMessageBox::question(
                    this,
                    tr("Game has changed"),
                    tr("Your game has been altered. Would you like to save the changes?"),
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                    QMessageBox::Cancel
        );

        // do nothing if they wish to cancel the close
        if (nResponse == QMessageBox::Cancel) {
            return;
        } else if (nResponse == QMessageBox::Yes) {
            SaveGameDialog(gameWidget);
        }
    }

    // remove our game widget from the tabs and delete it
    DeleteTab(index);
}

void EEKeeperWindow::on_actionSave_Game_triggered()
{
    // shouldn't be enabled, but just in case
    if (ui->tabWidget->count() == 0)
        return;

    // pop up our saving dialog
    SavedGameWidget *gameWidget = (SavedGameWidget *)(ui->tabWidget->currentWidget());
    SaveGameDialog(gameWidget);

    // make sure we rename the tab
    ui->tabWidget->setTabText(
                ui->tabWidget->currentIndex(),
                gameWidget->GameName()
    );
}

void EEKeeperWindow::on_actionSpell_Browser_triggered()
{
    ui->dockWidgetSpellBrowser->setVisible(!ui->dockWidgetSpellBrowser->isVisible());
    //ui->dockWidgetSpellBrowser->setFloating(false);
}

void EEKeeperWindow::on_actionKits_triggered()
{
    // open the kits window
    ValueListDialog dialog(_vlKit, 0, 0xFFFFFFFF, tr("Kits"), this);
    if (dialog.exec() == QDialog::Accepted) {
        // update and save
        dialog.FillValueList(_vlKit);
        _vlKit.Save(ULF_KITS);
    }
}

void EEKeeperWindow::on_actionAffects_triggered()
{
    // open the affects window
    ValueListDialog dialog(_vlAffects, 0, 0xFFFF, tr("Affects"), this);
    if (dialog.exec() == QDialog::Accepted) {
        // update and save
        dialog.FillValueList(_vlAffects);
        _vlAffects.Save(ULF_AFFECTS);
    }
}

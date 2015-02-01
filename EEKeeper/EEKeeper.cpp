#include "EEKeeper.h"
#include "InfBam.h"
#include "InfBifFile.h"
#include "InfKey.h"
#include "InfTlk.h"
#include "InfCreature.h"
#include "Inf2DA.h"
#include "PalImageList.h"
#include "SpellBitmaps.h"
#include "ValueList.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QRgb>

// Holds the indexes for the resources.
CInfKey _infKey;

// All the string resources are in a .tlk file. Might as well just keep it open
// and available. Item descriptions, for example, are stored in here.
CInfTlk	_infTlk;

CSpellBitmaps _spellBitmaps;

// An image list which contains the samples used for setting character
// colors.
CPalImageList _ilPal;

// Path to the installation.
QString _strInstallPath;

// Language
QString _strLanguage;

// Path to the Documents folder
QString _strDocumentsPath;

// This is set to true if the program has indexed the resources. This needs to
// happen for the editor to work properly. Most of the menu items set their state
// by this flag.
bool _bResourcesLoaded = false;

// When set to true log file entries are generated. This can be set by putting
// LOG on the command line to start the editor.
bool _bLogFile;

// Used to register a clipboard format for holding the CRE's AFF data.
quint32 _nClipFormatAff = 0;

// 2DAs
CInf2DA _2daHateRace;
CInf2DA _2daWeapProf;
CInf2DA _2daKitList;
CInf2DA _2daKit;

// Editor settings
bool _bUseKnownSpellLimit;
bool _bUseMemSpellLimit;
bool _bMemSpellsOnSave;		// Causes memorized spells to be refreshed as castable.
bool _bDisableSpellIcons;

int	_nKnownSpellLimit;
int	_nMemSpellLimit;

bool _bDefaultOpenSinglePlayer;
bool _bUseGridLines;
bool _bIgnoreDataVersions;
bool _bAllowQuicksaves;

bool _bAllowCHROverwrite;
bool _bAllowCREOverwrite;

// all of our value lists
CValueList _vlLanguages;
CValueList _vlAnimations;
CValueList _vlGender;
CValueList _vlRacialEnemy;
CValueList _vlKit;
CValueList _vlNumAttacks;
CValueList _vlProfs;
CValueList _vlClass;
CValueList _vlAlignment;
CValueList _vlRace;
CValueList _vlAffects;
CValueList _vlKitList;
CValueList _vlEnemyAlly;
CValueList _vlState;
CValueList _vlSpells;

KEY_RES_OVERRIDE _keyResOver[] =
{
    { RESTYPE_BMP, "bmp" },
    { RESTYPE_BAM, "bam" },
    { RESTYPE_ITM, "itm" },
    { RESTYPE_SPL, "spl" },
    { RESTYPE_IDS, "ids" },
    { RESTYPE_CRE, "cre" },
    { RESTYPE_2DA, "2da" },
    { RESTYPE_BCS, "bcs" },
    { 0, ""}
};

ITEMTYPE _itemType[] =
{
    { EEKeeper::tr("Misc"),           ITEMTYPE_MISC },
    { EEKeeper::tr("Necklace"),       ITEMTYPE_NECKLACE },
    { EEKeeper::tr("Armor"),          ITEMTYPE_ARMOR },
    { EEKeeper::tr("Belt"),           ITEMTYPE_BELT },
    { EEKeeper::tr("Boots"),          ITEMTYPE_BOOTS },
    { EEKeeper::tr("Arrows"),         ITEMTYPE_ARROWS },
    { EEKeeper::tr("Gloves"),         ITEMTYPE_GLOVES },
    { EEKeeper::tr("Helm"),           ITEMTYPE_HELM },
    { EEKeeper::tr("Key"),            ITEMTYPE_KEY },
    { EEKeeper::tr("Potion"),         ITEMTYPE_POTION },
    { EEKeeper::tr("Ring"),           ITEMTYPE_RING },
    { EEKeeper::tr("Scroll"),         ITEMTYPE_SCROLL },
    { EEKeeper::tr("Shield"),         ITEMTYPE_SHIELD },
    { EEKeeper::tr("Food"),           ITEMTYPE_FOOD },
    { EEKeeper::tr("Bullets"),        ITEMTYPE_BULLETS },
    { EEKeeper::tr("Bow"),            ITEMTYPE_BOW },
    { EEKeeper::tr("Dagger"),         ITEMTYPE_DAGGER },
    { EEKeeper::tr("Mace"),           ITEMTYPE_MACE },
    { EEKeeper::tr("Sling"),          ITEMTYPE_SLING },
    { EEKeeper::tr("Short Sword"),    ITEMTYPE_SHORTSWORD },
    { EEKeeper::tr("Sword"),          ITEMTYPE_SWORD },
    { EEKeeper::tr("Hammer"),         ITEMTYPE_HAMMER },
    { EEKeeper::tr("Morning Star"),   ITEMTYPE_MORNINGSTAR },
    { EEKeeper::tr("Flail"),          ITEMTYPE_FLAIL },
    { EEKeeper::tr("Darts"),          ITEMTYPE_DART },
    { EEKeeper::tr("Axe"),            ITEMTYPE_AXE },
    { EEKeeper::tr("Staff"),          ITEMTYPE_STAFF },
    { EEKeeper::tr("Crossbow"),       ITEMTYPE_CROSSBOW },
    { EEKeeper::tr("Hand to Hand"),   ITEMTYPE_HANDTOHAND },
    { EEKeeper::tr("Spear"),          ITEMTYPE_SPEAR },
    { EEKeeper::tr("Halberd"),        ITEMTYPE_HALBERD },
    { EEKeeper::tr("Bolts"),          ITEMTYPE_BOLTS },
    { EEKeeper::tr("Cloak"),          ITEMTYPE_CLOAK },
    { EEKeeper::tr("Gold"),           ITEMTYPE_GOLD },
    { EEKeeper::tr("Gem"),            ITEMTYPE_GEM },
    { EEKeeper::tr("Wand"),           ITEMTYPE_WAND },
    { EEKeeper::tr("Container"),      ITEMTYPE_CONTAINER },
    { EEKeeper::tr("Book"),           ITEMTYPE_BOOK },
    { EEKeeper::tr("Familiar"),       ITEMTYPE_FAMILIAR },
    { EEKeeper::tr("Misc2"),          ITEMTYPE_MISC2 },
    { EEKeeper::tr("Misc3"),          ITEMTYPE_MISC3 },
    { EEKeeper::tr("Drink"),          ITEMTYPE_DRINK },
    { EEKeeper::tr("Greatsword"),     ITEMTYPE_GREATSWORD },
    { EEKeeper::tr("Instrument"),     ITEMTYPE_INSTRUMENT },
    { EEKeeper::tr("Large Shield"),   ITEMTYPE_LARGESHIELD },
    { EEKeeper::tr("Medium Shield"),  ITEMTYPE_MEDIUMSHIELD },
    { EEKeeper::tr("Small Shield"),   ITEMTYPE_SMALLSHIELD },
    { EEKeeper::tr("Club"),           ITEMTYPE_CLUB },
    { EEKeeper::tr("Unknown"),        ITEMTYPE_UNKNOWN },	// This is a special case used by the item browser. It needs to stay the last item.
    { QString(""),                    ITEMTYPE_END }
};

// This needs to match the number of possible slots in a CRE.
ITEMPOS _itemPos[] =
{
    { EEKeeper::tr("Head") },
    { EEKeeper::tr("Body") },
    { EEKeeper::tr("Shield") },
    { EEKeeper::tr("Hands") },
    { EEKeeper::tr("Ring") },
    { EEKeeper::tr("Ring") },
    { EEKeeper::tr("Neck") },
    { EEKeeper::tr("Waist") },
    { EEKeeper::tr("Feet") },
    { EEKeeper::tr("Weapon 1") },
    { EEKeeper::tr("Weapon 2") },
    { EEKeeper::tr("Weapon 3") },
    { EEKeeper::tr("Weapon 4") },
    { EEKeeper::tr("Quiver 1") },
    { EEKeeper::tr("Quiver 2") },
    { EEKeeper::tr("Quiver 3") },
    { EEKeeper::tr("Unused") },
    { EEKeeper::tr("Back") },
    { EEKeeper::tr("Quick 1") },
    { EEKeeper::tr("Quick 2") },
    { EEKeeper::tr("Quick 3") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") },
    { EEKeeper::tr("Inventory") }
};

EEKeeper::EEKeeper() {
}

bool EEKeeper::ExtractResource(quint16 wResourceType, const QString &pszResource, const QString &pszPath)
{
    int nIndex = 0;
    while(_keyResOver[nIndex].wResType && _keyResOver[nIndex].wResType != wResourceType)
        nIndex++;

    if (!_keyResOver[nIndex].wResType)
        return(false);

    // get our filename and see if it already exists
    QString strFile = pszPath + pszResource + "." + _keyResOver[nIndex].szExt;
    if (QFile::exists(strFile))
    {
        // create our message box
        QString strMessage = "'" + strFile + "' " + tr("already exists. Do you want to overwrite the file?");
        int nResult = QMessageBox::warning(NULL, tr("Overwrite?"), strMessage, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        // see if the user doesn't want to continue
        if (nResult == QDialog::Rejected)
            return(false);
    }

    quint8 *pBuf;
    int  nDataSize;
    if (!_infKey.GetResData(wResourceType,pszResource,pBuf,nDataSize))
        return(false);

    QFile file(strFile);
    if (!file.open(QIODevice::WriteOnly))
    {
        delete[] pBuf;
        return(false);
    }

    file.write(reinterpret_cast<const char*>(pBuf),nDataSize);
    file.close();

    delete [] pBuf;
    return(true);
}

bool EEKeeper::SaveCreFile(CInfCreature *pCre, QString &strSavedAs, QWidget *parent)
{
    // ask the user which file to save as
    QString strFile = QFileDialog::getSaveFileName(
                    parent,
                    tr("Save CRE File"),
                    _strInstallPath,
                    tr("Creature Files (*.cre);;All Files (*.*)") );

    // see if the user cancelled
    if (strFile.isEmpty())
        return(false);

    // If the file exists, make a backup of it.
    if (QFile::exists(strFile))
    {
        QString strBackup = strFile.trimmed();
        if (strBackup.length() > 4)
        {
            // get the extention
            QString strExt = strBackup.right(4);
            strExt = strExt.toLower();

            // replace our .cre with .bak
            if (strExt == ".cre")
                strBackup = strBackup.left(strBackup.length()-4);
            strBackup += ".bak";
        }
        else
            strBackup += ".bak";

        if (!QFile::copy(strFile, strBackup) )
        {
            QMessageBox::critical(
                        parent,
                        tr("CRE Backup Failed"),
                        tr("Unable to create a backup of this creature file. Save canceled.") );
            return(false);
        }
    }

    QFile file(strFile);
    if (!file.open(QIODevice::WriteOnly) )
    {
        QString strMessage = tr("Unable to create the creature file: ") + strFile;
        QMessageBox::critical(
                    parent,
                    tr("Failed to Create CRE File"),
                    strMessage );
        return(false);
    }

    if (!pCre->Write(file))
    {
        QString strMessage = tr("Unable to save the creature file. Error ") + " ( " + pCre->GetLastError() + " ) ";
        QMessageBox::critical(
                    parent,
                    tr("Failed to Save CRE File"),
                    strMessage );
        file.close();
        return(false);
    }

    file.close();
    strSavedAs = strFile;
    _infKey.RefreshOverride();

    return(true);
}

bool EEKeeper::GetResourceBitmap(const QString &pszName, QImage &hBitmap, int nWidth, int nHeight, const QString &pszFileName)
{
    CResInfo *pBmpInfo = _infKey.GetResInfo(RESTYPE_BMP, pszName);
    if (!pBmpInfo)
    {
        // Didn't find it as a game resource. Check the Portraits directory
        // for a custom bitmap.

        // find the save game image or portrait:

        if (pszFileName == "BALDUR.BMP")
        {
            // TODO: check how it's determining this pszName
            QString strFile = pszName + pszFileName;
            if (!hBitmap.load(strFile) )
                return(false);

            if (nWidth || nHeight)
                return(ResizeBitmap(nWidth, nHeight, hBitmap));

            return(true);
        }
        else
        {
            QString strPath = _strDocumentsPath + DIR_PORTRAITS + QDir::separator() + pszName + ".bmp";

            if (!hBitmap.load(strPath))
                return(false);

            if (nWidth || nHeight)
                return(ResizeBitmap(nWidth, nHeight, hBitmap));

            return(true);
        }
    }

    // Found it in the game resource index.
    CInfBifFile *pBmpFile = _infKey.GetBifFile(pBmpInfo);
    if (!pBmpFile || !pBmpFile->IsValid())
        return(false);

    int nDataSize, nDataOffset;
    if (!pBmpFile->GetDataOffsetAndSize(pBmpInfo, nDataOffset, nDataSize))
        return(false);

    quint8 *pBmp = new quint8[nDataSize];
    if (pBmpFile->GetData(pBmpInfo, pBmp, nDataSize) == -1)
    {
        delete [] pBmpFile;
        return(false);
    }

    hBitmap = QImage::fromData(pBmp, nDataSize);

    delete [] pBmp;

    if (nWidth || nHeight)
        return(ResizeBitmap(nWidth, nHeight, hBitmap));

    return(true);
}

QString EEKeeper::GetSpellScriptName(const char *pszResName)
{
    QString strResult("");
    QString strResName(pszResName);

    if (strResName.length() < 4)
        return(strResult);

    strResName = strResName.toLower();

    // determine the type
    int nType;
    if (strResName.contains("wi") != -1) {
        nType = 2;
    } else if (strResName.contains("pr") != -1) {
        nType = 1;
    } else if (strResName.contains("in") != -1) {
        nType = 3;
    } else if (strResName.contains("cl") != -1) {
        nType = 4;
    } else {
        return(strResult);
    }

    char szBuf[100];
    qsnprintf(szBuf, 100, "%d%s", nType, pszResName+4);
    int nValue = atoi(szBuf);
    if (_vlSpells.GetText(nValue, strResult) )
        return(strResult);

    return(strResult);
}

bool EEKeeper::ResizeBitmap(int nWidth, int nHeight, QImage &image, QRgb clrTrans)
{
    // just force it if too large
    if (nWidth < image.width() || nHeight < image.height()) {
        image = image.scaled(nWidth, nHeight, Qt::KeepAspectRatio);
    } else {
        // make our image a blank slate, and copy the actual
        // image to it, centered
        QImage original(image);
        image = QImage(nWidth, nHeight, QImage::Format_ARGB32);
        image.fill(clrTrans);

        int nOriginalWidth = original.width();
        int xOffset = (nWidth - nOriginalWidth) / 2;
        int yOffset = (nHeight - original.height()) / 2;
        int nNumPixels = original.width() * original.height();
        int nPixelCount = 0;
        int x = 0;
        int y = 0;

        while (nPixelCount < nNumPixels) {
            image.setPixel(x + xOffset, y + yOffset, original.pixel(x, y));

            nPixelCount++;
            if (++x >= nOriginalWidth) {
                x = 0;
                ++y;
            }
        }
    }

    return(true);
}

bool EEKeeper::GetSpellBitmap(const CResInfo */*pInfo*/, INF_SPL &spl, const QRgb &clrTrans, QImage &hBitmap)
{
    CResInfo *pBamResInfo = _infKey.GetResInfo(RESTYPE_BAM, QString(spl.chSpellIcon));
    if (!pBamResInfo)
        return(false);

    CInfBifFile *pBamFile = _infKey.GetBifFile(pBamResInfo);
    if (!pBamFile || !pBamFile->IsValid())
        return(false);

    CInfBam bam;
    if (!pBamFile->GetBam(pBamResInfo, bam))
        return(false);

    return(bam.MakeBitmap(0, clrTrans, hBitmap, true, 32, 32));
}

bool EEKeeper::GetItemBitmap(const CResInfo */*pInfo*/, INF_ITM &itm, int nFrameWanted, QRgb clrTrans, QImage &hBitmap)
{
    CResInfo *pBamResInfo = _infKey.GetResInfo(RESTYPE_BAM, QString(itm.chItemIcon));
    if (!pBamResInfo)
        return(false);

    CInfBifFile *pBamFile = _infKey.GetBifFile(pBamResInfo);
    if (!pBamFile || !pBamFile->IsValid())
        return(false);

    CInfBam bam;
    if (!pBamFile->GetBam(pBamResInfo, bam))
        return(false);

    return(bam.MakeBitmap(nFrameWanted, clrTrans, hBitmap, true, 32, 32));
}


void EEKeeper::FixTlkDescription(const char *pszDesc, QString &strFixed)
{
    strFixed.clear();

    int nIndex = 0;
    int nLen = strlen(pszDesc);
    unsigned char ch;

    while(nIndex < nLen)
    {
        ch = pszDesc[nIndex];

        // Make sure every carriage return is followed by a linefeed.
        if (ch == '\r')
        {
            strFixed += ch;			// add the \r
            nIndex++;
            strFixed += '\n';		// add a \n no matter what

            if (nIndex < nLen && pszDesc[nIndex] == '\n') // if we happened to have a linefeed, skip it now
                nIndex++;
            continue;
        }

        if (ch == '\n')
        {
            strFixed += "\r\n";	// always want \r\n order.
            nIndex++;
            if (nIndex < nLen && pszDesc[nIndex] == '\r') // must have been in reverse order.
                nIndex++;
            continue;
        }

        strFixed += ch;
        nIndex++;
    }
}

CResInfo* EEKeeper::GetItem(const char *chRes, INF_ITM *pItm)
{
    char szRes[9];
    memcpy(szRes,chRes,8);
    szRes[8] = '\x0';

    CResInfo *pResInfo = _infKey.GetResInfo(RESTYPE_ITM,szRes);
    if (!pResInfo)
        return(NULL);

    CInfBifFile *pItemFile = _infKey.GetBifFile(pResInfo);
    if (!pItemFile || !pItemFile->IsValid())
        return(NULL);

    if (pItemFile->GetData(pResInfo->wLocator, pItm) == -1)
        return(NULL);

    return(pResInfo);
}

bool EEKeeper::GetItemType(quint16 wValue, QString &strType, int &nIndex)
{
    nIndex = 0;
    while(!_itemType[nIndex].pszType.isEmpty())
    {
        if (_itemType[nIndex].wType == wValue)
        {
            strType = _itemType[nIndex].pszType;
            return(true);
        }
        nIndex++;
    }

    // Added an "Unknown" type to the end of the list to make the item browser
    // a little easier to deal with. If the item being looked for is not in the
    // list then I'm returning the index to the "Unknown" item which is always
    // the versy last valid item in the list.
    nIndex--;
    strType = _itemType[nIndex].pszType;

    return(true);
}

void EEKeeper::MakeHexString(quint32 dwValue, char *pszResult, int nZeroPad)
{
    char szFormat[15];
    qsnprintf(szFormat, 12, "0x%%0%dX", nZeroPad);
    qsnprintf(pszResult, 12, szFormat, dwValue);
}

void EEKeeper::MakeHexString(quint32 dwValue, QString &strResult, int nZeroPad)
{
    strResult = QString("0x") + QString("%1").arg(dwValue, nZeroPad, 16, QChar('0')).toUpper();
}

bool EEKeeper::FindInstallPath(QString &strPath)
{
    //strPath = strPath;
    return false;

    /*
    Log::GetInstance().Write("Attempting to locate installation:\n");

    CRegKey rk[PATH_KEY_COUNT];
    int i;

    strPath.Empty();

    Log::GetInstance().Write("   Checking registry.\n");
    for (i=0;i<PATH_KEY_COUNT;i++)
        if (rk[i].Open((!i ? HKEY_LOCAL_MACHINE : rk[i-1].m_hKey),_pszInstall[i]) != ERROR_SUCCESS)
            break;

    // If it made it through to the end of the loop the last key in the array should have
    // the value we are looking for.
    if (i == PATH_KEY_COUNT)
    {
        Log::GetInstance().Write("   Registry key found.\n");
        quint32 dwSize = _MAX_PATH;
        char szPath[_MAX_PATH+1];
        //if (rk[PATH_KEY_COUNT-1].QueryValue(szPath,"Path",&dwSize) == ERROR_SUCCESS)
        //if (rk[PATH_KEY_COUNT-1].QueryValue(szPath,"DisplayIcon",&dwSize) == ERROR_SUCCESS)
        if (rk[PATH_KEY_COUNT-1].QueryStringValue(szPath,"DisplayIcon",&dwSize) == ERROR_SUCCESS)
        {
            Log::GetInstance().Write("   Got path from key.\n");
            strPath = szPath;
            strPath.TrimRight();
            if (!strPath.IsEmpty())
            {
                if (strPath.Right(1) != "\\")
                    strPath += "\\";
                return(true);
            }
            Log::GetInstance().Write("   Key was empty.\n");
        }
        else
        {
            Log::GetInstance().Write("   Failed to read registry key value.\n");
            return(false);
        }
    }

    Log::GetInstance().Write("   Searching default installation directory on fixed disks.\n");

    // If the registry entry wasn't found, search the most likely places to find the file.
    // Assuming people left the default install directories is probably the next fastest
    // way to attempt to find the file. Look on each drive for the default path looking
    // for Baldur.exe.
    quint32 dwDrives = GetLogicalDrives();

    quint32 dwMask;
    QString strFile;
    for (i=2;i<32;i++)
    {
        dwMask = (quint32)pow((float)2,i);
        if (dwDrives&dwMask)
        {
            strFile.Format("%c:\\",'A'+i);
            if (GetDriveType(strFile) == DRIVE_FIXED)
            {
                strFile.Format("%c:\\Program Files\\Baldur's Gate Enhanced Edition\\Data\\00766\\lang\\en_US\\dialog.tlk",'A'+i);
                if (GetFileAttributes(strFile) != 0xFFFFFFFF)
                {
                    strPath.Format("%c:\\Program Files\\Baldur's Gate Enhanced Edition\\Data\\00766\\",'A'+i);
                    QString strLog;
                    strLog.Format("   Found path: %s\n",(const char *)strPath);
                    Log::GetInstance().Write(strLog,true);
                    return(true);
                }
            }
        }
    }

    Log::GetInstance().Write("   Failed to locate BG:EE installation.\n");
    return(false);
    */
}

#ifndef EEKEEPER_H
#define EEKEEPER_H

#include "Inf2DA.h"
#include "InfBifFile.h"
#include "InfCreature.h"
#include "InfKey.h"
#include "InfTlk.h"
#include "PalImageList.h"
#include "SpellBitmaps.h"
#include "ValueList.h"

#include <QtCore>
#include <QImage>
#include <QWidget>

// Used to hold the resource type/file extension pairs for looking for
// extra data in the override foler. The data is filled set in this
// objects cpp file.
struct KEY_RES_OVERRIDE
{
    quint16     wResType;
    char		szExt[11];
};

#define ITEMTYPE_MISC						0x0000
#define ITEMTYPE_NECKLACE					0x0001
#define ITEMTYPE_ARMOR						0x0002
#define ITEMTYPE_BELT						0x0003
#define ITEMTYPE_BOOTS						0x0004
#define ITEMTYPE_ARROWS						0x0005
#define ITEMTYPE_GLOVES						0x0006
#define ITEMTYPE_HELM						0x0007
#define ITEMTYPE_KEY						0x0008
#define ITEMTYPE_POTION						0x0009
#define ITEMTYPE_RING						0x000A
#define ITEMTYPE_SCROLL						0x000B
#define ITEMTYPE_SHIELD						0x000C
#define ITEMTYPE_FOOD						0x000D
#define ITEMTYPE_BULLETS					0x000E
#define ITEMTYPE_BOW						0x000F
#define ITEMTYPE_DAGGER						0x0010
#define ITEMTYPE_MACE						0x0011
#define ITEMTYPE_SLING						0x0012
#define ITEMTYPE_SHORTSWORD                 0x0013
#define ITEMTYPE_SWORD						0x0014
#define ITEMTYPE_HAMMER						0x0015
#define ITEMTYPE_MORNINGSTAR				0x0016
#define ITEMTYPE_FLAIL						0x0017
#define ITEMTYPE_DART						0x0018
#define ITEMTYPE_AXE						0x0019
#define ITEMTYPE_STAFF						0x001A
#define ITEMTYPE_CROSSBOW					0x001B
#define ITEMTYPE_HANDTOHAND                 0x001C
#define ITEMTYPE_SPEAR						0x001D
#define ITEMTYPE_HALBERD					0x001E
#define ITEMTYPE_BOLTS						0x001F
#define ITEMTYPE_CLOAK						0x0020
#define ITEMTYPE_GOLD						0x0021
#define ITEMTYPE_GEM						0x0022
#define ITEMTYPE_WAND						0x0023
#define ITEMTYPE_CONTAINER					0x0024
#define ITEMTYPE_BOOK						0x0025
#define ITEMTYPE_FAMILIAR					0x0026
#define ITEMTYPE_INSTRUMENT                 0x0029	// Lyre of Progression?
#define ITEMTYPE_MISC2						0x002A	// I don't know what this is. "The Candle" uses it.
#define ITEMTYPE_CLUB						0x002C
#define ITEMTYPE_LARGESHIELD				0x002F
#define ITEMTYPE_MEDIUMSHIELD				0x0031
#define ITEMTYPE_SMALLSHIELD				0x0035
#define ITEMTYPE_MISC3						0x0037	// Telescope uses it.
#define ITEMTYPE_DRINK						0x0038	// Again not sure, wine and some extract uses this.
#define ITEMTYPE_GREATSWORD                 0x0039	// Another guess, 2 items use this.
#define ITEMTYPE_UNKNOWN                    0xFFFF
#define ITEMTYPE_END                        0x0040

struct ITEMTYPE
{
    QString pszType;
    quint16 wType;
};

struct ITEMPOS
{
    QString pszPos;
};

// Like above with the ITEMCATs, these need to be in sorting order. They
// are only used by the editor internally, so it doesn't matter what number
// are assigned.
#define SPELLCAT_INNATE						0x00
#define SPELLCAT_PRIEST						0x01
#define SPELLCAT_WIZARD						0x02
#define SPELLCAT_UNKNOWN					0x03

////////////////////////////////////////////
// Global Variables
////////////////////////////////////////////

// Holds the indexes for the resources.
extern CInfKey _infKey;

// All the string resources are in a .tlk file. Might as well just keep it open
// and available. Item descriptions, for example, are stored in here.
extern CInfTlk	_infTlk;

extern CSpellBitmaps _spellBitmaps;

// An image list which contains the samples used for setting character
// colors.
extern CPalImageList _ilPal;

// Path to the installation.
extern QString _strInstallPath;

// Language
extern QString _strLanguage;

// Path to the Documents folder
extern QString _strDocumentsPath;

// This is set to true if the program has indexed the resources. This needs to
// happen for the editor to work properly. Most of the menu items set their state
// by this flag.
extern bool _bResourcesLoaded;

// When set to true log file entries are generated. This can be set by putting
// LOG on the command line to start the editor.
extern bool _bLogFile;

// Used to register a clipboard format for holding the CRE's AFF data.
extern quint32 _nClipFormatAff;

// 2DAs
extern CInf2DA _2daHateRace;
extern CInf2DA _2daWeapProf;
extern CInf2DA _2daKitList;
extern CInf2DA _2daKit;

// Editor settings
extern bool	_bUseKnownSpellLimit;
extern bool	_bUseMemSpellLimit;
extern bool _bMemSpellsOnSave;		// Causes memorized spells to be refreshed as castable.
extern bool _bDisableSpellIcons;

extern int	_nKnownSpellLimit;
extern int	_nMemSpellLimit;

extern bool	_bDefaultOpenSinglePlayer;
extern bool _bUseGridLines;
extern bool _bIgnoreDataVersions;
extern bool _bAllowQuicksaves;

extern bool	_bAllowCHROverwrite;
extern bool	_bAllowCREOverwrite;

// all of our value lists
extern CValueList _vlLanguages;
extern CValueList _vlAnimations;
extern CValueList _vlGender;
extern CValueList _vlRacialEnemy;
extern CValueList _vlKit;
extern CValueList _vlNumAttacks;
extern CValueList _vlProfs;
extern CValueList _vlClass;
extern CValueList _vlAlignment;
extern CValueList _vlRace;
extern CValueList _vlAffects;
extern CValueList _vlKitList;
extern CValueList _vlEnemyAlly;
extern CValueList _vlState;
extern CValueList _vlSpells;

// Misc stuff
extern KEY_RES_OVERRIDE _keyResOver[];
extern ITEMPOS _itemPos[];
extern ITEMTYPE _itemType[];

class EEKeeper : public QObject
{
    Q_OBJECT

////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////
public:
    // Extracts a resource from the game and writes it to the path given. The path
    // should be a valid directory name. The file created will be the type of
    // name of the resource followed by the resource extension.
    static bool ExtractResource(quint16 wResourceType, const QString &pszResource, const QString &pszPath);

    // Saves a CRE file to the given location
    static bool SaveCreFile(CInfCreature *pCre, QString &strSavedAs, QWidget *parent);

    // Pass in a resource string and it will try to load the BMP resource. If the
    // bitmap doesn't exist as a resource it will try to load a file named that
    // from the portraits directory.
    // If nWidth and nHeight are non-zero this means you want the bitmap fit to
    // that specific size.
    static bool GetResourceBitmap(const QString &pszName, QImage &hBitmap, int nWidth=0, int nHeight=0, const QString &pszFileName=QString());

    // Pass in a spell resource name like "SPWI102" and get back a string that corresponds
    // in the SPELL.IDS list.
    static QString GetSpellScriptName(const char *pszResName);

    // Resize a given bitmap to the given height / width
    static bool ResizeBitmap(int nWidth, int nHeight, QImage &hBitmap, QRgb clrTrans = qRgba(0, 0, 0, 0));

    // Gets a given spells bitmap
    static bool GetSpellBitmap(const CResInfo *pInfo, INF_SPL &spl, const QRgb &clrTrans, QImage &hBitmap);

    // Gets a given image bitmap
    static bool GetItemBitmap(const CResInfo *pInfo, INF_ITM &itm, int nFrameWanted, QRgb clrTrans, QImage &hBitmap);

    // Fixes a TLK Description
    static void FixTlkDescription(const char *pszDesc, QString &strFixed);

    // Gets an ITM from the given resource
    static CResInfo* GetItem(const char *chRes, INF_ITM *pItm);

    // Gets an item type from a given value
    static bool GetItemType(quint16 wValue, QString &strType, int &nIndex);

    // Creates a hex string from the given value
    static void MakeHexString(quint32 dwValue, char *pszResult, int nZeroPad);

    // Creates a hex string with the given padding
    static void MakeHexString(quint32 dwValue, QString &strResult, int nZeroPad);

    // Attempt to determine the install path
    static bool FindInstallPath(QString &strPath);

public:
    static EEKeeper& GetInstance() {
        static EEKeeper keeper;
        return keeper;
    }

private:
    EEKeeper();
    EEKeeper(const EEKeeper &) : QObject() { }
    EEKeeper& operator=(const EEKeeper &);
    ~EEKeeper() { }
};

#endif // EEKEEPER_H

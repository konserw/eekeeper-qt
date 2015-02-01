#ifndef INFCREATURE_H
#define INFCREATURE_H

/************************************************************************************
    Copyright (c) 2000 Aaron O'Neil
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

        1) Redistributions of source code must retain the above copyright notice,
                this list of conditions and the following disclaimer.
        2) Redistributions in binary form must reproduce the above copyright notice,
                this list of conditions and the following disclaimer in the documentation
                and/or other materials provided with the distribution.
        3) Redistributions in binary form must reproduce the above copyright notice on
                program startup. Additional credits for program modification are acceptable
                but original copyright and credits must be visible at startup.
        4) You may charge a reasonable copying fee for any distribution of Shadow Keeper.
                You may charge any fee you choose for support of Shadow Keeper. You may not
                charge a fee for Shadow Keeper itself.

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**************************************************************************************/

#include "defines.h"
#include <QtCore>
#include <QFile>
#include <QList>

#pragma pack(push,1)

// Proficiencies are packed 3 bits each. Hi 3 bits is the 1st class of a
// dual class character.
#define HITRIBBLE(byte)					((quint8)((byte >> 3) & 0x07))
#define LOTRIBBLE(byte)					((quint8)(byte & 0x07))
#define MAKETRIBBLE(loTrib,hiTrib)	((quint8)((hiTrib << 3) | (loTrib & 0x07)))

#define CRE_STAT_DEAD			0x00000800
#define CRE_STAT_ACID_DEAD		0x00000400
#define CRE_STAT_FLAME_DEAD     0x00000200
#define CRE_STAT_EXPLODE_DEAD	0x00000100
#define CRE_STAT_STONE_DEAD     0x00000080
#define CRE_STAT_FROZEN_DEAD	0x00000040

// Creature flags
#define CRE_FLAG_LONG_TOOLTIP   0x00000001 // long name in tooltip
#define CRE_FLAG_NO_CORPSE      0x00000002
#define CRE_FLAG_KEEP_CORPSE    0x00000004
#define CRE_FLAG_WAS_FIGHTER    0x00000008 // set if dual classed from a fighter
#define CRE_FLAG_WAS_MAGE       0x00000010
#define CRE_FLAG_WAS_CLERIC     0x00000020
#define CRE_FLAG_WAS_THIEF      0x00000040
#define CRE_FLAG_WAS_DRUID      0x00000080
#define CRE_FLAG_WAS_RANGER     0x00000100
#define CRE_FLAG_FALLEN_PALADIN 0x00000200
#define CRE_FLAG_FALLEN_RANGER  0x00000400
#define CRE_FLAG_EXPORTABLE     0x00000800
#define CRE_FLAG_HIDE_TOOLTIP   0x00001000 // hide damage status in tooltip
#define CRE_FLAG_QUEST_CRITICAL 0x00002000 // quest critical or alt damage
#define CRE_FLAG_CANNOT_NPC     0x00004000 // can activate cannot be used by NPC triggers
#define CRE_FLAG_BEEN_IN_PARTY  0x00008000
#define CRE_FLAG_RESTORE_ITEM   0x00010000 // restore item in hand
#define CRE_FLAG_UNSET_BIT16    0x00020000 // unsets bit 16 (CRE_FLAG_RESTORE_ITEM)
#define CRE_FLAG_BIT18          0x00040000 // unknown
#define CRE_FLAG_BIT19          0x00080000 // unknown
#define CRE_FLAG_BIT20          0x00100000 // unknown
#define CRE_FLAG_BIT21          0x00200000 // unknown
#define CRE_FLAG_BIT22          0x00400000 // unknown
#define CRE_FLAG_BIT23          0x00800000 // unknown
#define CRE_FLAG_WALK_EA        0x01000000 // random walk (ea)
#define CRE_FLAG_WALK_GENERAL   0x02000000 // random walk (general)
#define CRE_FLAG_WALK_RACE      0x04000000 // random walk (race)
#define CRE_FLAG_WALK_CLASS     0x08000000 // random walk (class)
#define CRE_FLAG_WALK_SPECIFIC  0x10000000 // random walk (specific)
#define CRE_FLAG_WALK_GENDER    0x20000000 // random walk (gender)
#define CRE_FLAG_WALK_ALIGNMENT 0x40000000 // random walk (alignment)
#define CRE_FLAG_UNINTERUPTABLE 0x80000000 // memory only

// some helper defines
#define CRE_FLAG_HAS_DUALCLASS  0x000001F8 // or of all the CRE_FLAG_WAS_* flags

struct INF_CRE
{
    char		chSignature[0x04];
    char		chVersion[0x04];
    quint32		dwLongCreatureName;
    quint32		dwShortCreatureName;

    // see CRE_FLAG_* above for possible flags
    quint32     dwFlags;

    quint32		dwExpForKilling;
    quint32		dwExp;
    quint32		dwGold;
    quint32		dwStateFlags;
    quint16		wCurrentHP;
    quint16		wBaseHP;
    quint16		wAnimationID;

    char		chUnknown1[0x02];

    quint8		chMetalColor;
    quint8		chMinorColor;
    quint8		chMajorColor;
    quint8		chSkinColor;
    quint8		chLeatherColor;
    quint8		chArmorColor;
    quint8		chHairColor;

    char		chEffStructure; // 0 = v1, 1 = v2

    char		chSmallPortrait[8];
    char		chLargePortrait[8];
    quint8		chReputation;
    quint8		chHideInShadows;
    qint16		nAC1;
    qint16		nAC2;
    qint16		nACModCrushing;
    qint16		nACModMissile;
    qint16		nACModPiercing;
    qint16		nACModSlashing;
    char		chTHAC0;
    quint8		chAttacks;
    quint8		chSaveDeath;
    quint8		chSaveWands;
    quint8		chSavePoly;
    quint8		chSaveBreath;
    quint8		chSaveSpells;
    quint8		chResistFire;
    quint8		chResistCold;
    quint8		chResistElectricity;
    quint8		chResistAcid;
    quint8		chResistMagic;
    quint8		chResistMagicFire;
    quint8		chResistMagicCold;
    quint8		chResistSlashing;
    quint8		chResistCrushing;
    quint8		chResistPiercing;
    quint8		chResistMissile;

    quint8		chDetectIllusions;
    quint8		chSetTraps;
    quint8		chLore;
    quint8		chOpenLocks;
    quint8		chMoveSilently;
    quint8		chFindTraps;
    quint8		chPickPockets;
    quint8		chFatique;
    quint8		chIntoxication;
    quint8		chLuck;

    // These proficienciens seem to be obsolete in BGII. Instead they are
    // tacked onto the end of the CRE with affect structs. I've made two
    // function to replace these: GetProf and SetProf. It uses the
    // proficiency values from global.h.
    quint8		chProfLargeSwords;
    quint8		chProfSmallSwords;
    quint8		chProfBows;
    quint8		chProfSpears;
    quint8		chProfAxes;
    quint8		chProfMissiles;
    quint8		chProfGreatSwords;
    quint8		chProfDaggers;
    quint8		chProfHalberds;
    quint8		chProfMaces;
    quint8		chProfFlails;
    quint8		chProfHammers;
    quint8		chProfClubs;
    quint8		chProfQuarterstaffs;
    quint8		chProfCrossbows;

    char		chUnknown2[0x06];

    quint8		chTracking;

    char		chUnknown3[0x20];

    quint32		dwStrRefs[100];
    quint8		chLevelFirstClass;
    quint8		chLevelSecondClass;
    quint8		chLevelThirdClass;

    quint8		chSex; // from gender.ids via sex stat

    quint8		chStrength;
    quint8		chStrengthBonus;
    quint8		chIntelligence;
    quint8		chWisdom;
    quint8		chDexterity;
    quint8		chConstitution;
    quint8		chCharisma;
    quint8		chMorale;
    quint8		chMoraleBreak;
    quint8		chRacialEnemy;

    quint16		wMoralRecoveryTime;

    quint32		dwKit;
    char		chOverrideScript[8];
    char		chClassScript[8];
    char		chRaceScript[8];
    char		chGeneralScript[8];
    char		chDefaultScript[8];

    quint8		chEnemyAlly;
    quint8		chGeneral;
    quint8		chRace;
    quint8		chClass;
    quint8		chSpecific;
    quint8		chGender;

    quint8		chObjectReferences[5]; // OBJECT.IDS references

    quint8		chAlignment;

    quint16     wGlobalActorEnumeration;
    quint16     wLocalActorEnumeration;

    char		chName[32]; // death variable: SPRITE_IS_DEAD on death
    quint32		dwKnownSpellsOffset;
    quint32		dwKnownSpellsCount;
    quint32		dwSpellMemorizationInfoOffset;
    quint32		dwSpellMemorizationInfoCount;
    quint32		dwMemorizedSpellsOffset;
    quint32		dwMemorizedSpellsCount;
    quint32		dwItemSlotsOffset;
    quint32		dwItemsOffset;
    quint32		dwItemsCount;
    quint32		dwAffectOffset;
    quint32		dwAffectCount;

    char		chDialogRef[8];
};

#define AFF_TYPE_ABILITY			0x0A
#define AFF_TYPE_SPELL				0x7E		// Don't really have a clue what 7E is, but this is good for now.
#define AFF_TYPE_CONTINGENCY		0xE8
#define AFF_TYPE_PROF				0xE9

#define AFF_TARG_CRE				0x02

struct INF_AFF
{
    char		chSignature[4];			// Zeroed out for V2 EFFs.
    char		chVersion[4];			// Zeroed out for V2 EFFs.

    quint32		dwAffectType;
    quint32		dwTargetType;

    quint32		dwUnknown;

    qint32      nNP1;
    qint32      nNP2;
    quint32		dwFlags;
    qint32      nTime;
    quint16		wProbability1;
    quint16		wProbability2;
    char		chResRef0[8];
    quint32		dwDieSides;
    quint32		dwDieCount;
    quint32		dwSaveType;
    quint32		dwSaveBonus;
    quint32		dwUnknown1[3];
    quint32		dwDieSides2;
    quint32		dwDieCount2;
    quint32		dwFlags2;
    quint32		dwUnknown2[4];
    char		chResRef1[8];
    char		chResRef2[8];
    quint32		dwUnknown3[5];
    char		chResRef3[8];
    quint32		dwUnknown4[3];
    char		chName[32];
    quint32		dwUnknown5[18];
};

// Spell types as defined in the CRE spell data. Note that it is also important that
// the memorized section of spells should be written out in this same order. The
// memorization info block has indices to the start of each particular spell level
// following the memorization info block. It might work to put them out of order as
// long as you figured out the indices properly for the info block, but I've used a
// simple loop to calculate them, and it's based on writing them out in the order of
// priest, wizard, innate (which is the same order the game uses).
#define INF_CRE_ST_PRIEST		0
#define INF_CRE_ST_WIZARD		1
#define INF_CRE_ST_INNATE		2
#define INF_CRE_SPELLTYPES		3		// Number of known types.

struct INF_CRE_KNOWNSPELL
{
    char		chSpellName[8];
    quint16		wSpellLevel;
    quint16		wSpellType;
};

struct INF_CRE_MEMINFO
{
    quint16		wSpellLevel;
    quint16		wNumMemorizable1;
    quint16		wNumMemorizable2;
    quint16		wSpellType;
    quint32		dwSpellIndex;
    quint32		dwSpellCount;
};

struct INF_CRE_MEMSPELL
{
    char		chSpellName[8];
    quint32		dwMemorized;		// 00 = not memorized, 01 = memorized.
};

#define INF_NUM_ITEMSLOTS			38

#define INF_ITEMPOS_HEAD			0
#define INF_ITEMPOS_BODY			1
#define INF_ITEMPOS_SHIELD			2
#define INF_ITEMPOS_HANDS			3
#define INF_ITEMPOS_FINGER1         4
#define INF_ITEMPOS_FINGER2         5
#define INF_ITEMPOS_NECK			6
#define INF_ITEMPOS_WAIST			7
#define INF_ITEMPOS_FEET			8
#define INF_ITEMPOS_QUICKWEAPON1	9
#define INF_ITEMPOS_QUICKWEAPON2	10
#define INF_ITEMPOS_QUICKWEAPON3	11
#define INF_ITEMPOS_QUICKWEAPON4	12
#define INF_ITEMPOS_QUIVER1         13
#define INF_ITEMPOS_QUIVER2         14
#define INF_ITEMPOS_QUIVER3         15
#define INF_ITEMPOS_UNUSED			16
#define INF_ITEMPOS_BACK			17
#define INF_ITEMPOS_QUICKSLOT1      18
#define INF_ITEMPOS_QUICKSLOT2      19
#define INF_ITEMPOS_QUICKSLOT3      20
#define INF_ITEMPOS_INV				21		// 16 of these total.

struct INF_CRE_ITEMSLOTS
{
    quint16		wItemSlot[INF_NUM_ITEMSLOTS];
    char		chUnknown[4];
};

struct INF_CRE_ITEM
{
    char		chResName[8];
    char		chUnknown[2];
    quint16		wQuantity1;
    quint16		wQuantity2;
    quint16		wQuantity3;
    char		chIdentified;	// 00 = not ID'd, 01 = ID'd.
    char		chUknown1[3];
};

// This is used by the CInfCre object to hold the spell data. The original
// CRE spell info is read in and converted to a structure of this type. Also,
// this is how it passes data out of the object regarding spells and expects
// data to be passed back in in this format.	This data is memcpy'd so don't
// put any object in it.
struct SPELLDATA
{
    char	szResName[9];
    quint16	wLevel;
    quint16	wType;

    // This is the number of times the creature has the spell set to be memorized.
    qint32	nTimesMemorized;

    // This is the number of times the creature can still cast the spell. Before
    // they need to rest.
    qint32	nTimesCanCast;
};

// Used as an intermediate format for the retrieval and setting of the memorization
// maximums for each spell.
struct MEMINFO
{
    quint16	wType;
    quint16	wLevel;
    quint16	wNumMemorizable;
};

// Used for transferring lists of proficiencies in and out of the class.
class CProfData
{
public:
    CProfData() : m_chProf(0), m_nFirstClass(0), m_nSecondClass(0) { }

    quint8	m_chProf;
    int	m_nFirstClass;
    int	m_nSecondClass;
};

#pragma pack(pop)

typedef QList<INF_AFF *> INF_AFF_PLIST;
typedef QList<SPELLDATA *> SPELLDATA_PLIST;
typedef QList<CProfData *> CProfDataPlist;

#define	ERR_CRE_READHEADER			1001
#define	ERR_CRE_READBADKNOWN		1002
#define	ERR_CRE_READTOOMANYSPELLS	1003
#define	ERR_CRE_READKNOWN			1004
#define	ERR_CRE_READBADMEMINFO		1005
#define	ERR_CRE_READTOOMANYMEMINFO	1006
#define	ERR_CRE_READMEMINFO			1007
#define	ERR_CRE_READBADMEM			1008
#define	ERR_CRE_READTOOMANYMEM		1009
#define	ERR_CRE_READMEM				1010
#define	ERR_CRE_READBADITEMSLOTS	1011
#define	ERR_CRE_READITEMSLOTS		1012
#define	ERR_CRE_READBADITEMLIST		1013
#define	ERR_CRE_READTOOMANYITEMS	1014
#define	ERR_CRE_READITEM			1015
#define	ERR_CRE_WRITEHEADER			1016
#define	ERR_CRE_WRITEKNOWNSPELLS	1017
#define	ERR_CRE_WRITEMEMINFO		1018
#define	ERR_CRE_WRITEMEMSPELLS		1019
#define	ERR_CRE_WRITEITEMSLOTS		1020
#define	ERR_CRE_WRITEITEMS			1021
#define	ERR_CRE_OOM_AFFECTS			1022
#define	ERR_CRE_READAFFECTS			1023
#define	ERR_CRE_WRITEAFFECTS		1024
#define	ERR_CRE_MISSINGSIG			1025
#define	ERR_CRE_BADVERSION			1026
#define	ERR_CRE_OOM_KNOWNSPELLS		1027
#define	ERR_CRE_OOM_MEMINFO			1028
#define	ERR_CRE_OOM_MEMSPELL		1029
#define	ERR_CRE_UNKNOWNSPELLTYPE	1030
#define	ERR_CRE_OOM_SPELLDATA		1031
#define	ERR_CRE_READBADAFFECTS		1032
#define	ERR_CRE_BADAFFECTCOUNT		1033
#define	ERR_CRE_WRITENOTKNOWNSPELL	1034

struct INF_GAME_CHARINFO;

class CInfCreature
{
public:
    CInfCreature();
    ~CInfCreature();

    // Read the CRE record from the file. The file position must already be
    // placed in the right spot.
    bool	Read(QFile &file);

    // Writes the current data to the file.
    bool	Write(QFile &file);

    int	GetLastError() const { return(m_nError); }

    // Returns the number of bytes needed to store the information on disk.
    // This value is used by CInfGame to create the header entries when
    // recreating the Icewind.gam file.
    quint32	GetFileSpace();

    // Returns the resource string for the large character portrait.
    void	GetResLargePortrait(QString &strRes);

    void	GetName(QString &strName);
    void	SetName(const char *pszName);
    bool    IsNPC() const { return (m_infCre.dwLongCreatureName != 0xFFFFFFFF || m_infCre.dwShortCreatureName != 0xFFFFFFFF); }

    quint32 GetLongNameID() const { return(m_infCre.dwLongCreatureName); }
    void	SetLongNameID(quint32 dwValue);
    quint32 GetShortNameID() const { return(m_infCre.dwShortCreatureName); }
    void	SetShortNameID(quint32 dwValue);

    quint32	GetDualClass() const { return(m_infCre.dwFlags & CRE_FLAG_HAS_DUALCLASS); }
    void	SetDualClass(quint32 nValue=0);

    // toggles the given flag
    quint32 GetFlags() const { return(m_infCre.dwFlags); }
    void UnsetFlag(quint32 nFlag=0) { m_infCre.dwFlags &= ~nFlag; }
    void SetFlag(quint32 nFlag=0) { m_infCre.dwFlags |= nFlag; }

    quint8	GetFirstClassLevel() const { return(m_infCre.chLevelFirstClass); }
    void	SetFirstClassLevel(int nValue);
    quint8	GetSecondClassLevel() const { return(m_infCre.chLevelSecondClass); }
    void	SetSecondClassLevel(int nValue);
    quint8	GetThirdClassLevel() const { return(m_infCre.chLevelThirdClass); }
    void	SetThirdClassLevel(int nValue);

    quint8	GetStr() const { return(m_infCre.chStrength); }
    void	SetStr(int nStr);
    quint8	GetStrBonus() const { return(m_infCre.chStrengthBonus); }
    void	SetStrBonus(int nBonus);
    quint8	GetDex() const { return(m_infCre.chDexterity); }
    void	SetDex(int nDex);
    quint8	GetCon() const { return(m_infCre.chConstitution); }
    void	SetCon(int nCon);
    quint8	GetInt() const { return(m_infCre.chIntelligence); }
    void	SetInt(int nInt);
    quint8	GetWis() const { return(m_infCre.chWisdom); }
    void	SetWis(int nWis);
    quint8	GetCha() const { return(m_infCre.chCharisma); }
    void	SetCha(int nCha);

    quint8	GetAttacks() const { return(m_infCre.chAttacks); }
    void	SetAttacks(int nValue);
    short	GetAC() const { return(m_infCre.nAC1); }
    void	SetAC(int nValue);
    char	GetThac0() const { return(m_infCre.chTHAC0); }
    void	SetThac0(int nValue);

    quint32	GetExp() const { return(m_infCre.dwExp); }
    void	SetExp(quint32 dwExp);
    quint32 GetExpForKill() const { return(m_infCre.dwExpForKilling); }
    void  SetExpForKill(quint32 dwExp);
    quint32	GetGold() const { return(m_infCre.dwGold); }
    void	SetGold(quint32 dwGold);

    quint16	GetBaseHP() const { return(m_infCre.wBaseHP); }
    void	SetBaseHP(quint16 wBaseHP);
    quint16	GetCurrentHP() const { return(m_infCre.wCurrentHP); }
    void	SetCurrentHP(quint16 wCurrentHP);
    quint8	GetLore() const { return(m_infCre.chLore); }
    void	SetLore(int nValue);

    // The reputation value seems to be 10x what is shown in the game.
    quint8	GetReputation() const { return(m_infCre.chReputation/10); }
    void	SetReputation(int nValue);

    quint8	GetSaveDeath() const { return(m_infCre.chSaveDeath); }
    void	SetSaveDeath(int nValue);
    quint8	GetSaveWands() const { return(m_infCre.chSaveWands); }
    void	SetSaveWands(int nValue);
    quint8	GetSavePoly() const { return(m_infCre.chSavePoly); }
    void	SetSavePoly(int nValue);
    quint8	GetSaveBreath() const { return(m_infCre.chSaveBreath); }
    void	SetSaveBreath(int nValue);
    quint8	GetSaveSpells() const { return(m_infCre.chSaveSpells); }
    void	SetSaveSpells(int nValue);

    quint8	GetProfLargeSwords() const { return(LOTRIBBLE(m_infCre.chProfLargeSwords)); }
    void	SetProfLargeSwords(int nValue);
    quint8	GetProfSmallSwords() const { return(LOTRIBBLE(m_infCre.chProfSmallSwords)); }
    void	SetProfSmallSwords(int nValue);
    quint8	GetProfBows() const { return(LOTRIBBLE(m_infCre.chProfBows)); }
    void	SetProfBows(int nValue);
    quint8	GetProfSpears() const { return(LOTRIBBLE(m_infCre.chProfSpears)); }
    void	SetProfSpears(int nValue);
    quint8	GetProfClubs() const { return(LOTRIBBLE(m_infCre.chProfClubs)); }
    void	SetProfClubs(int nValue);
    quint8	GetProfFlails() const { return(LOTRIBBLE(m_infCre.chProfFlails)); }
    void	SetProfFlails(int nValue);
    quint8	GetProfAxes() const { return(LOTRIBBLE(m_infCre.chProfAxes)); }
    void	SetProfAxes(int nValue);
    quint8	GetProfMissiles() const { return(LOTRIBBLE(m_infCre.chProfMissiles)); }
    void	SetProfMissiles(int nValue);
    quint8	GetProfGreatSwords() const { return(LOTRIBBLE(m_infCre.chProfGreatSwords)); }
    void	SetProfGreatSwords(int nValue);
    quint8	GetProfDaggers() const { return(LOTRIBBLE(m_infCre.chProfDaggers)); }
    void	SetProfDaggers(int nValue);
    quint8	GetProfHalberds() const { return(LOTRIBBLE(m_infCre.chProfHalberds)); }
    void	SetProfHalberds(int nValue);
    quint8	GetProfMaces() const { return(LOTRIBBLE(m_infCre.chProfMaces)); }
    void	SetProfMaces(int nValue);
    quint8	GetProfCrossbows() const { return(LOTRIBBLE(m_infCre.chProfCrossbows)); }
    void	SetProfCrossbows(int nValue);
    quint8	GetProfQuarterStaffs() const { return(LOTRIBBLE(m_infCre.chProfQuarterstaffs)); }
    void	SetProfQuarterStaffs(int nValue);
    quint8	GetProfHammers() const { return(LOTRIBBLE(m_infCre.chProfHammers)); }
    void	SetProfHammers(int nValue);

    quint8	GetProfLargeSwords2() const { return(HITRIBBLE(m_infCre.chProfLargeSwords)); }
    void	SetProfLargeSwords2(int nValue);
    quint8	GetProfSmallSwords2() const { return(HITRIBBLE(m_infCre.chProfSmallSwords)); }
    void	SetProfSmallSwords2(int nValue);
    quint8	GetProfBows2() const { return(HITRIBBLE(m_infCre.chProfBows)); }
    void	SetProfBows2(int nValue);
    quint8	GetProfSpears2() const { return(HITRIBBLE(m_infCre.chProfSpears)); }
    void	SetProfSpears2(int nValue);
    quint8	GetProfClubs2() const { return(HITRIBBLE(m_infCre.chProfClubs)); }
    void	SetProfClubs2(int nValue);
    quint8	GetProfFlails2() const { return(HITRIBBLE(m_infCre.chProfFlails)); }
    void	SetProfFlails2(int nValue);
    quint8	GetProfAxes2() const { return(HITRIBBLE(m_infCre.chProfAxes)); }
    void	SetProfAxes2(int nValue);
    quint8	GetProfMissiles2() const { return(HITRIBBLE(m_infCre.chProfMissiles)); }
    void	SetProfMissiles2(int nValue);
    quint8	GetProfGreatSwords2() const { return(HITRIBBLE(m_infCre.chProfGreatSwords)); }
    void	SetProfGreatSwords2(int nValue);
    quint8	GetProfDaggers2() const { return(HITRIBBLE(m_infCre.chProfDaggers)); }
    void	SetProfDaggers2(int nValue);
    quint8	GetProfHalberds2() const { return(HITRIBBLE(m_infCre.chProfHalberds)); }
    void	SetProfHalberds2(int nValue);
    quint8	GetProfMaces2() const { return(HITRIBBLE(m_infCre.chProfMaces)); }
    void	SetProfMaces2(int nValue);
    quint8	GetProfCrossbows2() const { return(HITRIBBLE(m_infCre.chProfCrossbows)); }
    void	SetProfCrossbows2(int nValue);
    quint8	GetProfQuarterStaffs2() const { return(HITRIBBLE(m_infCre.chProfQuarterstaffs)); }
    void	SetProfQuarterStaffs2(int nValue);
    quint8	GetProfHammers2() const { return(HITRIBBLE(m_infCre.chProfHammers)); }
    void	SetProfHammers2(int nValue);

    quint8	GetResFire() const { return(m_infCre.chResistFire); }
    void	SetResFire(int nValue);
    quint8	GetResCold() const { return(m_infCre.chResistCold); }
    void	SetResCold(int nValue);
    quint8	GetResElectricity() const { return(m_infCre.chResistElectricity); }
    void	SetResElectricity(int nValue);
    quint8	GetResAcid() const { return(m_infCre.chResistAcid); }
    void	SetResAcid(int nValue);
    quint8	GetResMagic() const { return(m_infCre.chResistMagic); }
    void	SetResMagic(int nValue);
    quint8	GetResMagicFire() const { return(m_infCre.chResistMagicFire); }
    void	SetResMagicFire(int nValue);
    quint8	GetResMagicCold() const { return(m_infCre.chResistMagicCold); }
    void	SetResMagicCold(int nValue);
    quint8	GetResSlashing() const { return(m_infCre.chResistSlashing); }
    void	SetResSlashing(int nValue);
    quint8	GetResCrushing() const { return(m_infCre.chResistCrushing); }
    void	SetResCrushing(int nValue);
    quint8	GetResPiercing() const { return(m_infCre.chResistPiercing); }
    void	SetResPiercing(int nValue);
    quint8	GetResMissile() const { return(m_infCre.chResistMissile); }
    void	SetResMissile(int nValue);

    quint8	GetOpenLocks() const { return(m_infCre.chOpenLocks); }
    void	SetOpenLocks(int nValue);
    quint8	GetMoveSilently() const { return(m_infCre.chMoveSilently); }
    void	SetMoveSilently(int nValue);
    quint8	GetFindTraps() const { return(m_infCre.chFindTraps); }
    void	SetFindTraps(int nValue);
    quint8	GetPickPockets() const { return(m_infCre.chPickPockets); }
    void	SetPickPockets(int nValue);
    quint8	GetHideInShadows() const { return(m_infCre.chHideInShadows); }
    void	SetHideInShadows(int nValue);
    quint8	GetDetectIllusions() const { return(m_infCre.chDetectIllusions); }
    void	SetDetectIllusions(int nValue);
    quint8	GetSetTraps() const { return(m_infCre.chSetTraps); }
    void  SetSetTraps(int nValue);

    quint8	GetRace() const { return(m_infCre.chRace); }
    void	SetRace(int nValue);
    quint8  GetGender() const { return(m_infCre.chGender); }
    void	SetGender(int nValue);
    quint8	GetAlignment() const { return(m_infCre.chAlignment); }
    void	SetAlignment(int nValue);
    quint8	GetClass() const { return(m_infCre.chClass); }
    void	SetClass(int nValue);
    quint32	GetKit() const { return(m_infCre.dwKit); }
    void	SetKit(quint32 dwValue);
    quint8	GetRacialEnemy() const { return(m_infCre.chRacialEnemy); }
    void	SetRacialEnemy(int nValue);
    quint8	GetEnemyAlly() const { return(m_infCre.chEnemyAlly); }
    void	SetEnemyAlly(int nValue);

    quint16	GetAnimationID() const { return(m_infCre.wAnimationID); }
    void	SetAnimationID(quint16 wValue);

    quint32 GetStateFlags() const { return(m_infCre.dwStateFlags); }
    void  SetStateFlags(quint32 dwValue);

    void	GetSmallPortrait(QString &strResource);
    void	SetSmallPortrait(const char *pszResource);
    void	GetLargePortrait(QString &strResource);
    void	SetLargePortrait(const char *pszResource);

    quint8	GetSkinColor() const { return(m_infCre.chSkinColor); }
    void	SetSkinColor(quint8 chColor);
    quint8	GetHairColor() const { return(m_infCre.chHairColor); }
    void	SetHairColor(quint8 chColor);
    quint8	GetMajorColor()	const { return(m_infCre.chMajorColor); }
    void	SetMajorColor(quint8 chColor);
    quint8	GetMinorColor() const { return(m_infCre.chMinorColor); }
    void	SetMinorColor(quint8 chColor);
    quint8	GetArmorColor() const { return(m_infCre.chArmorColor); }
    void	SetArmorColor(quint8 chColor);
    quint8	GetMetalColor() const { return(m_infCre.chMetalColor); }
    void	SetMetalColor(quint8 chColor);
    quint8	GetLeatherColor() const { return(m_infCre.chLeatherColor); }
    void	SetLeatherColor(quint8 chColor);

    // Returns the movement modifier of a character. These are actually character
    // affects. If it exists the value is returned, if not zero is.
    quint32	GetSpeed();

    // Sets the character speed affect. If Zero is passed in the affect is removed.
    void	SetSpeed(quint32 dwSpeed);

    // Gets a list proficiencies from teh affects list. The passed in list will
    // be filled with CProfData objects. The list should be empty before being
    // passed in. The function will just add the profs to the tail of the list
    // if you don't clear it first. The caller is responsible for deleting the
    // objects in the list.
    void GetProfs(CProfDataPlist &list);

    // Pass in a list of CProfData objects. Creates the affect records for the
    // CRE. All the items in the passed in are deleted and the list is cleared.
    void SetProfs(CProfDataPlist &list);

    // Get a list of all the character affects. It will not return the proficiencies
    // or the speed affect since those are handled separately.
    bool GetAffects(INF_AFF_PLIST &list);

    // Set the list of affects for a character. All existing affects are deleted and
    // this list is used. This does not affect the proficiencies or the move speed affect.
    // This function does not make copies of the data. It retains the pointers passed
    // in via the list. Do not delete the items in this list. The list will be cleared
    // once the items are transferred.
    void SetAffects(INF_AFF_PLIST &list);

    // Needs to be a pointer to an array of INF_NUM_ITEMSLOTS items. This returns a fully
    // expanded copy of the data. Meaning if there are only 10 items in the file there
    // will still be returned a full INF_NUM_ITEMSLOTS array. Each of the items is
    // placed in the array according to the positions in the INF_CRE_ITEMSLOTS. Positions
    // that don't have an item are set to zero.
    void	GetItems(INF_CRE_ITEM *pItems);

    // Should be passed a full INF_NUM_ITEMSLOTS array. Any items that have a non-null
    // resource string will be placed in the characters inventory. Calling this function
    // also sets up the INF_CRE_ITEMSLOTS data.
    void	SetItems(const INF_CRE_ITEM *pItems);

    // Returns the number of spells currently of a particular type. There is obviously
    // no range checking done on this function, so make sure you use the INF_CRE_ST_
    // defines.
    int	GetSpellCount(int nType) const { return(m_nSpellCount[nType]); }

    // Gets a copy of the spell data. This is a copy of the internal data so pData needs
    // to point to a buffer big enough to hold as many items as GetSpellCount returns.
    void	GetSpells(int nType, SPELLDATA *pData);

    // Sets a list of spells for a particular type. Calling this function copies the data
    // pointed to by pData to internal buffers.
    void	SetSpells(int nType, int nSpellCount, const SPELLDATA *pData);

    int	GetMemorizationInfoCount() const { return(m_infCre.dwSpellMemorizationInfoCount); }

    // Gets a copy of the memorization maximums. pMemInfo must point to a buffer big enough
    // to hold as many items as GetMemorizationInfoCount returns.
    void	GetMemorizationInfo(MEMINFO *pMemInfo);

    // Must point to an array of GetMemorizationInfoCount size.
    void	SetMemorizationInfo(const MEMINFO *pMemInfo);

    bool	HasChanged() const { return(m_bHasChanged); }

    // Sets the CharInfo for reference for PC names
    void    SetCharInfo(INF_GAME_CHARINFO *pCharInfo) { m_pCharInfo = pCharInfo; }

private:
    // Called before calculation or writing any spell information. This function updates
    // the CRE header and recalculates all the data in the memorization info buffer.
    void	UpdateSpellInfo();

private:
    INF_CRE				m_infCre;
    INF_CRE_KNOWNSPELL	*m_pKnownSpells;
    INF_CRE_MEMINFO		*m_pMemInfo;
    INF_CRE_MEMSPELL	*m_pMemSpells;
    INF_CRE_ITEMSLOTS	m_infItemSlots;
    INF_CRE_ITEM		m_infItems[INF_NUM_ITEMSLOTS];	// Can only be as many as there are item slots.
    SPELLDATA			*m_pSpellData[INF_CRE_SPELLTYPES];
    int					m_nSpellCount[INF_CRE_SPELLTYPES];
    INF_AFF_PLIST       m_plAffects;
    INF_GAME_CHARINFO   *m_pCharInfo; // pointer to this characters charinfo from game

    // It appears characters can have spells memorized that they don't actually
    // know. In particular, it looks like when a character casts a spell sequencer
    // a different version of the spell gets added to the morized list. SPWI420
    // I think is the spell, while when you use it, you end up with a SPWI420D in
    // your memorized list. Previously, I assumed all spells in the memorized list
    // had a matching "Known Spell" entry. Any spells in this list get recreated
    // only in the memorized section when writing the CRE. I put them in a separate
    // list to avoid letting the user seem them in the spell boxes and to make life
    // easier on the alrady existing spell code. It seems to be counting these as
    // innate abilities too. Game locks up if you don't count them in the memorization
    // block as an innate.
    SPELLDATA_PLIST     m_plMemButNotKnown;

    bool	m_bHasChanged;
    int 	m_nError;
};

#endif // INFCREATURE_H

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

#ifndef INFBIFFILE_H
#define INFBIFFILE_H

#include <QtCore>
#include <QFile>

#include "InfBam.h"

#pragma pack(push,1)

struct INF_BIF_HEADER
{
    char		chSignature[4];
    char		chVersion[4];
    quint32		dwFileEntryCount;
    quint32		dwTileEntryCount;
    quint32		dwFileEntryOffset;		// Tileset entries will follow the file entries.
};

struct INF_BIF_FILEENTRY
{
    quint32		dwLocator;
    quint32		dwOffset;
    quint32		dwSize;
    quint16		wType;
    char		chUnknown[2];
};

// I didn't add the tileset types. I don't currently have any interest in
// them.

struct INF_ITM
{
    char		chSignature[4];
    char		chVersion[4];
    quint32		dwGenericItemName;
    quint32		dwIdentifiedItemName;
    char		chUsedUpItem[8];
    quint32		dwTypeFlags;
    quint16		wCategory;
    quint32		dwUsability;
    char		chInvBAMResourceIndex[2];
    quint16		wMinLevel;
    quint16		wMinStrength;
    quint16		wMinStrengthBonus;
    quint16		wMinIntelligence;
    quint16		wMinDexterity;
    quint16		wMinWisdom;
    quint16		wMinConstitution;
    quint16		wMinCharisma;
    quint32		dwBaseValue;
    quint16		wMaxStackable;
    char		chItemIcon[8];
    quint16		wLore;
    char		chGroundIcon[8];
    quint32		dwBaseWeight;
    quint32		dwItemDescriptionGeneric;
    quint32		dwItemDescriptionIdentified;
    char		chCarrieddIcon[8];
    quint32		dwEnchantment;
    quint32		dwAbilityOffset;
    quint16		dwAbilityCount;
    quint32		dwEffectsOffset;
    quint16		wUnknown;
    quint16		wGlobalEffects;
};

struct INF_SPL
{
    char		chSignature[4];
    char		chVersion[4];
    quint32		dwGenericSpellName;
    quint32		dwIdentifiedSpellName;
    char		chCastSoundRef[8];
    quint32		dwUnknown;
    quint16		wCategory;
    quint32		dwUsability;
    quint16		wUnknown2;
    quint16		wMinLevel;
    quint16		wMinStrength;
    quint16		wMinStrengthBonus;
    quint16		wMinIntelligence;
    quint16		wMinDexterity;
    quint16		wMinWisdom;
    quint16		wMinConstitution;
    quint16		wMinCharisma;
    quint32		dwSpellLevel;
    quint16		wUnknown3;
    char		chSpellIcon[8];
    quint16		wUnknown4;
    char		chUnknown[8];
    quint32		dwUnknown3;
    quint32		dwSpellDescriptionGeneric;
    quint32		dwSpellDescriptionIdentified;
    char		chUnknown2[8];
    quint32		dwUnknown4;
    quint32		dwAbilityOffset;
    quint16		wAbilityCount;
    quint32		dwEffectsOffset;
    quint16		wUnknown5;
    quint16		wGlobalEffects;
};

#pragma pack(pop)

class CBifFileEntry
{
public:
    quint32		dwOffset;
    quint32		dwSize;
};

// helper typedefs
typedef QMap<quint16, CBifFileEntry *> CBifFileEntryMap;
typedef QMap<quint16, CBifFileEntryMap *> CInfBifFileResMap;

struct INF_CRE;
class CResInfo;
class CInfBifFile
{
public:
    CInfBifFile();
    ~CInfBifFile();

    // Opens the BIF file and stores its data
    bool Open(const QString &pszFilename, bool bAsOverride=false);

    void GetFilename(QString &strFilename) const { strFilename = m_strFilename; }

    // Retrieves the offset in the bif file and the size of the data for the passed
    // in resource. Returns true if successful finding the information.
    bool GetDataOffsetAndSize(const CResInfo *pResInfo, int &nOffset, int &nSize);

    CBifFileEntry *GetFileEntry(quint16 wResourceType, quint16 wLocator);

    // Given a resource type and a locator returns the data to pData.
    // When it finds the resource it returns the offset from the start of the
    // .bif file. This offset is used when the internal resource has offsets
    // that start relative to the start of this resource in the file. Returns -1
    // if it fails.
    // If this file is an override resource, the first two parameters are
    // ignored.
    int GetData(quint16 wResourceType, quint16 wLocator, void *pData, int nBufSize);
    int GetData(const CResInfo *pResInfo, void *pData, int nBufSize);

    // Does the same as above essentially.
    int GetData(quint16 wLocator, INF_ITM *pItem);
//	int GetData(quint16 wLocator, INF_BAM_HEADER *pBamHeader);
    int GetData(quint16 wLocator, INF_SPL *pSpell);
    int GetData(quint16 wLocator, INF_CRE *pCre);

    // Retrieves a BAM resource object.
    bool GetBam(const CResInfo *pResInfo, CInfBam &bam);

    bool IsValid()							{ return(m_bIsValid); }

    bool Seek(qint64 offset) {
        if (!m_file)
            return false;
        return m_file->seek(offset);
    }

    qint64 Read(char *data, qint64 maxlen) {
        if (!m_file)
            return false;
        return m_file->read(data, maxlen);
    }

private:
    INF_BIF_HEADER	m_bifHeader;

    // Used to keep track of the different resource types. The type is the key
    // to this map. Each entry in the map is a CBifFileEntryMap object, using the
    // resource locator as a key to the entry.
    CInfBifFileResMap m_mapResType;

    // Name of the open file.
    QString	m_strFilename;

    // Set to true when we are looking at an override file.
    bool	m_bAsOverride;

    // Set by the open function.
    bool	m_bIsValid;

    // Our file
    QFile   *m_file;
};

#endif // INFBIFFILE_H

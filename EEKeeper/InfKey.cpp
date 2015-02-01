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

#include <QApplication>
#include <QFile>


#include "defines.h"
#include "InfKey.h"
#include "EEKeeper.h"

quint8 _xorKey[XOR_KEY_LEN] =
{
    0x88,0xa8,0x8f,0xba,0x8a,0xd3,0xb9,0xf5,0xed,0xb1,0xcf,0xea,0xaa,0xe4,0xb5,0xfb,
    0xeb,0x82,0xf9,0x90,0xca,0xc9,0xb5,0xe7,0xdc,0x8e,0xb7,0xac,0xee,0xf7,0xe0,0xca,
    0x8e,0xea,0xca,0x80,0xce,0xc5,0xad,0xb7,0xc4,0xd0,0x84,0x93,0xd5,0xf0,0xeb,0xc8,
    0xb4,0x9d,0xcc,0xaf,0xa5,0x95,0xba,0x99,0x87,0xd2,0x9d,0xe3,0x91,0xba,0x90,0xca
};

CInfKey::CInfKey()
{
    m_pBifInfo = NULL;
}

CInfKey::~CInfKey()
{
    delete [] m_pBifInfo;

    OnIdleCleanup();

    CResInfoPlist *pResList;
    for (ResTypeMap::iterator it = m_mapResType.begin(); it != m_mapResType.end(); ++it)
    {
        // grab the next list, delete all its entries, then delete the list from memory
        pResList = it.value();
        qDeleteAll(pResList->begin(), pResList->end());
        delete pResList;
    }

    delete [] m_pBifFiles;
}

void CInfKey::OnIdleCleanup()
{
    qDeleteAll(m_listTempBif);
    m_listTempBif.clear();
}

bool CInfKey::Read(const QString &pszFilename, QProgressDialog *pCtrl)
{
    CResPtr res;
    CBifPtr bif;
    QFile file(pszFilename);

    qDebug() << "Reading chitin.key:" << pszFilename;
    qDebug() << "   Opening file.";
    if (!file.open(QIODevice::ReadOnly))
        return(false);

    qDebug() << "   Reading header";
    if (file.read(reinterpret_cast<char*>(&m_keyHeader), sizeof(INF_KEY_HEADER)) != sizeof(INF_KEY_HEADER))
        return(false);

    qDebug() << "   Allocating memory for file keys";
    bif.ptr = new INF_KEY_BIF[m_keyHeader.dwBifCount];
    if (!bif.ptr)
        return(false);

    if (!file.seek(m_keyHeader.dwBifOffset) )
        return(false);

    qDebug() << "   Reading keys.";
    if (file.read(reinterpret_cast<char*>(bif.ptr), sizeof(INF_KEY_BIF)*m_keyHeader.dwBifCount) != sizeof(INF_KEY_BIF)*m_keyHeader.dwBifCount)
        return(false);

    qDebug() << "   Allocating memory for resource keys.";
    res.ptr = new INF_KEY_RESOURCE[m_keyHeader.dwResourceCount];
    if (!res.ptr)
        return(false);

    if (!file.seek(m_keyHeader.dwResourceOffset) )
        return(false);

    qDebug() << "   Reading resource keys.";
    if (file.read(reinterpret_cast<char*>(res.ptr), sizeof(INF_KEY_RESOURCE)*m_keyHeader.dwResourceCount) != sizeof(INF_KEY_RESOURCE)*m_keyHeader.dwResourceCount)
        return(false);

    // Find all the filename for the bif entries.
    qDebug() << "   Allocating memory for bif information.";
    m_pBifInfo = new CBifInfo[m_keyHeader.dwBifCount];
    if (!m_pBifInfo)
        return(false);

    qDebug() << "   Allocating memory for bif files.";
    m_pBifFiles = new CInfBifFile[m_keyHeader.dwBifCount];
    if (!m_pBifFiles)
        return(false);

    qDebug() << "   Reading bif file information.";
    char szPath[1000];
    for (int i=0;i<(int)m_keyHeader.dwBifCount;i++)
    {
        if (!file.seek(bif.ptr[i].dwFilenameOffset) )
            return(false);

        if (file.read(reinterpret_cast<char*>(szPath), bif.ptr[i].wFilenameLength) != bif.ptr[i].wFilenameLength)
            return(false);

        // This is a fix for a Mac version. For some reason the filenames are listed with
        // a colon as the first character instead of a backslash.
        if (szPath[0] == ':')
            szPath[0] = '/';

        qDebug() << "     " << szPath;
        m_pBifInfo[i].strFilename = QString(szPath);
        m_pBifInfo[i].wLocation = bif.ptr[i].wFileLocation;
    }

    if (pCtrl)
    {
        // I'm multiplying the number of bif files by a larger number because there
        // are so many resources. If each file doesn't represent a large chunk the
        // progress control won't be able to show anything.
        pCtrl->setRange(0, m_keyHeader.dwResourceCount+m_keyHeader.dwBifCount*FILE_PROG_MULT);
        pCtrl->setValue(0);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    qDebug() << "   Indexing resources.";
    // I'm storing the names combined with the resource type to make a unique key.
    CResInfo        *pResInfo;
    CResInfoPlist   *pResList;
    ResTypeMap::iterator itResMap;
    char szName[9];
    for (int i=0; i < (int)m_keyHeader.dwResourceCount; i++)
    {
        // I'm not using all the resources types. This is an attempt to speed it up
        // a little bit. The map classes take a little while to add all the items.
        switch(res.ptr[i].wType)
        {
            case RESTYPE_ITM :
            case RESTYPE_BAM :
            case RESTYPE_BMP :
            case RESTYPE_SPL :
            case RESTYPE_CRE :
            case RESTYPE_IDS :
            case RESTYPE_2DA :
            case RESTYPE_BS :
                break;

            default:
                continue;
        }

        // Find the resource.
        itResMap = m_mapResType.find(res.ptr[i].wType);
        if (itResMap == m_mapResType.end())
        {
            qDebug() << "      Allocating new map for resource type:" << QString::number(res.ptr[i].wType, 16);

            // If this type of resource isn't known yet, add one to the resource
            // type map.
            pResList = new CResInfoPlist;
            if (!pResList)
                return(false);
            qDebug() << "      New map allocated.";
            m_mapResType.insert(res.ptr[i].wType, pResList);
        } else {
            pResList = itResMap.value();
        }

        memcpy(szName, res.ptr[i].chName, 8);
        szName[8] = '\x0';

        pResInfo = new CResInfo;
        if (!pResInfo)
            return(false);

        pResInfo->strName = szName;
        pResInfo->wBifKeyIndex = (quint16)((res.ptr[i].dwLocator & 0xFFF00000) >> 20);
        pResInfo->wLocator  = (quint16)(res.ptr[i].dwLocator & 0x00003FFF);
        pResInfo->chTileIndex = (quint8)((res.ptr[i].dwLocator & 0x000FC000) >> 14);
        pResInfo->chFlags = 0;
        pResInfo->wResourceType = res.ptr[i].wType;

        pResList->append(pResInfo);

        if (pCtrl)
        {
            pCtrl->setValue(i+1);
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }

    qDebug() << "   Opening BIF files.";
    // Open up and index all the BIF files. I was doing this on an as needed
    // basis but it started being a pain since the files I tended to use were
    // used multiple times. Each time a bif file is opened I index all the
    // resources in the file -- it was starting to add up when doing items.
    QString strBifFile;
    for (int i=0;i<(int)m_keyHeader.dwBifCount;i++)
    {
        if (pCtrl)
        {
            pCtrl->setValue(pCtrl->value() + FILE_PROG_MULT);
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        strBifFile = _strInstallPath + m_pBifInfo[i].strFilename;
        if (!m_pBifFiles[i].Open(strBifFile)) {
            qDebug() << "       Failed to read:" << strBifFile;
            continue;
        }
    }

    RefreshOverride();

    return(true);
}

bool CInfKey::GetResList(quint16 wResourceType, CResInfoPlist *&rList)
{
    ResTypeMap::iterator it = m_mapResType.find(wResourceType);
    if (it != m_mapResType.end() )
    {
        rList = it.value();
        return true;
    }

    return false;
}

CResInfo* CInfKey::GetResInfo(quint16 wResourceType, const QString &pszResName)
{
    CResInfoPlist *pList = NULL;
    if (!GetResList(wResourceType, pList) )
        return(NULL);

    CResInfo *pResInfo;
    for (CResInfoPlist::iterator it = pList->begin(); it != pList->end(); ++it)
    {
        pResInfo = (*it);
        if (pResInfo->strName.compare(pszResName, Qt::CaseInsensitive) == 0 )
            return(pResInfo);
    }

    return(NULL);
}

bool CInfKey::GetBifFile(quint32 nBifKeyIndex, QString &strFilename)
{
    if ((qint32)nBifKeyIndex < 0 || nBifKeyIndex >= m_keyHeader.dwBifCount)
        return(false);

    strFilename = m_pBifInfo[nBifKeyIndex].strFilename;
    return(true);
}

CInfBifFile* CInfKey::GetBifFile(quint32 nBifKeyIndex)
{
    if ((qint32)nBifKeyIndex < 0 || nBifKeyIndex >= m_keyHeader.dwBifCount)
        return(NULL);

    return(&m_pBifFiles[nBifKeyIndex]);
}

bool CInfKey::GetBifFile(const CResInfo *pInfo, QString &strFilename)
{
    if (pInfo->IsFlagSet(RIF_USEOVERRIDE))
    {
        // Need to find the extension for this resource type.
        int nIndex = 0;
        while(_keyResOver[nIndex].szExt[0])
        {
            if (_keyResOver[nIndex].wResType == pInfo->wResourceType)
                break;
            nIndex++;
        }

        if (!_keyResOver[nIndex].szExt[0])
            return(false);

        strFilename = _strInstallPath + QString(DIR_OVERRIDE) + QDir::separator() + pInfo->strName +
                QString(".") + QString(_keyResOver[nIndex].szExt);

        return(true);
    }

    if (pInfo->wBifKeyIndex < 0 || pInfo->wBifKeyIndex >= m_keyHeader.dwBifCount)
        return(false);

    strFilename = m_pBifInfo[pInfo->wBifKeyIndex].strFilename;
    return(true);
}

CInfBifFile* CInfKey::GetBifFile(const CResInfo *pInfo, bool bIgnoreOverride)
{
    // If it isn't an overriden resource, get the actual bif file.
    if (!pInfo->IsFlagSet(RIF_USEOVERRIDE) || bIgnoreOverride)
        return(GetBifFile(pInfo->wBifKeyIndex));

    // Check and see if we already have it open in the temp list.
    CTempBifFile *pTempFile;
    for (CTempBifFilePlist::iterator it = m_listTempBif.begin(); it != m_listTempBif.end(); ++it)
    {
        pTempFile = (*it);
        if (pTempFile->wResType == pInfo->wResourceType &&
                pTempFile->strResName.compare(pInfo->strName, Qt::CaseInsensitive) == 0)
            return(&pTempFile->infFile);
    }

    // Need to find the extension for this resource type.
    int nIndex = 0;
    while(_keyResOver[nIndex].szExt[0])
    {
        if (_keyResOver[nIndex].wResType == pInfo->wResourceType)
            break;
        nIndex++;
    }

    if (!_keyResOver[nIndex].szExt[0])
        return(NULL);

    // Didn't find one, so need to create a new object and open the
    // file.
    pTempFile = new CTempBifFile;

    QString strFilename  = _strInstallPath + QString(DIR_OVERRIDE) + QDir::separator() + pInfo->strName +
            QString(".") + QString(_keyResOver[nIndex].szExt);

    if (!pTempFile->infFile.Open(strFilename, true))
    {
        delete pTempFile;
        return(NULL);
    }

    pTempFile->wResType = pInfo->wResourceType;
    pTempFile->strResName = pInfo->strName;

    m_listTempBif.append(pTempFile);

    return(&pTempFile->infFile);
}

bool CInfKey::GetResData(quint16 wResourceType, const QString &pszResName, quint8 *&pBuf, int &nLen, bool bAddNull)
{
    CResInfo *pResInfo = GetResInfo(wResourceType, pszResName);
    if (!pResInfo)
        return(false);

    CInfBifFile *pBifFile = GetBifFile(pResInfo);
    if (!pBifFile || !pBifFile->IsValid())
        return(false);

    int nDataOffset;
    if (!pBifFile->GetDataOffsetAndSize(pResInfo, nDataOffset, nLen))
        return(false);

    pBuf = new quint8[(bAddNull ? nLen+1 : nLen)];
    if (!pBuf)
        return(false);

    if (pBifFile->GetData(pResInfo,pBuf,nLen) == -1)
    {
        delete [] pBuf;
        return(false);
    }

    if (bAddNull)
        pBuf[nLen] = '\x0';

    return(true);
}

void CInfKey::RefreshOverride()
{
    qDebug() << "   Scanning override folder for additional resources:";
    QString strOverridePath = _strInstallPath + QString(DIR_OVERRIDE) + QDir::separator();

    // open our directory
    QDir dir(strOverridePath);
    if (!dir.exists())
    {
        qDebug() << "            Unable to open directory for RefreshOverride(): " << strOverridePath;
        return;
    }

    // Check the override folder for additional resources. I'm not including
    // this in the progress control, it should be really fast to just check
    // the existence of the files.
    QString strFileFilter;
    QString strFile;
    QStringList strFileList;
    CResInfo *pResInfo;
    CResInfoPlist *pResList;
    int nIndex;
    for (int i=0; _keyResOver[i].szExt[0]; ++i)
    {
        qDebug() << "      Resource extension" << _keyResOver[i].szExt;

        // get all the files with the extention filter
        strFileFilter = QString("*.") + QString(_keyResOver[i].szExt);
        strFileList = dir.entryList(QStringList(strFileFilter), QDir::Files | QDir::Readable);

        for (QStringList::iterator it = strFileList.begin(); it != strFileList.end(); ++it)
        {
            strFile = (*it);
            qDebug() << "        " << strFile;

            // Pull off the name up to the extension. That will be the resource ID.
            nIndex = strFile.lastIndexOf(QString("."));
            if (nIndex != -1)
                strFile = strFile.left(nIndex);

            // See if we already have this resource information loaded.
            qDebug() << "            Looking for existing resource.";
            pResInfo = GetResInfo(_keyResOver[i].wResType, strFile);
            if (pResInfo)
            {
                // We already have it, just mark it to use overriden version.
                pResInfo->SetFlag(RIF_USEOVERRIDE);
                qDebug() << "               Resources exists, using overridden version.";
            }
            else
            {
                // Don't have it. Must be a new item. So need to make a CResInfo object for it.
                if (GetResList(_keyResOver[i].wResType, pResList))
                {
                    qDebug() << "            Allocating memory for new resource.";
                    pResInfo = new CResInfo;
                    if (!pResInfo)
                    {
                        qDebug() << "         Failed to allocate memory for new resrouce.";
                        break;
                    }

                    // The only relevant members of the object are the name and that it is an
                    // overriden item. The rest of the members are used for looking up in a BIF
                    // file.
                    pResInfo->strName = strFile;
                    pResInfo->SetFlag(RIF_USEOVERRIDE|RIF_CUSTOM);
                    pResInfo->wResourceType = _keyResOver[i].wResType;

                    pResList->append(pResInfo);
                }
            }
        }
    }


    qDebug() << "   Rechecking CRE overrides.";

    // When refreshing, it's also possible that some of the Overridden
    // resources have been deleted. Need to remove the override flag on those.
    // For the momement, CRE are the only ones I care about for this.
    // I don't want to delete any ResInfo objects at the point. It is possible
    // that a custom resource was deleted and should be removed from the list,
    // but I don't want to cause problems if anything has a pointer to the
    // resource at the time this is run.
    CResInfoPlist *m_pCreatures;
    QString	strCreFile;
    if (_infKey.GetResList(RESTYPE_CRE, m_pCreatures) )
    {
        for (CResInfoPlist::iterator it = m_pCreatures->begin(); it != m_pCreatures->end(); ++it)
        {
            pResInfo = (*it);
            if (!pResInfo)
                continue;

            if (pResInfo->IsFlagSet(RIF_USEOVERRIDE))
            {
                strCreFile = strOverridePath + pResInfo->strName + ".cre";
                if (!QFile::exists(strCreFile))
                    pResInfo->RemoveFlag(RIF_USEOVERRIDE);
            }
        }
    }

    qDebug() << "   Done with override refresh.\n";
}

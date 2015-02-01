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
#include "InfBifFile.h"
#include "InfCreature.h"
#include "InfKey.h"
#include "EEKeeper.h"
#include "Log.h"


CInfBifFile::CInfBifFile() : m_file(NULL)
{
    m_bAsOverride = false;
    m_bIsValid = false;
}

CInfBifFile::~CInfBifFile()
{
    // if we have an open file, close it
    if (m_file) {
        m_file->close();
        delete m_file;
        m_file = NULL;
    }

    // clear the memory of each bif entry
    for (CInfBifFileResMap::iterator it = m_mapResType.begin(); it != m_mapResType.end(); ++it) {
        CBifFileEntryMap *pBifEntries = it.value();
        qDeleteAll(pBifEntries->begin(), pBifEntries->end());
        delete pBifEntries;
        pBifEntries = NULL;
    }
}

bool CInfBifFile::Open(const QString &pszFilename, bool bAsOverride)
{
    // make sure we close an open file
    if (m_file) {
        m_file->close();
        delete m_file;
        m_file = NULL;
    }

    m_file = new QFile(pszFilename);

    if (!m_file->open(QIODevice::ReadOnly))
    {
        qDebug() << "         Failed to open the file:" << pszFilename;
        delete m_file;
        m_file = NULL;
        return(false);
    }

    m_bAsOverride = bAsOverride;

    // Only need to read in the catalog if it is a real bif file.
    if (!m_bAsOverride)
    {
        if (m_file->read(reinterpret_cast<char*>(&m_bifHeader), sizeof(INF_BIF_HEADER) ) != sizeof(INF_BIF_HEADER)) {
            m_file->close();
            delete m_file;
            m_file = NULL;
            return(false);
        }

        // Make sure it is a BIFF format I'm prepared to deal with. "BIFC" pops up a bit.
        if (memcmp(m_bifHeader.chSignature, "BIFF", 4) ) {
            m_file->close();
            delete m_file;
            m_file = NULL;
            return(false);
        }

        if (!_bIgnoreDataVersions && memcmp(m_bifHeader.chVersion, "V1  ", 4)) {
            m_file->close();
            delete m_file;
            m_file = NULL;
            return(false);
        }

        qDebug() << "         Seeking to file entries.";
        if (!m_file->seek(m_bifHeader.dwFileEntryOffset)) {
            m_file->close();
            delete m_file;
            m_file = NULL;
            return(false);
        }

        INF_BIF_FILEENTRY bifFileEntry;
        CBifFileEntryMap *pBifEntries;
        CBifFileEntry *pFileEntry;
        CInfBifFileResMap::iterator itBifEntries;

        qDebug() << "         Number of file entries:" << m_bifHeader.dwFileEntryCount;
        for (int i=0; i < (int)m_bifHeader.dwFileEntryCount; i++)
        {
            if (m_file->read(reinterpret_cast<char*>(&bifFileEntry), sizeof(INF_BIF_FILEENTRY) ) != sizeof(INF_BIF_FILEENTRY) )
            {
                qDebug() << "         Failed to read bif file entry.";
                m_file->close();
                delete m_file;
                m_file = NULL;
                return(false);
            }

            // Grab the resource pBifEntries if in the list
            // If not in the list, add it
            itBifEntries = m_mapResType.find(bifFileEntry.wType);
            if (itBifEntries == m_mapResType.end())
            {
                pBifEntries = new CBifFileEntryMap;
                if (!pBifEntries)
                {
                    qDebug() << "         Failed to allocate memory for resource map.";
                    m_file->close();
                    delete m_file;
                    m_file = NULL;
                    return(false);
                }

                m_mapResType.insert(bifFileEntry.wType, pBifEntries);
            } else {
                pBifEntries = itBifEntries.value();
            }

            pFileEntry = new CBifFileEntry;
            if (!pFileEntry)
            {
                qDebug() << "         Failed to allocate memory for file entry information.";
                m_file->close();
                delete m_file;
                m_file = NULL;
                return(false);
            }

            pFileEntry->dwOffset = bifFileEntry.dwOffset;
            pFileEntry->dwSize = bifFileEntry.dwSize;

            pBifEntries->insert((quint16)bifFileEntry.dwLocator, pFileEntry);
        }
    }

    qDebug() << "         Finished reading file entries.";

    m_strFilename = pszFilename;
    m_bIsValid = true;

    return(true);
}

bool CInfBifFile::GetDataOffsetAndSize(const CResInfo *pResInfo, int &nOffset, int &nSize)
{
    // If this is an override the offset is the beginning of the file and the size
    // is the size of the file.
    if (m_bAsOverride)
    {
        // we need an open file to override
        if (!m_file || !m_file->isReadable())
            return false;

        nOffset = 0;
        nSize = (int)m_file->size();
        return(true);
    }

    // get the file entry
    CBifFileEntry *pFileEntry = GetFileEntry(pResInfo->wResourceType, pResInfo->wLocator);
    if (pFileEntry == NULL)
        return(false);

    nSize = pFileEntry->dwSize;
    nOffset = pFileEntry->dwOffset;

    return(true);
}

CBifFileEntry *CInfBifFile::GetFileEntry(quint16 wResourceType, quint16 wLocator) {
    // grab the bif entry from the res map
    CInfBifFileResMap::iterator itBifEntries = m_mapResType.find(wResourceType);
    if (itBifEntries == m_mapResType.end()) {
        return(NULL);
    }

    // grab the file entry from the bif map
    CBifFileEntryMap *pBifEntries = itBifEntries.value();
    CBifFileEntryMap::iterator itFileEntry = pBifEntries->find(wLocator);
    if (itFileEntry == (pBifEntries->end())) {
        return(NULL);
    }

    // fill in the size and offset data
    return itFileEntry.value();
}

int CInfBifFile::GetData(const CResInfo *pResInfo, void *pData, int nBufSize)
{
    return(GetData(pResInfo->wResourceType, pResInfo->wLocator, pData, nBufSize));
}

int CInfBifFile::GetData(quint16 wResourceType, quint16 wLocator, void *pData, int nBufSize)
{
    if (!m_file || !m_file->isReadable())
        return(-1);

    CBifFileEntry *pFileEntry = NULL;
    qint64 dwBytesToRead;

    if (!m_bAsOverride)
    {
        // get the file entry
        if ((pFileEntry = GetFileEntry(wResourceType, wLocator) ) == NULL ) {
            return(-1);
        }

        if (!m_file->seek(pFileEntry->dwOffset)) {
            return(-1);
        }

        dwBytesToRead = (qint64)qMin((quint32)nBufSize, pFileEntry->dwSize);
    }
    else
    {
        if (!m_file->seek(0))
            return(-1);

        dwBytesToRead = nBufSize;
    }

    if (m_file->read(reinterpret_cast<char*>(pData), dwBytesToRead) != dwBytesToRead) {
        return(-1);
    }

    return((m_bAsOverride ? 0 : pFileEntry->dwOffset));
}

int CInfBifFile::GetData(quint16 wLocator, INF_ITM *pItem)
{
    return(GetData(RESTYPE_ITM, wLocator, pItem, sizeof(INF_ITM)));
}

int CInfBifFile::GetData(quint16 wLocator, INF_SPL *pSpell)
{
    return(GetData(RESTYPE_SPL, wLocator, pSpell, sizeof(INF_SPL)));
}

int CInfBifFile::GetData(quint16 wLocator, INF_CRE *pCre)
{
    return(GetData(RESTYPE_CRE, wLocator, pCre, sizeof(INF_CRE)));
}

/*int CInfBifFile::GetData(quint16 wLocator, INF_BAM_HEADER *pBamHeader)
{
    return(GetData(RESTYPE_BAM,wLocator,pBamHeader,sizeof(INF_BAM_HEADER)));
} */

bool CInfBifFile::GetBam(const CResInfo *pResInfo, CInfBam &bam)
{
    if (!m_file || !m_file->isOpen())
        return(false);

    if (pResInfo->wResourceType != RESTYPE_BAM)
        return(false);

    int nDataOffset, nDataSize;
    if (!GetDataOffsetAndSize(pResInfo, nDataOffset, nDataSize))
        return(false);

    uchar *pData = new uchar[nDataSize];
    if (!pData)
        return(false);

    if (!m_file->seek(nDataOffset) || m_file->read(reinterpret_cast<char*>(pData), nDataSize) != nDataSize) {
        delete[] pData;
        return(false);
    }

    // If this is an uncompressed bam we can just attach the buffer to the
    // bam file object and be done with it.
    if (!memcmp(pData,"BAM ",4))
    {
        bam.SetBuffer(pData, nDataSize);
        return(true);
    }

    int nExpandSize = *((int*)(pData+8));

    // try 12 if it doesn't work - qUncompress should need the 4-byte header though
    QByteArray expand = qUncompress(pData+8, nDataSize-8);
    if (expand.isEmpty())
    {
        delete [] pData;
        qDebug() << "qUncompress failed:" << pResInfo->strName;
        return (false);
    }

    // done with the data
    delete [] pData;

    // warn if there is a difference between the expected and actual size
    if (expand.length() != nExpandSize) {
        qDebug() << "qUncompress unexpected size:" << expand.length() << "vs" << nExpandSize;
        nExpandSize = expand.length();
    }

    // copy the expanded data over and set the buffer
    uchar *pExpand = new uchar[nExpandSize];
    memcpy(pExpand, expand.constData(), nExpandSize);
    bam.SetBuffer(pExpand, nExpandSize);

    return(true);
}

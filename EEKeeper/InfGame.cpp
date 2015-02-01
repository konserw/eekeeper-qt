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


#include "InfGame.h"
#include "defines.h"
#include "EEKeeper.h"
#include <QMessageBox>

CInfGame::CInfGame()
{
    m_pinfOutCharInfo = NULL;
    m_pinfOutParty = NULL;
    m_bHasChanged = false;
    m_nError = ERR_NONE;
    m_pJournal = NULL;
    m_nJournalSize = 0;
    m_nAfterJournalSize = 0;
    m_pAfterJournal = NULL;
}

CInfGame::~CInfGame()
{
    delete [] m_pinfOutCharInfo;
    delete [] m_pinfOutParty;
    delete [] m_pJournal;
    delete [] m_pAfterJournal;

    qDeleteAll(m_plGlobals);
}

bool CInfGame::Read(const QString &pszFilename)
{
    QFile file(pszFilename);
    qint64 nCount;
    int i;

    if (!file.open(QIODevice::ReadOnly))
    {
        m_nError = ERR_GAME_FAILEDOPEN;
        return(false);
    }
    m_strFilename = pszFilename;

    nCount = file.read(reinterpret_cast<char*>(&m_infGame), sizeof(INF_GAME));
    if (nCount != sizeof(INF_GAME))
    {
        m_nError = ERR_GAME_BADHEADER;
        file.close();
        return(false);
    }

    if (memcmp(m_infGame.chSignature,"GAME",4))
    {
        m_nError = ERR_GAME_MISSINGSIG;
        file.close();
        return(false);
    }

    if (!_bIgnoreDataVersions && memcmp(m_infGame.chVersion,"V2.0",4) && memcmp(m_infGame.chVersion,"V2.1",4))
    {
        m_nError = ERR_GAME_BADVERSION;
        file.close();
        return(false);
    }

    // After each section is read I'm checking the file position to be sure it follows
    // with how I expect the file to be built.
    // Character Info for in-party characters should follow the header.
    if (file.pos() != m_infGame.dwInPartyCharOffset)
    {
        m_nError = ERR_GAME_BADPARTYCHARINFO;
        file.close();
        return(false);
    }

    // I fixed it at a maximum of 6 for the editor.
    if (m_infGame.dwInPartyCharCount > INF_MAX_CHARACTERS)
    {
        m_nError = ERR_GAME_TOOMANYINPARTY;
        file.close();
        return(false);
    }

    // Read in all the INF_CHARINFO records.
    nCount = file.read(reinterpret_cast<char*>(&m_infCharInfo[0]), sizeof(INF_GAME_CHARINFO)*m_infGame.dwInPartyCharCount);
    if (nCount != sizeof(INF_GAME_CHARINFO)*m_infGame.dwInPartyCharCount)
    {
        m_nError = ERR_GAME_READPARTYCHARINFO;
        file.close();
        return(false);
    }

    for (i=0;i<(int)m_infGame.dwInPartyCharCount;i++)
    {
        if (!m_infParty[i].Read(file))
        {
            m_nError = m_infParty[i].GetLastError();
            file.close();
            return(false);
        }

        // add the CharInfo to this character
        m_infParty[i].SetCharInfo(&m_infCharInfo[i]);
    }

    // At this point the file should be pointing at the start of the creatures not in
    // the party data.
    if (file.pos() != m_infGame.dwOutPartyCharOffset)
    {
        m_nError = ERR_GAME_BADOUTPARTYCHARINFO;
        file.close();
        return(false);
    }

    if (m_infGame.dwOutPartyCharCount)
    {
        m_pinfOutCharInfo = new INF_GAME_CHARINFO[m_infGame.dwOutPartyCharCount];
        if (!m_pinfOutCharInfo)
        {
            m_nError = ERR_GAME_OOM_OUTPARTYCHARINFO;
            file.close();
            return(false);
        }

        nCount = file.read(reinterpret_cast<char*>(&m_pinfOutCharInfo[0]),sizeof(INF_GAME_CHARINFO)*m_infGame.dwOutPartyCharCount);
        if (nCount != sizeof(INF_GAME_CHARINFO)*m_infGame.dwOutPartyCharCount)
        {
            m_nError = ERR_GAME_READOUTPARTYCHARINFO;
            file.close();
            return(false);
        }

        m_pinfOutParty = new CInfCreature[m_infGame.dwOutPartyCharCount];
        if (!m_pinfOutParty)
        {
            m_nError = ERR_GAME_OOM_OUTPARTYCRE;
            file.close();
            return(false);
        }

        for (i=0;i<(int)m_infGame.dwOutPartyCharCount;i++)
        {
            if (!m_pinfOutParty[i].Read(file))
            {
                m_nError = m_pinfOutParty[i].GetLastError();
                file.close();
                return(false);
            }
        }
    }

    // Should be looking at the variables now.
    if (file.pos() != m_infGame.dwGlobalVarOffset)
    {
        m_nError = ERR_GAME_BADVARS;
        file.close();
        return(false);
    }

    INF_GAME_GLOBAL *pGlobal;
    for (i=0;i<(int)m_infGame.dwGlobalVarCount;i++)
    {
        pGlobal = new INF_GAME_GLOBAL;
        if (!pGlobal)
        {
            m_nError = ERR_GAME_OOM_VARS;
            file.close();
            return(false);
        }

        nCount = file.read(reinterpret_cast<char*>(pGlobal),sizeof(INF_GAME_GLOBAL));
        if (nCount != sizeof(INF_GAME_GLOBAL))
        {
            m_nError = ERR_GAME_READVARS;
            file.close();
            return(false);
        }

        m_plGlobals.append(pGlobal);
    }

    // Journal entries should follow the variables.
    if (file.pos() != m_infGame.dwJournalOffset)
    {
        m_nError = ERR_GAME_BADJOURNAL;
        file.close();
        return(false);
    }

    qint64 nFileSize = file.size();
    qint64 nPos = file.pos();

    // I'm not sure if this data is always here or not, but it will affect the size
    // of the journal data.
    if (m_infGame.dwAfterJournalOffset)
        m_nJournalSize = m_infGame.dwAfterJournalOffset - m_infGame.dwJournalOffset;
    else
        m_nJournalSize = nFileSize - nPos;

    m_pJournal = new quint8[m_nJournalSize];
    if (!m_pJournal)
    {
        m_nError = ERR_GAME_OOM_JOURNAL;
        file.close();
        return(false);
    }

    nCount = file.read(reinterpret_cast<char*>(m_pJournal), m_nJournalSize);
    if (nCount != (quint32)m_nJournalSize)
    {
        m_nError = ERR_GAME_READJOURNAL;
        file.close();
        return(false);
    }

    // Read in that possible extra data after the journal entries.
    if (m_infGame.dwAfterJournalOffset)
    {
        nPos = file.pos();
        m_nAfterJournalSize = nFileSize - nPos;
        m_pAfterJournal = new quint8[m_nAfterJournalSize];
        if (!m_pAfterJournal)
        {
            m_nError = ERR_GAME_OOM_AFTERJOURNAL;
            file.close();
            return(false);
        }

        nCount = file.read(reinterpret_cast<char*>(m_pAfterJournal), m_nAfterJournalSize);
        if (nCount != (quint32)m_nAfterJournalSize)
        {
            m_nError = ERR_GAME_READAFTERJOURNAL;
            file.close();
            return(false);
        }
    }

    file.close();
    m_bHasChanged = false;
    return(true);
}

bool CInfGame::Write(const QString &pszFilename)
{
    QFile file(pszFilename);
    quint32	nOffset;
    int i;
    quint64 nCount;

    nOffset = sizeof(INF_GAME);
    for (i=0;i<(int)m_infGame.dwInPartyCharCount;i++)
    {
        if (!i)
            m_infCharInfo[i].dwCREOffset = sizeof(INF_GAME) + m_infGame.dwInPartyCharCount*sizeof(INF_GAME_CHARINFO);
        else
            m_infCharInfo[i].dwCREOffset = m_infCharInfo[i-1].dwCREOffset + m_infCharInfo[i-1].dwCRESize;
        m_infCharInfo[i].dwCRESize = m_infParty[i].GetFileSpace();

        nOffset += sizeof(INF_GAME_CHARINFO);
        nOffset += m_infParty[i].GetFileSpace();
    }
    m_infGame.dwOutPartyCharOffset = nOffset;

    for (i=0;i<(int)m_infGame.dwOutPartyCharCount;i++)
    {
        if (!i)
            m_pinfOutCharInfo[i].dwCREOffset = m_infGame.dwOutPartyCharOffset + m_infGame.dwOutPartyCharCount*sizeof(INF_GAME_CHARINFO);
        else
            m_pinfOutCharInfo[i].dwCREOffset = m_pinfOutCharInfo[i-1].dwCREOffset + m_pinfOutCharInfo[i-1].dwCRESize;
        m_pinfOutCharInfo[i].dwCRESize = m_pinfOutParty[i].GetFileSpace();

        nOffset += sizeof(INF_GAME_CHARINFO);
        nOffset += m_pinfOutParty[i].GetFileSpace();
    }
    m_infGame.dwGlobalVarOffset = nOffset;

    nOffset += m_infGame.dwGlobalVarCount * sizeof(INF_GAME_GLOBAL);
    m_infGame.dwJournalOffset = nOffset;

    if (m_infGame.dwAfterJournalOffset)
    {
        nOffset += m_nJournalSize;
        m_infGame.dwAfterJournalOffset = nOffset;
    }

    if (!file.open(QIODevice::WriteOnly))
    {
        m_nError = ERR_GAME_FAILEDCREATE;
        return(false);
    }

    if (file.write(reinterpret_cast<const char*>(&m_infGame), sizeof(INF_GAME)) != sizeof(INF_GAME))
    {
        m_nError = ERR_GAME_WRITEHEADER;
        file.close();
        return(false);
    }

    // write the party char info to file
    nCount = file.write(reinterpret_cast<const char*>(m_infCharInfo), sizeof(INF_GAME_CHARINFO)*m_infGame.dwInPartyCharCount);
    if (nCount != m_infGame.dwInPartyCharCount * sizeof(INF_GAME_CHARINFO))
    {
            m_nError = ERR_GAME_WRITEPARTYCHARINFO;
            file.close();
            return(false);
    }

    for (i=0;i<(int)m_infGame.dwInPartyCharCount;i++)
        if (!m_infParty[i].Write(file))
        {
            m_nError = m_infParty[i].GetLastError();
            return(false);
        }

    if (m_infGame.dwOutPartyCharCount > 0)
    {
        nCount = file.write(reinterpret_cast<const char*>(m_pinfOutCharInfo), m_infGame.dwOutPartyCharCount*sizeof(INF_GAME_CHARINFO));
        if (nCount != m_infGame.dwOutPartyCharCount * sizeof(INF_GAME_CHARINFO))
        {
            m_nError = ERR_GAME_WRITEOUTPARTYCHARINFO;
            file.close();
            return(false);
        }

        for (i=0;i<(int)m_infGame.dwOutPartyCharCount;i++)
        {
            if (!m_pinfOutParty[i].Write(file))
            {
                m_nError = m_pinfOutParty[i].GetLastError();
                return(false);
            }
        }
    }

    if (m_infGame.dwGlobalVarCount)
    {
        for (INF_GAME_GLOBAL_PLIST::iterator it = m_plGlobals.begin(); it != m_plGlobals.end(); ++it)
        {
            if (file.write(reinterpret_cast<const char*>(*it), sizeof(INF_GAME_GLOBAL)) != sizeof(INF_GAME_GLOBAL) )
            {
                m_nError = ERR_GAME_WRITEVARIABLES;
                file.close();
                return(false);
            }
        }
    }

    if (m_nJournalSize)
    {
        if (file.write(reinterpret_cast<const char*>(m_pJournal), m_nJournalSize) != m_nJournalSize)
        {
            m_nError = ERR_GAME_WRITEJOURNAL;
            file.close();
            return(false);
        }
    }

    if (m_nAfterJournalSize)
    {
        if (file.write(reinterpret_cast<const char*>(m_pAfterJournal),m_nAfterJournalSize) != m_nAfterJournalSize)
        {
            m_nError = ERR_GAME_WRITEAFTERJOURNAL;
            file.close();
            return(false);
        }
    }

    // close the file
    file.close();

    m_bHasChanged = false;
    return(true);
}

bool CInfGame::HasChanged()
{
    if (m_bHasChanged)
        return(true);

    // check every in/out party character for changes
    int i;
    for (i=0;i<(int)m_infGame.dwInPartyCharCount;i++)
        if (m_infParty[i].HasChanged())
            return(true);

    for (i=0;i<(int)m_infGame.dwOutPartyCharCount;i++)
        if (m_pinfOutParty[i].HasChanged())
            return(true);

    return(false);
}

void CInfGame::SetPartyGold(quint32 dwGold)
{
    if (m_infGame.dwGold != dwGold)
        m_bHasChanged = true;
    m_infGame.dwGold = dwGold;
}

void CInfGame::GetPartyCharName(int nChar, QString &strName)
{
    if (m_infParty[nChar].GetLongNameID() == 0xFFFFFFFF)
    {
        if (m_infCharInfo[nChar].szName[0])
            strName = QString(m_infCharInfo[nChar].szName);
        else
            m_infParty[nChar].GetName(strName);
    }
    else
        m_infParty[nChar].GetName(strName);
}

void CInfGame::SetPartyCharName(int nChar, const char *pszName)
{
    if (m_infCharInfo[nChar].szName[0])
    {
        if (memcmp(m_infCharInfo[nChar].szName, pszName, 20) != 0)
            m_bHasChanged = true;
        qstrncpy(m_infCharInfo[nChar].szName, pszName, 20);
    }
    else
        m_infParty[nChar].SetName(pszName);
}

void CInfGame::GetOutOfPartyCharName(int nChar, QString &strName)
{
    if (m_pinfOutParty[nChar].GetLongNameID() == 0xFFFFFFFF)
    {
        if (m_pinfOutCharInfo[nChar].szName[0])
            strName = m_pinfOutCharInfo[nChar].szName;
        else
            m_pinfOutParty[nChar].GetName(strName);
    }
    else
        m_pinfOutParty[nChar].GetName(strName);
}

void CInfGame::SetOutOfPartyCharName(int nChar, const char *pszName)
{
    if (m_pinfOutCharInfo[nChar].szName[0])
    {
        if (memcmp(m_pinfOutCharInfo[nChar].szName, pszName, 20))
            m_bHasChanged = true;
        qstrncpy(m_pinfOutCharInfo[nChar].szName, pszName, 20);
    }
    else
        m_pinfOutParty[nChar].SetName(pszName);
}

void CInfGame::SetPartyReputation(quint8 chRep)
{
    chRep = VALIDATE_RANGE(chRep,0,20);
    if (m_infGame.chPartyReputation != chRep*10)
        m_bHasChanged = true;
    m_infGame.chPartyReputation = chRep*10;

    for (int i=0;i<(int)m_infGame.dwInPartyCharCount;i++)
        m_infParty[i].SetReputation(chRep);
}

bool CInfGame::GetGlobals(INF_GAME_GLOBAL_PLIST &list)
{
    if (m_plGlobals.count() < 1)
        return(true);

    INF_GAME_GLOBAL *pNew;
    for (INF_GAME_GLOBAL_PLIST::iterator it = m_plGlobals.begin(); it != m_plGlobals.end(); ++it)
    {
        pNew = new INF_GAME_GLOBAL;
        if (!pNew)
            return(false);

        memcpy(pNew, (*it), sizeof(INF_GAME_GLOBAL));

        list.append(pNew);
    }

    return(true);
}

void CInfGame::SetGlobals(INF_GAME_GLOBAL_PLIST &list)
{
    // clear our current list
    qDeleteAll(m_plGlobals);
    m_plGlobals.clear();

    // copy in all the items from the new list using implicit sharing
    m_plGlobals = list;
    m_infGame.dwGlobalVarCount = m_plGlobals.count();

    list.clear();
}

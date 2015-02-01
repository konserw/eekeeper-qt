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
#include "InfChr.h"
#include "EEKeeper.h"

#include <QFile>

CInfChr::CInfChr()
{
    m_bHasChanged = false;
    m_nError = ERR_NONE;
}

bool CInfChr::Read(const QString &pszFilename)
{
    QFile file(pszFilename);
    quint64 nCount;

    if (!file.open(QIODevice::ReadOnly))
    {
        m_nError = ERR_CHR_FAILEDOPEN;
        return(false);
    }
    m_strFilename = pszFilename;

    nCount = file.read(reinterpret_cast<char*>(&m_infChr),sizeof(INF_CHR));
    if (nCount != sizeof(INF_CHR))
    {
        m_nError = ERR_CHR_BADHEADER;
        file.close();
        return(false);
    }

    if (memcmp(m_infChr.chSignature,"CHR ",4))
    {
        m_nError = ERR_CHR_MISSINGSIG;
        file.close();
        return(false);
    }

    if (!_bIgnoreDataVersions && memcmp(m_infChr.chVersion,"V2.0",4) && memcmp(m_infChr.chVersion,"V2.1",4))
    {
        m_nError = ERR_CHR_BADVERSION;
        file.close();
        return(false);
    }

    if (!m_infCre.Read(file))
    {
        m_nError = m_infCre.GetLastError();
        file.close();
        return(false);
    }

    m_bHasChanged = false;
    return(true);
}

bool CInfChr::Write(const QString &pszFilename)
{
    m_infChr.dwDataStart = sizeof(INF_CHR);
    m_infChr.dwDataLen = m_infCre.GetFileSpace();

    QFile file(pszFilename);
    if (!file.open(QIODevice::WriteOnly))
    {
        m_nError = ERR_CHR_FAILEDCREATE;
        return(false);
    }

    if (file.write(reinterpret_cast<const char*>(&m_infChr), sizeof(INF_CHR)) != sizeof(INF_CHR))
    {
        m_nError = ERR_CHR_WRITEHEADER;
        file.close();
        return(false);
    }

    if (!m_infCre.Write(file))
    {
        m_nError = m_infCre.GetLastError();
        file.close();
        return(false);
    }

    m_bHasChanged = false;
    file.close();
    return(true);
}

void CInfChr::GetName(QString &strName)
{
    strName = m_infChr.szName;
}

void CInfChr::SetName(const char *pszName)
{
    qstrncpy(m_infChr.szName, pszName, CHR_NAME_MAXLEN-1);
}

bool CInfChr::HasChanged()
{
    if (m_bHasChanged || m_infCre.HasChanged())
        return(true);

    return(false);
}

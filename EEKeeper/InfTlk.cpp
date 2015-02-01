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
#include "InfTlk.h"
#include "EEKeeper.h"
#include "Log.h"

CInfTlk::CInfTlk() : m_file(NULL)
{
    m_pEntries = NULL;
}

CInfTlk::~CInfTlk()
{
    // close our open file
    if (m_file)
    {
        m_file->close();
        delete m_file;
    }

    delete [] m_pEntries;
}

bool CInfTlk::Open(const QString &pszFilename)
{
    qDebug() << "Opening dialog.tlk:";

    // close our old file
    if (m_file)
    {
        m_file->close();
        delete m_file;
        m_file = NULL;
    }

    m_file = new QFile(pszFilename);
    if (!m_file->open(QIODevice::ReadOnly) )
    {
        delete m_file;
        m_file = NULL;
        return(false);
    }

    qDebug() << "   Reading dialog header.";
    if (m_file->read(reinterpret_cast<char*>(&m_tlkHeader),sizeof(INF_TLK_HEADER)) != sizeof(INF_TLK_HEADER))
    {
        qDebug() << "      Unabled to read the header from the .tlk file.";
        m_file->close();
        delete m_file;
        m_file = NULL;
        return(false);
    }

    qDebug() << "   Allocating memory for string entries.";
    m_pEntries = new STRINGENTRY[m_tlkHeader.dwStringCount];
    if (!m_pEntries)
    {
        qDebug() << "      Unable to allocate memory for string index table.";
        m_file->close();
        delete m_file;
        m_file = NULL;
        return(false);
    }

    qDebug() << "   Reading string entries.";
    INF_TLK_ENTRY entry;
    for (int i=0; i < (int)m_tlkHeader.dwStringCount; i++)
    {
        if (m_file->read(reinterpret_cast<char*>(&entry),sizeof(INF_TLK_ENTRY)) != sizeof(INF_TLK_ENTRY))
        {
            qDebug() << "      Failed reading a talk file entry.";
            m_file->close();
            delete m_file;
            m_file = NULL;
            return(false);
        }

        m_pEntries[i].dwOffset = entry.dwOffset + m_tlkHeader.dwStringOffset;
        m_pEntries[i].dwLength = entry.dwLength;
        m_pEntries[i].wUnknown = entry.wUnknown;
    }

    qDebug() << "TLK String Count :" << m_tlkHeader.dwStringCount;
    qDebug() << "   Done with Dialog.tlk";

    return(true);
}

bool CInfTlk::GetString(quint32 dwIndex, QString &str)
{
    if (!m_file || !m_file->isOpen())
        return false;

    str.clear();
    if (dwIndex >= m_tlkHeader.dwStringCount)
        return(false);

    if (!m_pEntries[dwIndex].dwLength)
        return(false);

    if (!m_file->seek(m_pEntries[dwIndex].dwOffset) )
        return(false);

    // read in from the buffer
    char pBuf[ m_pEntries[dwIndex].dwLength + 1 ];
    memset(pBuf, 0, m_pEntries[dwIndex].dwLength + 1);
    if (m_file->read(reinterpret_cast<char*>(pBuf), m_pEntries[dwIndex].dwLength) != m_pEntries[dwIndex].dwLength)
        return(false);

    // convert to QString
    str = pBuf;
    return(true);
}

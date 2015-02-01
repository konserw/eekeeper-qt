#ifndef INFTLK_H
#define INFTLK_H

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

#include <QtCore>
#include <QFile>

#pragma pack(push,1)

struct INF_TLK_HEADER
{
    char		chSignature[4];
    char		chVersion[4];

    char		chUnknown[2];

    quint32		dwStringCount;
    quint32		dwStringOffset;
};

struct INF_TLK_ENTRY
{
    quint16		wUnknown;

    char		chSoundName[8];
    quint32		dwVolumeVariance;
    quint32		dwPitchVariance;
    quint32		dwOffset;
    quint32		dwLength;
};

#pragma pack(pop)

struct STRINGENTRY
{
    quint16		wUnknown;
    quint32		dwOffset;
    quint32		dwLength;
};

class CInfTlk
{
public:
    CInfTlk();
    ~CInfTlk();

    bool	Open(const QString &pszFilename);

    bool	GetString(quint32 dwIndex, QString &str);
    quint32	GetStringCount() const { return(m_tlkHeader.dwStringCount); }

    INF_TLK_HEADER	m_tlkHeader;

    // I only care about the real strings, not the sound stuff. So I'm only
    // storing the 2 pieces of data I need.
    STRINGENTRY	*m_pEntries;

private:
    QFile   *m_file;
};

#endif // INFTLK_H

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

#include "EEKeeper.h"
#include "Inf2DA.h"

CInf2DA::CInf2DA() : m_nRows(0), m_nCols(0), m_pszBuf(NULL), m_nBufLen(0), m_pHeader(NULL), m_pData(NULL)
{

}

CInf2DA::~CInf2DA()
{
    delete [] m_pszBuf;
    delete [] m_pHeader;
    delete [] m_pData;
}

bool CInf2DA::GetLine(int &nIndex, QString &strLine)
{
    strLine.clear();
    if (nIndex >= m_nBufLen)
        return(false);

    // Read characters up until the end of the buffer, a carriage return or a linefeed.
    while(m_pszBuf[nIndex] && m_pszBuf[nIndex] != '\r' && m_pszBuf[nIndex] != '\n')
    {
        strLine += m_pszBuf[nIndex];
        nIndex++;
    }

    // Position the index to the start of the next line.
    while(m_pszBuf[nIndex] && (m_pszBuf[nIndex] == '\r' || m_pszBuf[nIndex] == '\n'))
        nIndex++;

    return(true);
}

int CInf2DA::ParseLine(const QString &pszLine, QString *pRow)
{
    int nLen = pszLine.length();
    if (nLen == 0)
        return(0);

    int nCount = 0;
    int nIndex = 0;
    while(nIndex < nLen)
    {
        // Eat up any whitespace.
        while(nIndex < nLen && pszLine.at(nIndex).isSpace())
            nIndex++;

        // At end of buffer?
        if (nIndex >= nLen)
            break;

        if (pRow)
        {
            // I've seen some 2da files that have a row or two that has a lot more
            // columns (no data) than there are headers. If there is a pointer to
            // a row I'm assuming that the counts have already been done and that it
            // is ok to use m_nCols.
            if (nCount < m_nCols)
                pRow[nCount].clear();
            else
                return(nCount);
        }

        // Collect the data up until the next whitespace.
        while(nIndex < nLen && !pszLine.at(nIndex).isSpace())
        {
            if (pRow)
            {
                // See comments above about m_nCols and long rows.
                if (nCount < m_nCols)
                    pRow[nCount] += pszLine.at(nIndex);
                else
                    return(nCount);
            }
            nIndex++;
        }

        nCount++;
        nIndex++;
    }

    return(nCount);
}

bool CInf2DA::GetDimensions()
{
    QString strLine;
    int nIndex = 0;

    // Eat up the first two lines. The first line is the version string. I don't know
    // what the second line represents.
    if (!GetLine(nIndex, strLine) || !GetLine(nIndex, strLine) )
        return(false);

    // The next line has the header names for the columns.
    if (!GetLine(nIndex, strLine) )
        return(false);

    // First determine how many columns there are by the number of headers. The first
    // column doesn't have a header so Number of Headers + 1.
    m_nCols = ParseLine(strLine, NULL) + 1;
    if (!m_nCols)
        return(false);

    // Just need to count how many lines exist now.
    m_nRows = 0;
    while(GetLine(nIndex, strLine) )
        m_nRows++;

    return(true);
}

bool CInf2DA::Parse(const char *pszText)
{
    m_nBufLen = strlen(pszText);
    if (!m_nBufLen)
        return(false);

    m_pszBuf = new char[m_nBufLen+1];
    if (!m_pszBuf)
        return(false);

    // Keep a local copy around.
    qstrncpy(m_pszBuf, pszText, m_nBufLen+1);

    // Figure out how big the array needs to be.
    if (!GetDimensions())
        return(false);

    QString strLine;
    int i;
    int nIndex = 0;

    // Get the column headers. Eat up the first two lines and we want the third.
    // The number of columns I've seen has always been one less than the actual data
    // but I'm allocating enough room for a full row just in case the column header
    // line is missing; in which case it would parse out into a full row.
    m_pHeader = new QString[m_nCols];
    if (!m_pHeader)
        return(false);

    for (i=0; i<3; i++)
        if (!GetLine(nIndex, strLine) )
            return(false);

    if (ParseLine(strLine, m_pHeader) != m_nCols-1)
        return(false);

    m_pData = new QString[m_nRows*m_nCols];
    if (!m_pData)
        return(false);

    for (i=0;i<m_nRows;i++)
    {
        if (!GetLine(nIndex, strLine) )
            return(false);

        ParseLine(strLine, m_pData+i*m_nCols);
    }

    return(true);
}

QString CInf2DA::GetHeader(int nCol)
{
    if (nCol >= m_nCols-1)
        return(QString(""));

    return(m_pHeader[nCol]);
}

bool CInf2DA::GetData(int nRow, int nCol, int &nValue)
{
    if (nRow >= m_nRows || nCol >= m_nCols)
        return(false);

    bool ok;
    nValue = m_pData[nRow*m_nCols + nCol].toInt(&ok);
    return(ok);
}

bool CInf2DA::GetData(int nRow, int nCol, quint32 &dwValue)
{
    if (nRow >= m_nRows || nCol >= m_nCols)
        return(false);

    bool ok;
    dwValue = m_pData[nRow*m_nCols + nCol].toUInt(&ok);
    return(ok);
}

bool CInf2DA::GetData(int nRow, int nCol, QString &strValue)
{
    if (nRow >= m_nRows || nCol >= m_nCols)
        return(false);

    strValue = m_pData[nRow*m_nCols+nCol];
    return(true);
}

bool CInf2DA::GetDataByValue(int nSearchCol, int nSearchValue, int nValueCol, QString &strValue)
{
    if (nSearchCol >= m_nCols || nValueCol >= m_nCols)
        return(false);

    bool ok;
    int nValue;
    for (int i=0;i<m_nRows;i++)
    {
        nValue = m_pData[i*m_nCols+nSearchCol].toInt(&ok);
        if (ok && nValue == nSearchValue)
        {
            strValue = m_pData[i*m_nCols+nValueCol];
            return(true);
        }
    }

    return(false);
}

/************************************************************************************
    Copyright (c) 2000 Aaron O'Neil, 2013 Marshall Mattingly
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

#ifndef INF2DA_H
#define INF2DA_H

#include <QtCore>

class CInf2DA
{
public:
    CInf2DA();
    ~CInf2DA();

    // Pass in the 2DA resource text.
    bool Parse(const char *pszText);
    bool Parse(quint8 *pBuf)	{ return Parse( (const char *)pBuf ) ; }

    // Valid only after Parse returns successfully.
    int  GetRows() const { return(m_nRows); }
    int  GetCols() const { return(m_nCols); }

    // Returns the text header name of a column.
    QString GetHeader(int nCol);

    bool GetData(int nRow, int nCol, int &nValue);
    bool GetData(int nRow, int nCol, quint32 &dwValue);
    bool GetData(int nRow, int nCol, QString &strValue);

    // Given a search column and a search value it looks through all the rows for a match.
    // If found, it returns true and sets the value reference equal to the data in the
    // nValueCol.
    bool GetDataByValue(int nSearchCol, int nSearchValue, int nValueCol, QString &strValue);

private:

    // Gets a line of text from the buffer starting at the given index.
    // nIndex is update to be the next character after the end of the line.
    bool	GetLine(int &nIndex, QString &strLine);

    // Takes a line and parses it out the individual columns. Returns a count
    // of the number of items parsed. pRow needs to be a pointer to an array
    // of QStrings large enough to hold all the columns. If pRow is NULL then
    // just the count of columns is returned.
    int	ParseLine(const QString &pszLine, QString *pRow);

    // Figures out how many rows and columns of data exist.
    bool    GetDimensions();

private:

    int	m_nRows;
    int	m_nCols;

    // Holds the original unparsed data.
    char	*m_pszBuf;
    int     m_nBufLen;

    // Holds the column headers.
    QString *m_pHeader;

    // Array to hold the table data.
    QString *m_pData;

};

#endif // INF2DA_H

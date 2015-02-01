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

#ifndef VALUELIST_H
#define VALUELIST_H

#include <QtCore>
#include <QComboBox>
#include <QList>

struct TValueItem
{
    QString	strText;
    quint32	dwValue;

    TValueItem() :
        strText(""),
        dwValue(0)
    { }

    TValueItem(const QString& strText, quint32 dwValue) :
        strText(strText),
        dwValue(dwValue)
    { }

    TValueItem(const TValueItem& copy) :
        strText(copy.strText),
        dwValue(copy.dwValue)
    { }

    // comparator for QList
    bool operator== (const TValueItem& other) const
    {
        return (strText.compare(other.strText) == 0 && dwValue == other.dwValue);
    }

    // easy of use
    operator QVariant() const { return QVariant::fromValue(*this); }
};

// declare our TValueItem as a QMetaType for storage in our table
Q_DECLARE_METATYPE(TValueItem)

class CValueList : public QList<TValueItem>
{
public:
    CValueList();

    bool	Load(const QString &pszFilename, bool bAllowEmpty=false);
    bool	Save(const QString &pszFilename);
    bool    Save();

    bool	GetText(quint32 dwValue, QString &strText);

    void    FillComboBox(QComboBox &cb);
    static bool     SetComboBoxValue(QComboBox &cb, quint32 dwValue, bool bDisableOnNoMatch);
    static quint32  GetComboBoxValue(const QComboBox &cb);

private:
    QString m_strFile;
};

#endif // VALUELIST_H

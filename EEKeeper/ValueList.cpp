
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
#include "ValueList.h"

#include <QFile>

CValueList::CValueList() : QList<TValueItem>()
{
}

bool CValueList::Load(const QString &pszFilename, bool bAllowEmpty)
{
    // open our file
    m_strFile = pszFilename;
    QFile file(m_strFile);
    if (!file.open(QIODevice::ReadOnly)) {
        return bAllowEmpty;
    }

    // open the serialized data stream
    QDataStream ar(&file);
    if (ar.status() != QDataStream::Ok)
    {
        file.close();
        return bAllowEmpty;
    }

    int nCount;
    ar >> nCount;

    if (nCount < 1)
    {
        file.close();
        return bAllowEmpty;
    }

    TValueItem vi;
    for (int i=0; i<nCount && ar.status() == QDataStream::Ok; i++)
    {
        ar >> vi.strText;
        ar >> vi.dwValue;

        this->append(vi);
    }

    ar.setDevice(NULL);
    file.close();

    for (CValueList::iterator it = this->begin(); it != this->end(); ++it)
    {
        vi = (*it);

        qDebug() << vi.strText << "," << QString("0x") + QString::number(vi.dwValue, 16).toUpper();
    }

    return(true);
}

bool CValueList::Save()
{
    return this->Save(m_strFile);
}

bool CValueList::Save(const QString &pszFilename)
{
    QFile file(pszFilename);
    if (!file.open(QIODevice::WriteOnly) )
        return(false);

    // open the serialized data stream
    QDataStream ar(&file);
    if (ar.status() != QDataStream::Ok)
    {
        file.close();
        return false;
    }

    // write the size to the file
    ar << this->size();

    // write each CValueItem to the file
    TValueItem vi;
    for (CValueList::iterator it = this->begin(); it != this->end(); ++it)
    {
        vi = (*it);
        ar << vi.strText;
        ar << vi.dwValue;
    }

    ar.setDevice(NULL);
    file.close();

    return(true);
}

void CValueList::FillComboBox(QComboBox &cb)
{
    TValueItem vi;
    for (CValueList::iterator it = this->begin(); it != this->end(); ++it)
    {
        vi = (*it);

        // insert in alphabetical order
        int i = 0;
        for (; i < cb.count(); i++) {
            if (cb.itemText(i).compare(vi.strText) > 0)
                break;
        }

        cb.insertItem(i, vi.strText, vi.dwValue);
    }
}

bool CValueList::SetComboBoxValue(QComboBox &cb, quint32 dwValue, bool bDisableOnNoMatch)
{
    // loop through until we find the value
    int nCount = cb.count();
    for (int i=0; i<nCount; i++) {
        if (cb.itemData(i).toUInt() == dwValue)
        {
            cb.setCurrentIndex(i);
            if (bDisableOnNoMatch)
                cb.setEnabled(true);
            return(true);
        }
    }

    // disable the combobox, if needed, and clear the current index
    cb.setCurrentIndex(-1);
    if (bDisableOnNoMatch)
        cb.setEnabled(false);
    return(false);
}

quint32 CValueList::GetComboBoxValue(const QComboBox &cb)
{
    int nIndex = cb.currentIndex();
    if (nIndex == -1)
        return(0xFFFFFFFF);

    bool ok;
    quint32 nValue = cb.itemData(nIndex).toUInt(&ok);

    if (!ok)
        return 0xFFFFFFFF;

    return nValue;
}

bool CValueList::GetText(quint32 dwValue, QString &strText)
{
    TValueItem vi;
    for (CValueList::iterator it = this->begin(); it != this->end(); ++it)
    {
        vi = (*it);

        if (vi.dwValue == dwValue)
        {
            strText = vi.strText;
            return(true);
        }
    }

    return(false);
}

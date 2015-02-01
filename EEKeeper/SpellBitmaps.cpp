
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
#include "SpellBitmaps.h"
#include "InfKey.h"
#include "InfTlk.h"

#include "EEKeeper.h"

#include <QApplication>

CSpellBitmaps::CSpellBitmaps()
{
    m_nEmptyIndex = 0;
}

CSpellBitmaps::~CSpellBitmaps()
{
    // clear our map
    qDeleteAll(m_mapInfo);
}

bool CSpellBitmaps::LoadImages(bool bBlankOnly, QProgressDialog *pCtrl)
{
    qDebug() << "   Loading spell bitmaps.";

    //if (!m_ilSpells.Create(32,32,ILC_COLOR32,270,30))
    m_ilSpells.clear();

    CResInfoPlist *pSpells;
    if (!_infKey.GetResList(RESTYPE_SPL, pSpells)) {
        qDebug() << "       Failed to obtain the resources list.";
        qDebug() << "   Done loading spell bitmaps.";
        return(false);
    }

    /*
    CDC *pDC = AfxGetMainWnd()->GetDC();
    BITMAPINFOHEADER bih;
    memset(&bih,0,sizeof(BITMAPINFOHEADER));

    quint8 chBits[32*32*sizeof(COLORREF)];
    COLORREF clrWhite = RGB(255,255,255);
    for (int i=0;i<32*32;i++)
        memcpy(chBits+i*sizeof(COLORREF),&clrWhite,sizeof(COLORREF));

    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = 32;
    bih.biHeight = 32;
    bih.biPlanes = 1;
    bih.biBitCount = 32;
    bih.biCompression = BI_RGB;

    AfxGetMainWnd()->ReleaseDC(pDC);
    */

    // create our blank image and store the index
    QImage img(32, 32, QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    m_ilSpells.append(img);

    m_nEmptyIndex = m_ilSpells.size() - 1;

    if (bBlankOnly)
    {
        qDebug() << "       Using blank spell images.";

        pCtrl->setRange(0, 1);
        pCtrl->setValue(1);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        qDebug() << "   Done loading spell bitmaps.";
        return(true);
    }

    qDebug() << "       Spell count:" << QString::number(pSpells->count());

    if (pCtrl)
    {
        pCtrl->setRange(0, pSpells->count()-1);
        pCtrl->setValue(0);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    CResInfo *pResInfo;
    CInfBifFile *pBifFile;
    CSpellBitmapInfo *pInfo;
    int nIndex;
    int nCount = 0;
    char szSpellIcon[10];
    INF_SPL spl;

    memset(szSpellIcon, 0, 10);
    for (CResInfoPlist::iterator it = pSpells->begin(); it != pSpells->end(); ++it)
    {
        if (pCtrl)
        {
            pCtrl->setValue(++nCount);
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        pResInfo = (*it);

        pBifFile = _infKey.GetBifFile(pResInfo);
        if (!pBifFile || !pBifFile->IsValid()) {
            qDebug() << "           Failed to get BIF file!";
            continue;
        }

        if (pBifFile->GetData(pResInfo->wLocator, &spl) == -1) {
            continue;
        }

        QString szSpellIcon(spl.chSpellIcon);

        // See if we already have the icon in the list.
        if (m_mapInfo.contains(szSpellIcon) )
            continue;

        if (!EEKeeper::GetSpellBitmap(pResInfo, spl, qRgba(0, 0, 0, 0), img))
            continue;

        // At least one of the icon images is the wrong size. This causes the
        // image control to get confused somehow. So now I'm checking to make
        // sure the image really is 32x32 and not adding it if isn't. Berserk
        // "SPIN117" has a goofy sized icon, in fact it's a squirrel. It has 2
        // frames, like an inventory object where the rest of the spell icons
        // have just a single frame. Also, this description for this innate
        // is messed up, it says barkskin. This probably isn't supposed to be
        // available. The game has some cloud with lightning icon when you give
        // this character that ability. No clue where it pulls that icon from.
        if (img.width() != 32 || img.height() != 32)
            continue;

        m_ilSpells.append(img);
        nIndex = m_ilSpells.size() - 1;
        if (nIndex == -1)
            continue;

        pInfo = new CSpellBitmapInfo;
        if (!pInfo)
            return(false);

        pInfo->nIndex = nIndex;
        m_mapInfo.insert(szSpellIcon, pInfo);
    }

    return(true);
}

int CSpellBitmaps::GetImageIndex(const QString &pszResource)
{
    SpellBitmapMap::iterator it = m_mapInfo.find(pszResource);
    if (it == m_mapInfo.end())
        return(m_nEmptyIndex);

    return((*it)->nIndex);
}

void CSpellBitmaps::GetIcon(int index, QIcon &icon)
{
    if (index < 0 || index >= m_ilSpells.size()) {
        index = m_nEmptyIndex;
    }

    QPixmap pixmap = QPixmap::fromImage(m_ilSpells[index]);
    icon = QIcon(pixmap);
}

void CSpellBitmaps::GetIcon(const QString &pszResource, QIcon &icon)
{
    GetIcon(GetImageIndex(pszResource), icon);
}

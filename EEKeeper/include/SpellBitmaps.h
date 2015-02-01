#ifndef SPELLBITMAPS_H
#define SPELLBITMAPS_H

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

// This class loads all the spell BAMs and creates an image list out of them.
// The main reason I did it this way was to for managing the icons shown in
// the CRE spell editor. Managing a small image list that pulls from a pool of
// a lot of different images is a real pain -- the list items are associated
// in the list by an index which will change if items are deleted or added.
//
// There are not all that many spell icons (compared to items anyway).

#include <QtCore>
#include <QIcon>
#include <QImage>
#include <QList>
#include <QMap>
#include <QProgressDialog>

typedef QList<QImage> QImageList;

struct CSpellBitmapInfo
{
    int nIndex;
};

typedef QMap<QString, CSpellBitmapInfo*> SpellBitmapMap;

class CSpellBitmaps
{
public:
    CSpellBitmaps();
    ~CSpellBitmaps();

    // Can only be called after all the resources have been index. This runs
    // through all the spell resources and creates a bitmap from each BAM. This
    // bitmap is then added to the image list and then destroyed. If bBlankOnly
    // is true on a blank icon is created for the image list. This is to accomodate
    // users turning off the usage of spell icons. I still need a blank icon to
    // stick in the spell list control.
    bool LoadImages(bool bBlankOnly, QProgressDialog *pCtrl=NULL);

    // Pass in a resource string for a spell, returns the index in the image map
    // for this spell. -1 if the spell isn't found.
    int	GetImageIndex(const QString &pszResource);

    // Get an Icon representing the spell
    void GetIcon(int index, QIcon &icon);
    void GetIcon(const QString &pszResource, QIcon &icon);

public:
    QImageList      m_ilSpells;
    SpellBitmapMap  m_mapInfo;
    int             m_nEmptyIndex;

};

#endif // SPELLBITMAPS_H

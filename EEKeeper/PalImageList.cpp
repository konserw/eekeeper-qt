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
#include "PalImageList.h"
#include "EEKeeper.h"

CPalImageList::CPalImageList() : QList<QImage>()
{

}

bool CPalImageList::CreateFromBitmap(const QImage &/*hBitmap*/)
{
    return false;
    /*
    AddToLog("Creating Image List from bitmap.\n");

    BITMAP bm;

    AddToLog("   Getting bitmap info.\n");
    if (!::GetObject(hBitmap,sizeof(bm),&bm))
        return(false);

    AddToLog("   Getting DC.\n");
    CDC *pDC = AfxGetMainWnd()->GetDC();
    if (!pDC)
        return(false);

    AddToLog("   Creating compatible bitmap.\n");
    CBitmap bmp;
    if (!bmp.CreateCompatibleBitmap(pDC,bm.bmWidth*2*bm.bmHeight,bm.bmWidth*2))
    {
        AfxGetMainWnd()->ReleaseDC(pDC);
        return(false);
    }

    AddToLog("   Creating compatible DC.\n");
    CDC dc;
    if (!dc.CreateCompatibleDC(pDC))
    {
        AfxGetMainWnd()->ReleaseDC(pDC);
        return(false);
    }

    AddToLog("   Allocating bit buffer.\n");
    quint8 *pBits = new quint8[bm.bmWidth*4*bm.bmHeight];
    if (!pBits)
    {
        AfxGetMainWnd()->ReleaseDC(pDC);
        return(false);
    }

    BITMAPINFO bmi;
    memset(&bmi,0,sizeof(bmi));

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biHeight = bm.bmHeight*-1;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = bm.bmWidth;
    bmi.bmiHeader.biCompression = BI_RGB;

    AddToLog("   Getting bitmap bits.\n");
    if (!::GetDIBits(
            pDC->GetSafeHdc(),
            hBitmap,
            0,
            bm.bmHeight,
            pBits,
            &bmi,
            DIB_RGB_COLORS))
    {
        AfxGetMainWnd()->ReleaseDC(pDC);
        delete [] pBits;
        return(false);
    }

    CBitmap *pOldBitmap = dc.SelectObject(&bmp);

    AddToLog("   Creating color samples.\n");
    quint8 *ptr;
    COLORREF clrFixed;
    RGBQUAD *pQuad;
    CRect r;
    int j;
    for (int i=0;i<bm.bmHeight;i++)
    {
        r.top = 0;
        r.bottom = bm.bmWidth*2;
        r.left = i * (bm.bmWidth*2);
        r.right = r.left + bm.bmWidth*2;

        ptr = pBits + (i * (bm.bmWidth*4)) + 4;

        // I didn't do the full row. The pointy shape you end up with if you do
        // all the colors down to a 2x2 square doesn't look as good. The colors
        // on the right side are the darker versions generally so the color shows
        // up a little better this way too -- brighter overall.
        for (j=bm.bmWidth-3;j>=1;j--)
        {
            ptr = pBits + (i * (bm.bmWidth*4)) + j*4;
            pQuad = (RGBQUAD*)ptr;
            clrFixed = RGB(pQuad->rgbRed,pQuad->rgbGreen,pQuad->rgbBlue);

            dc.FillSolidRect(&r,clrFixed);
            r.DeflateRect(1,1);
        }
    }

    dc.SelectObject(pOldBitmap);
    delete [] pBits;

    Create(bm.bmWidth*2,bm.bmWidth*2,ILC_COLOR24,0,1);
    Add(&bmp,RGB(255,255,255));

    AddToLog("   Finished creating Image List.\n");
    return(true);
    */
}

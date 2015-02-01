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
#include "EEKeeper.h"
#include "InfBam.h"

#include <QColor>

CInfBam::CInfBam()
{
    m_pData = NULL;
    m_pHeader = NULL;
    m_nDataLen = 0;
}

CInfBam::~CInfBam()
{
    delete [] m_pData;
}

void CInfBam::SetBuffer(uchar *pData, quint32 nDataLen)
{
    m_pData = pData;
    m_nDataLen = nDataLen;
    m_pHeader = (INF_BAM_HEADER*)m_pData;
}

bool CInfBam::MakeBitmap(int nFrameWanted, QRgb clrTrans, QImage &image,
                         bool bForceSize, int nWidth, int nHeight)
{
    //ASSERT(m_pHeader);
    if (!m_pHeader)
        return false;

    if (m_pHeader->wFrameCount < 1)
        return false;

    if (nFrameWanted >= m_pHeader->wFrameCount)
        nFrameWanted = 0;

    INF_BAM_FRAME *pFrame = (INF_BAM_FRAME*)(m_pData+m_pHeader->dwFrameOffset + ( nFrameWanted*sizeof(INF_BAM_FRAME) ) );

    if (pFrame->wWidth > MAX_ICON_WIDTH || pFrame->wHeight > MAX_ICON_HEIGHT)
        return(false);

    // get the pallette
    QRgb *pPal = (QRgb*)(m_pData + m_pHeader->dwPaletteOffset);
    uchar *pRawBits = m_pData + (pFrame->dwFrameDataOffset & 0x7FFFFFFF);
    bool bIsCompressed = !(pFrame->dwFrameDataOffset & 0x80000000);

    image = QImage(pFrame->wWidth, pFrame->wHeight, QImage::Format_ARGB32);

    int nNumPixels = pFrame->wWidth * pFrame->wHeight;
    int nSourceOff = 0;
    int nPixelCount = 0;
    int nCount;
    int x = 0;
    int y = 0;

    while(nPixelCount < nNumPixels)
    {
        if (bIsCompressed && pRawBits[nSourceOff] == m_pHeader->chTransparentIndex)
        {
            nSourceOff++;
            nCount = pRawBits[nSourceOff] + 1;
            while(nCount)
            {
                //TODO: double-check x/y
                image.setPixel(x, y, clrTrans);
                nCount--;
                nPixelCount++;

                if (++x >= pFrame->wWidth) {
                    x = 0;
                    ++y;
                }
            }

            nSourceOff++;
        }
        else
        {
            // If it is not compressed, still need to catch the transparent pixels and
            // fill with the transaprent color.
            if (pRawBits[nSourceOff] == m_pHeader->chTransparentIndex)
                image.setPixel(x, y, clrTrans);
            else {
                QRgb revColor  = *(pPal + pRawBits[nSourceOff]);
                QRgb realColor = qRgb(qRed(revColor), qGreen(revColor), qBlue(revColor));
                image.setPixel(x, y, realColor);
            }

            nSourceOff++;
            nPixelCount++;

            if (++x >= pFrame->wWidth) {
                x = 0;
                ++y;
            }
        }
    }

    if (bForceSize) {
        EEKeeper::ResizeBitmap(nWidth, nHeight, image, clrTrans);
    }

    return(true);
}

bool CInfBam::ConvertToGrey(QRgb *pPal)
{
    int nRed, nGreen, nBlue;
    quint8 chGrey;
    quint8 *pClr;

    for (int i=0;i<256;i++)
    {
        pClr = (quint8*)(pPal+i);

        nBlue = *pClr+1;
        nGreen = *pClr+2;
        nRed = *pClr+3;

        chGrey = (quint8)((nRed+nGreen+nBlue)/3);

        *(pClr+1) = chGrey;
        *(pClr+2) = chGrey;
        *(pClr+3) = chGrey;
    }

    return(true);
}


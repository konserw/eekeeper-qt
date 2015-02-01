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

#ifndef INFBAM_H
#define INFBAM_H

#include <QtCore>
#include <QImage>

#pragma pack(push,1)

struct INF_BAM_HEADER
{
    char    chSignature[4];
    char	chVersion[4];
    quint16	wFrameCount;
    quint8	chCycleCount;
    quint8	chTransparentIndex;
    quint32	dwFrameOffset;
    quint32	dwPaletteOffset;
    quint32	dwFrameLookupTableOffset;
};

struct INF_BAM_FRAME
{
    quint16	wWidth;
    quint16	wHeight;
    quint16	wCenterX;
    quint16	wCenterY;
    quint32	dwFrameDataOffset;		// bit 31 is a flag for RLE. 1 = RLE
};

struct INF_BAM_CYCLE
{
    quint16	wFrameIndexCount;
    quint16	wFirstFrameIndex;			// Index into the frame lookup table.
};

#pragma pack(pop)

#define MAX_ICON_WIDTH		400
#define MAX_ICON_HEIGHT		400

class CInfBam
{
public:
    CInfBam();
    ~CInfBam();

    /**
     * @brief SetBuffer sets the data buffer and must be called before any other functions.
     * @param pData the data
     * @param nDataLen the length of the data
     */
    void SetBuffer(uchar *pData, quint32 nDataLen);

    /**
     * @brief GetBuffer grabs the data buffer
     * @return the data buffer
     */
    uchar* GetBuffer()	const { return(m_pData); }

    /**
     * @brief MakeBitmap creates a QImage from the data set earlier
     * @param nFrameWanted the frame to grab
     * @param clrTrans the transparent color
     * @param image reference to the image that will be returned
     * @param bForceSize if true, forces to scale to width and height
     * @param nWidth the width to scale
     * @param nHeight the height to scale
     * @return true if successful
     */
    bool MakeBitmap(int nFrameWanted, QRgb clrTrans, QImage &image,
                    bool bForceSize=false, int nWidth=0, int nHeight=0);

    /**
     * @brief ConvertToGrey converts the given palette to grayscale
     * @param pPal the palette to convert
     * @return true if successful
     */
    static bool ConvertToGrey(QRgb *pPal);

private:
    // Pointer to the data buffer attached to the memory file.
    uchar           *m_pData;
    quint32			m_nDataLen;

    INF_BAM_HEADER	*m_pHeader;
};

#endif // INFBAM_H

/*
 * Name        : Screenshot.cpp
 * Description : Performs screenshot and bitmap conversion
 * Project     : This file is part of OpenMAR, an Open Mobile Augmented Reality browser
 * Website     : http://OpenMAR.org
 *
 * Copyright (c) 2010 David Caabeiro
 *
 * All rights reserved. This program and the accompanying materials are made available 
 * under the terms of the Eclipse Public License v1.0 which accompanies this 
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 *
 */

#include "Screenshot.h"

#include <eikenv.h>
#include <fbs.h>
#include <ImageConversion.h> 
#include <PathInfo.h>
#include <w32std.h>

#include "Logger.h"

CScreenshot* CScreenshot::NewL(MObserver& aObserver, const TSize& aSize, const TDisplayMode& aDisplayMode)
{
    CScreenshot* self = new(ELeave) CScreenshot(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aSize, aDisplayMode);
    CleanupStack::Pop();

    return self;
}

CScreenshot::~CScreenshot()
{
    Cancel();

    delete iEncoder;
    
    delete iBitmapContext;
    delete iBitmapDevice;
    delete iBitmap;
}

CScreenshot::CScreenshot(MObserver& aObserver)
    :CActive(EPriorityStandard), iObserver(aObserver)
{
    CActiveScheduler::Add(this);
}

void CScreenshot::ConstructL(const TSize& aSize, const TDisplayMode& aDisplayMode)
{
    // Create bitmap where screen bitmap will be blitted
    iBitmap = new(ELeave) CFbsBitmap;
    iBitmap->Create(aSize, aDisplayMode);

    iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);
    User::LeaveIfError(iBitmapDevice->CreateContext(iBitmapContext));

    LOGARG("Bitmap capture size is %d x %d", aSize.iWidth, aSize.iHeight);
}

void CScreenshot::RequestL(const CFbsBitmap& aSourceBitmap, const TSize& aSize)
{
    // Blit screen bitmap into internal bitmap
    iBitmapContext->BitBlt(TPoint(0, 0), &aSourceBitmap, TRect(TPoint(0, 0), aSize));

    // Generate file name
    TTime time;
    time.HomeTime();

    TDateTime dateTime = time.DateTime();

#if defined(__WINS__)
    iFilename.Copy(PathInfo::PhoneMemoryRootPath());
#else
    iFilename.Copy(PathInfo::MemoryCardRootPath());
#endif
    iFilename.Append(PathInfo::ImagesPath());

    _LIT(KFormat, "%04d%02d%02d%02d%02d%02d.png");
    iFilename.AppendFormat(KFormat, dateTime.Year(), dateTime.Month() + 1, dateTime.Day() + 1, dateTime.Hour(), dateTime.Minute(), dateTime.Second());

    // Convert to PNG
    delete iEncoder;
    iEncoder = 0;

    iEncoder = CImageEncoder::FileNewL(CCoeEnv::Static()->FsSession(), iFilename, CImageEncoder::EOptionAlwaysThread, KImageTypePNGUid);
    iEncoder->Convert(&iStatus, *iBitmap);
    SetActive();
}

void CScreenshot::RunL()
{   
    iObserver.ScreenshotReadyL(iStatus.Int(), iFilename);
}

void CScreenshot::DoCancel()    
{
    if (iEncoder)
        iEncoder->Cancel();
}

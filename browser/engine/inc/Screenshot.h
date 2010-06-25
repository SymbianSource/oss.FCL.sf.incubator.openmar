/*
 * Name        : Screenshot.h
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

#ifndef SCREENSHOT_H_
#define SCREENSHOT_H_

#include <e32base.h>
#include <gdi.h>

class TSize;

class CFbsBitmap;
class CFbsBitmapDevice;
class CFbsBitGc;

class CImageEncoder;

class CScreenshot : public CActive
{
public:
    class MObserver
    {
    public:
        virtual void ScreenshotReadyL(TInt aError, const TDesC& aFilename) = 0;
    };

public:
    static CScreenshot* NewL(MObserver& aObserver, const TSize& aSize, const TDisplayMode& aDisplayMode);
    ~CScreenshot();

protected:
    CScreenshot(MObserver& aObserver);
    void ConstructL(const TSize& aSize, const TDisplayMode& aDisplayMode);

public:
    void RequestL(const CFbsBitmap& aSourceBitmap, const TSize& aSize);

protected:
    void RunL();
    void DoCancel();

private:
    MObserver& iObserver;

    CFbsBitmap* iBitmap;
    CFbsBitmapDevice* iBitmapDevice;
    CFbsBitGc* iBitmapContext;

    TFileName iFilename;
    CImageEncoder* iEncoder;
};

#endif

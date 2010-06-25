/*
 * Name        : SplashContainer.cpp
 * Description : 
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

#include "SplashContainer.h"

#include <AknDef.h>
#include <AknUtils.h>
#include <AknViewAppui.h>
#include <AknView.h>

#include <fbs.h>
#include <GulIcon.h>

//#include <Splash_0x2002E1AB.mbg>

//#include "IconLoader.h"

#include "Logger.h"

CSplashContainer::CSplashContainer(CAknView& aView)
    : iView(aView)
{}

CSplashContainer::~CSplashContainer()
{
    delete iBackground;

    delete iBitmapContext;
    delete iBitmapDevice;
    delete iBitmap;
}

void CSplashContainer::ConstructL(const TRect& aRect, const CCoeControl* aParent)
{
    if (aParent)
        SetContainerWindowL(*aParent);
    else
        CreateWindowL();

    iBitmap = new(ELeave) CFbsBitmap;
    iBitmap->Create(aRect.Size(), Window().DisplayMode());

    iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);
    User::LeaveIfError(iBitmapDevice->CreateContext(iBitmapContext));

    iBitmapContext->SetBrushColor(KRgbBlue);
    iBitmapContext->SetBrushStyle(CFbsBitGc::ESolidBrush);
    iBitmapContext->Clear();

    const CFont* font = iEikonEnv->LegendFont();
    iBitmapContext->UseFont(font);

    iBitmapContext->SetPenColor(KRgbWhite);
    iBitmapContext->SetPenStyle(CFbsBitGc::ESolidPen);

    _LIT(KPrimaryText, "Initializing..");
    iBitmapContext->DrawText(KPrimaryText, TPoint(font->WidthZeroInPixels(), font->FontMaxHeight()));

//    _LIT(KImageFile, "\\resource\\apps\\Splash_0x20026309.mif");
//    iBackground = CreateIconL(KImageFile, EMbmSplash_0x20026309Splash, EMbmSplash_0x20026309Splash_mask, aRect.Size());

    SetRect(aRect);
    ActivateL();
}

void CSplashContainer::HandleResourceChange(TInt aType)
{
    LOGARG("CSplashContainer::HandleResourceChange(%d)", aType);

    CCoeControl::HandleResourceChange(aType);

    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EApplicationWindow, rect);
        SetRect(rect);
    }
}

void CSplashContainer::Draw(const TRect& aRect) const
{
    CWindowGc& gc = SystemGc();
    gc.BitBlt(TPoint(0, 0), iBitmap);
}

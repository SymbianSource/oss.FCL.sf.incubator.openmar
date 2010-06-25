/*
 * Name        : ARContainer.cpp
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

#include "ARContainer.h"

#include <coemain.h>
#include <eikenv.h>

#include <AknButton.h>
#include <AknNoteWrappers.h>
#include <AknView.h>

#include "Application.hrh"

#include <OpenMAR_0x2002E1AB.rsg>
#include <Button_0x2002E1AB.mbg>

#include "IconLoader.h"
#include "ResetAndDestroy.h"

#include "Logger.h"

// Constant definitions
_LIT(KButtonFile, "\\resource\\apps\\Button_0x2002E1AB.mif");
const TSize KButtonSize(36, 36);

CARContainer* CARContainer::NewL(CAknView& aView, const TRect& aRect)
{
    CARContainer* self = new(ELeave) CARContainer(aView);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    CleanupStack::Pop(self);

    return self;
}

CARContainer::CARContainer(CAknView& aView)
    : iView(aView)
{}

void CARContainer::ConstructL(const TRect& aRect)
{
    CreateWindowL();

    CreateButtonGroupL();

    // This bitmap will contain the camera frame and overlays
    iBitmap = new(ELeave) CFbsBitmap;
    iBitmap->Create(aRect.Size(), Window().DisplayMode());

    iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);
    User::LeaveIfError(iBitmapDevice->CreateContext(iBitmapContext));

    // Make black background to be shown immediately to the user 
    iBitmapContext->SetBrushColor(KRgbBlack);
    iBitmapContext->SetBrushStyle(CFbsBitGc::ESolidBrush);
    iBitmapContext->Clear();

    SetRect(aRect);

    LOGARG("Control rect size is %d x %d", aRect.Width(), aRect.Height());

    iCoeEnv->AddForegroundObserverL(*this);

#if defined(__MARM__)
    iCamera = CDigitalCamera::NewL(*this);

    // The rect will be the intersection between the camera frame
    // and the window rect
    TRect cameraRect(TPoint(0, 0), iCamera->ViewportSize());
    cameraRect.Intersection(aRect);
#else
    TRect cameraRect(aRect);
#endif

    LOGARG("Viewport rect is (%d, %d, %d, %d)", 
            cameraRect.iTl.iX, cameraRect.iTl.iY, cameraRect.iBr.iX, cameraRect.iBr.iY);

    iScreenshot = CScreenshot::NewL(*this, cameraRect.Size(), Window().DisplayMode());

    // Instantiate overlay implementations
    RImplInfoPtrArray implementations;
    REComSession::ListImplementationsL(TUid::Uid(KOverlayInterfaceUidValue), implementations);
    CleanupResetAndDestroyPushL(implementations);

    // Pass container window and rect size to each overlay
    OpenMAR::CPOIOverlay::SParameter param = { Window(), cameraRect };

    for (TInt i = 0; i < implementations.Count(); ++i)
    {
        const CImplementationInformation* info = implementations[i];
        OpenMAR::CPOIOverlay* plugin = OpenMAR::CPOIOverlay::NewL(info->ImplementationUid(), param);
        iOverlayList.AppendL(plugin);
    }

    CleanupStack::PopAndDestroy(&implementations);

    // Start camera in low priority task
    iSensorTask = CIdle::NewL(CActive::EPriorityIdle);

    TCallBack sensorStart(SensorStart, this);
    iSensorTask->Start(sensorStart);

    ActivateL();
}

CARContainer::~CARContainer()
{
    DoSensorStop();

    iOverlayList.ResetAndDestroy();
    iOverlayList.Close();

    delete iCamera;
    delete iScreenshot;

    iCoeEnv->RemoveForegroundObserver(*this);

    delete iBitmapContext;
    delete iBitmapDevice;
    delete iBitmap;

    DeleteButtonGroup();
}

void CARContainer::CreateButtonGroupL()
{
    // Screenshot button
    CGulIcon* screenshotNormal = CreateIconL(KButtonFile, EMbmButton_0x2002e1abScreenshotbuttonnormal, EMbmButton_0x2002e1abScreenshotbuttonnormal_mask, KButtonSize);

    iScreenshotButton = CAknButton::NewL(screenshotNormal, 0, 0, 0, KNullDesC, KNullDesC, KAknButtonNoFrame, 0);
    iScreenshotButton->SetContainerWindowL(*this);
    iScreenshotButton->SetObserver(this);
}

void CARContainer::DeleteButtonGroup()
{
    delete iScreenshotButton;
}

TInt CARContainer::SensorStart(TAny* aPtr)
{
    CARContainer* self = static_cast<CARContainer*>(aPtr);
    self->DoSensorStart();
    return KErrNone;
}

void CARContainer::DoSensorStart()
{
    LOGTXT("Starting sensors..");

#if defined(__MARM__)
    iCamera->Start();
#endif

    for (TInt i = 0; i < iOverlayList.Count(); ++i)
        iOverlayList[i]->StartL();
}

TInt CARContainer::SensorStop(TAny* aPtr)
{
    CARContainer* self = static_cast<CARContainer*>(aPtr);
    self->DoSensorStop();
    return KErrNone;
}

void CARContainer::DoSensorStop()
{
    LOGTXT("Stopping sensors..");

    for (TInt i = 0; i < iOverlayList.Count(); ++i)
        iOverlayList[i]->Stop();

#if defined(__MARM__)
    iCamera->Stop();
#endif
}

/*
 * Before performing the screenshot, disable overlay and sensor activity.
 * This way the screenshot image is generated fast.
 */  

void CARContainer::ScreenshotStartL()
{
    iScreenshotButton->SetDimmed(ETrue);

    HandleLosingForeground();
    iScreenshot->RequestL(*iBitmap, iBitmap->SizeInPixels());
}

/*
 * Resume overlay and sensor activity.
 */
void CARContainer::ScreenshotStop()
{
    HandleGainingForeground();

    iScreenshotButton->SetDimmed(EFalse);
}

void CARContainer::SizeChanged()
{
    TRect screenshotButtonRect(TPoint(0, 0), KButtonSize);
    screenshotButtonRect.Move(TPoint(Rect().Width() - KButtonSize.iWidth, Rect().Height() - KButtonSize.iHeight));

    iScreenshotButton->SetRect(screenshotButtonRect);
}

void CARContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
//    if (aPointerEvent.iType == TPointerEvent::EButton1Down && iCamera)
//        iCamera->StartFocus();
    CCoeControl::HandlePointerEventL(aPointerEvent);
}

void CARContainer::HandleResourceChange(TInt aType)
{
    LOGARG("CARContainer::HandleResourceChange(%d)", aType);

    CCoeControl::HandleResourceChange(aType);
/*
    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EApplicationWindow, rect);
        SetRect(rect);
    }
*/
}

void CARContainer::Draw(const TRect& /*aRect*/) const
{
    CWindowGc& gc = SystemGc();
    gc.BitBlt(TPoint(0, 0), iBitmap);
}

TInt CARContainer::CountComponentControls() const
{
    return ETotal;
}

CCoeControl* CARContainer::ComponentControl(TInt aIndex) const
{
    switch (aIndex)
    {
        case EScreenshot:
            return iScreenshotButton;

        default:
            return 0;
    }
}

void CARContainer::HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType)
{
    if (aEventType == EEventStateChanged)
    {
        if (aControl == iScreenshotButton)
        {
            LOGTXT("Screenshot requested");

            ScreenshotStartL();
        }
    }
}

void CARContainer::HandleGainingForeground()
{
    LOGTXT("Gaining ForegroundL");

    // Clear bitmap to avoid showing screen capture when back
    iBitmapContext->SetBrushColor(KRgbBlack);
    iBitmapContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmapContext->Clear();

    iSensorTask->Cancel();
    TCallBack sensor(SensorStart, this);
    iSensorTask->Start(sensor);
}

void CARContainer::HandleLosingForeground()
{
    LOGTXT("Losing foreground");
/*
    iSensorTask->Cancel();
    TCallBack sensor(SensorStop, this);
    iSensorTask->Start(sensor);
*/
    DoSensorStop();
}

void CARContainer::CameraReady(TInt aError)
{
    LOGARG("Camera ready (error %d)", aError);

    // Perform some error handling
}

void CARContainer::CameraFrame(CFbsBitmap& aFrame)
{
    // Use camera frame as background
    iBitmapContext->BitBlt(TPoint(0, 0), &aFrame);

    for (TInt i = 0; i < iOverlayList.Count(); ++i)
    {
        // Perform overlay rendering
        const CFbsBitmap& overlay = iOverlayList[i]->RenderScene();
        // Blend it with the background image
        iBitmapContext->BitBlt(TPoint(0, 0), &overlay);
    }

    const CFont* font = iEikonEnv->LegendFont();
    iBitmapContext->UseFont(font);

    _LIT(KText, "Focus: %d");
    TBuf<32> text;
    text.Format(KText, iOverlayList[0]->GetFocusedPOI());

    const TPoint pos(10, 20);

    iBitmapContext->DrawText(text, pos);

    // TODO: should call this every Nth frame
    User::ResetInactivityTime();

    DrawNow();
}

void CARContainer::FocusReady(TInt aError)
{
    if (aError == KErrNone)
        LOGTXT("Focused correctly");
//    else if (aError == KErrECamNotOptimalFocus)
//        LOGTXT("Focus not optimal");
}

void CARContainer::ScreenshotReadyL(TInt aError, const TDesC& aFilename)
{
    ScreenshotStop();

    TBuf<128> text;

    if (aError)
    {
        CAknErrorNote* note = new(ELeave) CAknErrorNote;
        HBufC* format = iEikonEnv->AllocReadResourceLC(R_SCREENSHOT_TEXT_ERROR);
        text.Format(*format, aError);
        CleanupStack::PopAndDestroy(format);
        note->ExecuteLD(text);
    }
    else
    {
        CAknInformationNote* note = new(ELeave) CAknInformationNote;
        HBufC* format = iEikonEnv->AllocReadResourceLC(R_SCREENSHOT_TEXT_OK);
        text.Format(*format, &aFilename);
        CleanupStack::PopAndDestroy(format);
        note->ExecuteLD(text);
    }
}

/*
 * Name        : ARContainer.h
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

#ifndef __ARCONTAINER_H__
#define __ARCONTAINER_H__

#include <coecntrl.h>
#include <coecobs.h>
#include <AknButton.h>

#include "POIOverlay.h"
#include "Camera.h"
#include "Screenshot.h"

class CAknView;

/**
 * @brief AR control class
 * 
 * This class provides the basic view and is in charge of initializing the camera, 
 * which dictates the refresh rate of the overlays
 */
class CARContainer : public CCoeControl,
            public MCoeControlObserver,
            public MCoeForegroundObserver,
            public CDigitalCamera::MObserver,
            public CScreenshot::MObserver
{
public:
    static CARContainer* NewL(CAknView& aView, const TRect& aRect);
    ~CARContainer();

private:
    CARContainer(CAknView& aView);
    void ConstructL(const TRect& aRect);

    void CreateButtonGroupL();
    void DeleteButtonGroup();

    static TInt SensorStart(TAny* aPtr);
    void DoSensorStart();

    static TInt SensorStop(TAny* aPtr);
    void DoSensorStop();

    void ScreenshotStartL();
    void ScreenshotStop();

protected:
    void SizeChanged();
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    void HandleResourceChange(TInt aType);
    void Draw(const TRect& aRect) const;

    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;

protected:
    // From MCoeControlObserver
    void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

    // From MCoeForegroundObserver
    void HandleGainingForeground();
    void HandleLosingForeground();

    // From CDigitalCamera::MObserver
    void CameraReady(TInt aError);
    void CameraFrame(CFbsBitmap& aFrame);
    void FocusReady(TInt aError);

    // From CScreenshot::MObserver
    void ScreenshotReadyL(TInt aError, const TDesC& aFilename);

private:
    CAknView& iView;
    CAknButton* iScreenshotButton;

    CFbsBitmap* iBitmap;
    CFbsBitmapDevice* iBitmapDevice;
    CFbsBitGc* iBitmapContext;

    enum TButtonId {
        EScreenshot,
        ETotal
    };

private:
    RPointerArray<OpenMAR::CPOIOverlay> iOverlayList;
    CDigitalCamera* iCamera;
    CScreenshot* iScreenshot;
    CIdle* iSensorTask;
};

#endif // __ARCONTAINER_H__

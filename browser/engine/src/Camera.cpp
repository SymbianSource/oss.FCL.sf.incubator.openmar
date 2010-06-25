/*
 * Name        : Camera.cpp
 * Description : Camera helper class
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

#include "Camera.h"

#include <ecamadvsettings.h>
#include <fbs.h>

#include "Logger.h"

CDigitalCamera* CDigitalCamera::NewL(MObserver& aObserver)
{
    CDigitalCamera* self = new(ELeave) CDigitalCamera(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CDigitalCamera::~CDigitalCamera()
{
    delete iCameraSetting;
    delete iCamera;
}

CDigitalCamera::CDigitalCamera(MObserver& aObserver)
    : iObserver(aObserver), iViewportSize(KMaxTInt, KMaxTInt)
{}

void CDigitalCamera::ConstructL()
{
    TInt cameraCount = CCamera::CamerasAvailable();

    LOGARG("Camera count is %d", cameraCount);

    // Use main camera (index 0)
    iCamera = CCamera::New2L(*this, 0, 0);
    iCameraSetting = CCamera::CCameraAdvancedSettings::NewL(*iCamera);

    TCameraInfo cameraInfo;
    iCamera->CameraInfo(cameraInfo);

    LOGTXT("Camera capture sizes supported:");

    for (TInt i = 0; i < cameraInfo.iNumImageSizesSupported; ++i)
    {
        TSize size;
        iCamera->EnumerateCaptureSizes(size, i, CCamera::EFormatExif); 

        if (iViewportSize.iWidth > size.iWidth && iViewportSize.iHeight > size.iHeight)
            iViewportSize = size;

        LOGARG("Index %d Size %d x %d", i, size.iWidth, size.iHeight);
    }

    LOGARG("Using smallest viewport: %d x %d", iViewportSize.iWidth, iViewportSize.iHeight);
}

void CDigitalCamera::Start()
{
    iCamera->Reserve();
}

void CDigitalCamera::Stop()
{
    iCamera->Release();
}

void CDigitalCamera::StartFocus()
{
    iCameraSetting->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle);
}

void CDigitalCamera::ReserveComplete(TInt aError)
{
    if (aError)
    {
        LOGARG("Camera reserve complete error %d", aError);
        iObserver.CameraReady(aError);
    }
    else
        iCamera->PowerOn();
}

void CDigitalCamera::PowerOnComplete(TInt aError)
{
    TCameraInfo cameraInfo;
    iCamera->CameraInfo(cameraInfo);

    if ((aError == KErrNone) &&
        (cameraInfo.iOptionsSupported & TCameraInfo::EViewFinderBitmapsSupported) &&
        (cameraInfo.iOrientation == TCameraInfo::EOrientationOutwards))
    {
        TSize requestSize(iViewportSize);

        LOGARG("Requesting viewfinder size %d x %d", requestSize.iWidth, requestSize.iHeight);
        TRAPD(error, iCamera->StartViewFinderL(CCamera::EFormatFbsBitmapColor16MU, requestSize));
        LOGARG("Obtaining viewfinder size %d x %d", requestSize.iWidth, requestSize.iHeight);

        iObserver.CameraReady(error);
    }
    else
    {
        // Either power complete error or viewfinder bitmap unsupported 
        iObserver.CameraReady(KErrNotSupported);
    }
}

void CDigitalCamera::ViewFinderFrameReady(CFbsBitmap& aFrame)
{
    iObserver.CameraFrame(aFrame);
}

void CDigitalCamera::ImageReady(CFbsBitmap* /*aBitmap*/, HBufC8* /*aData*/, TInt /*aError*/)
{}

void CDigitalCamera::FrameBufferReady(MFrameBuffer* /*aFrameBuffer*/, TInt /*aError*/)
{}

void CDigitalCamera::HandleEvent(const TECAMEvent& aEvent)
{
    if (aEvent.iEventType == KUidECamEventReserveComplete)
        ReserveComplete(aEvent.iErrorCode);
    else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
        PowerOnComplete(aEvent.iErrorCode);
    else if (aEvent.iEventType == KUidECamEventCameraSettingFocusRange || 
            aEvent.iEventType == KUidECamEventCameraSettingFocusRange2)
        ;
    else if (aEvent.iEventType == KUidECamEventCameraSettingAutoFocusType ||
            aEvent.iEventType == KUidECamEventCameraSettingAutoFocusType2)
        ;
    else if (aEvent.iEventType == KUidECamEventCameraSettingsOptimalFocus)
        iObserver.FocusReady(aEvent.iErrorCode);

    LOGARG("Camera event %x (error %d)", aEvent.iEventType, aEvent.iErrorCode);
}

void CDigitalCamera::ViewFinderReady(MCameraBuffer& aCameraBuffer, TInt /*aError*/)
{
    CFbsBitmap& bitmap = aCameraBuffer.BitmapL(0);
    ViewFinderFrameReady(bitmap);
    aCameraBuffer.Release();
}

void CDigitalCamera::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/, TInt /*aError*/)
{}

void CDigitalCamera::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/, TInt /*aError*/)
{}

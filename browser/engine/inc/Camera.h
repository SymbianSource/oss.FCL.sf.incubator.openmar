/*
 * Name        : Camera.h
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

#ifndef CAMERA_H_
#define CAMERA_H_

#include <ECam.h>

class CFbsBitmap;

/**
 * @brief Wrapper class over CCamera
 * 
 * Implemented to use the minimum frame size 
 */
class CDigitalCamera : public CBase, 
            public MCameraObserver,
            public MCameraObserver2
{
public:
    class MObserver
    {
    public:
        virtual void CameraReady(TInt aError) = 0;
        virtual void CameraFrame(CFbsBitmap& aFrame) = 0;
        virtual void FocusReady(TInt aError) = 0;
    };

public:
    static CDigitalCamera* NewL(MObserver& aObserver);
    ~CDigitalCamera();

protected:
    CDigitalCamera(MObserver& aObserver);
    void ConstructL();

public:
    void Start();
    void Stop();

    void StartFocus();

    const TSize& ViewportSize() const { return iViewportSize; }

protected:
    // From MCameraObserver
    void ReserveComplete(TInt aError);
    void PowerOnComplete(TInt aError);
    void ViewFinderFrameReady(CFbsBitmap& aFrame);
    void ImageReady(CFbsBitmap* aBitmap, HBufC8* aData, TInt aError);
    void FrameBufferReady(MFrameBuffer* aFrameBuffer, TInt aError);

    // From MCameraObserver2
    void HandleEvent(const TECAMEvent& aEvent);
    void ViewFinderReady(MCameraBuffer& aCameraBuffer, TInt aError);
    void ImageBufferReady(MCameraBuffer& aCameraBuffer, TInt aError);
    void VideoBufferReady(MCameraBuffer& aCameraBuffer, TInt aError);

private:
    MObserver& iObserver;
    CCamera* iCamera;
    CCamera::CCameraAdvancedSettings* iCameraSetting;
    TSize iViewportSize;
};

#endif

/*
 * Name        : Overlay.h
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

#ifndef OVERLAY_H_
#define OVERLAY_H_

#include <e32base.h>
#include <GLES/egl.h>

#include "POIOverlay.h"

#include "Matrix4d.h"
#include "Position.h"

class RWindow;
class CWsBitmap;

class CAccelerometer;
class CMagnetometer;
class CAutoRotation;
class CManager;

/**
 * @brief Points of interest overlay
 * 
 * Uses orientation and position to request POIs around the current location 
 */
class COverlay : public OpenMAR::CPOIOverlay,
            public MPositionObserver
{
public:
    static COverlay* NewL(SParameter& aParameter);
    ~COverlay();

protected:
    COverlay(SParameter& aParameter);
    void ConstructL();

public:
    void StartL();
    void Stop();
    const CFbsBitmap& RenderScene();

protected:
    // From MPositionObserver
    void PositionUpdateL(TInt aError, const TPosition& aPosition);

public:
    TInt GetFocusedPOI();

private:
    CPosition* iPosition;
    CAccelerometer* iAccelerometer;
    CMagnetometer* iMagnetometer;
    CAutoRotation* iAutoRotation;

    CManager* iManager;

private:
    RWindow& iWindow;
    TRect iRect;

    EGLDisplay  iEglDisplay;
    EGLSurface  iEglSurface;
    EGLConfig   iConfig;
    EGLContext  iEglContext;

    CWsBitmap*  iPixmap;

private:
    Matrix4d iProjection;
    Matrix4d iModelView;
};

#endif  // OVERLAY_H_

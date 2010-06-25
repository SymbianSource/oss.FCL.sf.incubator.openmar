/*
 * Name        : Overlay.cpp
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

#include "Overlay.h"

#include <coemain.h>
#include <gdi.h>
#include <w32std.h>

#include "Vector3d.h"
#include "Vector4d.h"

#include "Accelerometer.h"
#include "Magnetometer.h"
#include "AutoRotation.h"

#include "Logger.h"

#include "Manager.h"

COverlay* COverlay::NewL(SParameter& aParameter)
{
    COverlay* self = new(ELeave) COverlay(aParameter);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

COverlay::COverlay(SParameter& aParameter)
    : iWindow(aParameter.iWindow), iRect(aParameter.iRect)
{}

void COverlay::ConstructL()
{
    LOGTXT("Initializing EGL..");

    iEglDisplay = ::eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (iEglDisplay == 0)
    {
        _LIT(KGetDisplayFailed, "eglGetDisplay failed");
        User::Panic(KGetDisplayFailed, 0);
    }

    if (::eglInitialize(iEglDisplay, 0, 0) == EGL_FALSE)
    {
        _LIT(KInitializeFailed, "eglInitialize failed");
        User::Panic(KInitializeFailed, 0);
    }

    EGLConfig* configList = 0;
    EGLint configSize     = 0;
    EGLint numOfConfigs   = 0;

    // Get the number of possible EGLConfigs
    if (::eglGetConfigs(iEglDisplay, configList, configSize, &numOfConfigs) == EGL_FALSE)
    {
        _LIT(KGetConfigsFailed, "eglGetConfigs failed");
        User::Panic( KGetConfigsFailed, 0 );
    }

    configSize = numOfConfigs;

    // Allocate memory for the configList
    configList = (EGLConfig*) User::Alloc(sizeof(EGLConfig) * configSize);
    if (configList == 0)
    {
        _LIT(KConfigAllocFailed, "Config alloc failed");
        User::Panic(KConfigAllocFailed, 0);
    }

    /*
     * Define properties for the wanted EGLSurface. To get the best possible 
     * performance, choose an EGLConfig with a buffersize matching the current 
     * window's display mode
     */

    TDisplayMode displayMode = iWindow.DisplayMode();
    TInt bufferSize = 0;

    switch (displayMode)
    {
        case EColor4K:
            bufferSize = 12;
            break;

        case EColor64K:
            bufferSize = 16;
            break;

        case EColor16M:
            bufferSize = 24;
            break;

        case EColor16MU:
        case EColor16MA:
        case EColor16MAP:
            bufferSize = 32;
            break;

        default:
            _LIT(KDisplayModeError, "Unsupported display mode");
            User::Panic(KDisplayModeError, 0);
            break;
    }

    // Define properties for the wanted EGLSurface 
    const EGLint attrib_list[] = { 
            EGL_SURFACE_TYPE,       EGL_PBUFFER_BIT,
//            EGL_TRANSPARENT_TYPE,   EGL_TRANSPARENT_RGB,
            EGL_BUFFER_SIZE,        bufferSize,
            EGL_NONE
    };

    // Choose an EGLConfig that best matches to the properties in attrib_list_fsaa
    if (::eglChooseConfig(iEglDisplay, attrib_list, configList, configSize, &numOfConfigs) == EGL_FALSE)
    {
        _LIT( KChooseConfigFailed, "eglChooseConfig failed");
        User::Panic(KChooseConfigFailed, 0);
    }

    iConfig = configList[0];    // Choose the best EGLConfig. EGLConfigs
                                // returned by eglChooseConfig are sorted so
                                // that the best matching EGLConfig is first in
                                // the list.
    User::Free(configList);

    TInt width  = iRect.Size().iWidth;
    TInt height = iRect.Size().iHeight;

    LOGARG("Window size is %d x %d", width, height);

    const EGLint attrib_list2[] = { 
            EGL_WIDTH,  width,
            EGL_HEIGHT, height,
            EGL_NONE
    };

    // Create a window where the graphics are blitted
    iEglSurface = ::eglCreatePbufferSurface(iEglDisplay, iConfig, attrib_list2);

    if (iEglSurface == 0)
    {
        _LIT(KCreateWindowSurfaceFailed, "eglCreateWindowSurface failed");
        User::Panic(KCreateWindowSurfaceFailed, 0);
    }

    // Create a rendering context
    iEglContext = ::eglCreateContext(iEglDisplay, iConfig, EGL_NO_CONTEXT, 0);

    if (iEglContext == 0)
    {
        _LIT(KCreateContextFailed, "eglCreateContext failed");
        User::Panic(KCreateContextFailed, 0);
    }

    // Make the context current. Binds context to the current rendering thread and surface.
    if (::eglMakeCurrent(iEglDisplay, iEglSurface, iEglSurface, iEglContext) == EGL_FALSE)
    {
        _LIT(KMakeCurrentFailed, "eglMakeCurrent failed");
        User::Panic(KMakeCurrentFailed, 0);
    }

    // Create a Symbian bitmap where the graphics from the Pbuffer are copied
    iPixmap = new(ELeave) CWsBitmap(CCoeEnv::Static()->WsSession());
    iPixmap->Create(iRect.Size(), iWindow.DisplayMode());

    // Manager is in charge of POIs from different providers
    iManager = CManager::NewL();
}

COverlay::~COverlay()
{
    delete iManager;

    delete iPixmap;
    iPixmap = 0;

    ::eglMakeCurrent(iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    ::eglDestroyContext(iEglDisplay, iEglContext);
    ::eglDestroySurface(iEglDisplay, iEglSurface);
    ::eglTerminate(iEglDisplay);
}

void COverlay::StartL()
{
    // First off we enable the position and orientation sensors
    iPosition = CPosition::NewL(*this);
    iPosition->Request();

#if defined(__MARM__)
    iAccelerometer = CAccelerometer::NewL();
    iAccelerometer->StartL();

    iMagnetometer = CMagnetometer::NewL();
    iMagnetometer->StartL();

    iAutoRotation = CAutoRotation::NewL();
    iAutoRotation->ResetL();
#endif

    // We now calculate the view frustum and create the appropriate projection matrix
    TReal near = 1.0f;
    TReal far  = 3000.0f;

    TReal fovy = 0;
    Math::Tan(fovy, 45 * KDegToRad / 2);

    TInt width  = iWindow.Size().iWidth;
    TInt height = iWindow.Size().iHeight;
    TReal aspectRatio = static_cast<TReal>(width) / height;

    TReal top    = near * fovy;
    TReal bottom = -top;
    TReal left   = bottom * aspectRatio;
    TReal right  = top * aspectRatio;

    iProjection.Load(
            2 * near / (right - left),       0,                               0,                            0,
            0                        ,       2 * near / (top - bottom),       0,                            0,
            (right + left) / (right - left), (top + bottom) / (top - bottom), -(far + near) / (far - near), -1,
            0                              , 0                              , - 2 * far * near / (far - near), 0
    );

    ::glViewport(0, 0, width, height);
    ::glMatrixMode(GL_PROJECTION);
    ::glLoadMatrixf(iProjection.m);

    ::glMatrixMode(GL_MODELVIEW);

    ::glEnableClientState(GL_VERTEX_ARRAY);
    ::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//    ::glEnableClientState(GL_NORMAL_ARRAY);

    ::glEnable(GL_DEPTH_TEST);
    ::glDepthFunc(GL_LESS);

    // Set background transparency 
    ::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void COverlay::Stop()
{
    ::glDisable(GL_DEPTH_TEST);

//    ::glDisableClientState(GL_NORMAL_ARRAY);
    ::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    ::glDisableClientState(GL_VERTEX_ARRAY);

#if defined(__MARM__)
    iAutoRotation->RestoreL();
    delete iAutoRotation;
    iAutoRotation = 0;

    iMagnetometer->Stop();
    delete iMagnetometer;
    iMagnetometer = 0;

    iAccelerometer->Stop();
    delete iAccelerometer;
    iAccelerometer = 0;
#endif

    delete iPosition;
    iPosition = 0;
}

const CFbsBitmap& COverlay::RenderScene()
{
    // Estimate orientation based on sensor data
#if defined(__MARM__)
/*
 * +X is defined as the cross product Y.Z (it is tangential to
 * the ground at the device's current location and roughly points East)
 * +Y is tangential to the ground at the device's current location and
 * points towards the magnetic North Pole
 * +Z points towards the sky and is perpendicular to the ground
 */
    Vector3d A = iAccelerometer->GetValue();
    Vector3d E = iMagnetometer->GetValue();

    Vector3d H = Vector3d::Cross(E, A);
    Scalar hNorm = H.Norm();
    H.mX /= hNorm;
    H.mY /= hNorm;
    H.mZ /= hNorm;

    Scalar aNorm = A.Norm();
    A.mX /= aNorm;
    A.mY /= aNorm;
    A.mZ /= aNorm;

    Vector3d M = Vector3d::Cross(A, H);

    iModelView.Load(
            H.mX, H.mY, H.mZ, 0,
            M.mX, M.mY, M.mZ, 0,
            A.mX, A.mY, A.mZ, 0,
            0, 0, 0, 1
    );
    ::glLoadMatrixf(iModelView.m);
#else
    ::glLoadIdentity();
#endif

    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    iManager->Render();

    ::eglCopyBuffers(iEglDisplay, iEglSurface, iPixmap);

    return *iPixmap;
}

/*
 * Callback from position provider. We use current position to set up world's origin
 * and make request to different providers
 */
void COverlay::PositionUpdateL(TInt aError, const TPosition& aPosition)
{
    LOGARG("Got position: lat=%f,lon=%f,alt=%f", aPosition.Latitude(), aPosition.Longitude(), aPosition.Altitude());

    // Set the "world" origin
    iManager->SetOrigin(aPosition);
    // Request POIs for that position
    iManager->RequestL(aPosition);
}

/*
 * From the current list of POIs retrieved, find the one that is closest to the
 * screen center.
 * 
 * We use both projection and modelview transforms to obtain screen coordinates.
 * Note that both matrices are in column-major ordering, so we need to transpose them.
 */
TInt COverlay::GetFocusedPOI()
{
    const TPoint viewportCenter(iRect.Center());

    TInt mostCentered = KMaxTInt; 
    TInt focused = KErrNotFound;

    for (TInt i = 0; i < iManager->iObjectList.Count(); ++i)
    {
        const Vector3d position(iManager->iObjectList[i]->GetPosition());

        const Vector4d world(position.mX, position.mY, position.mZ, 1);
        const Vector4d camera(iModelView.Transpose() * world);
        const Vector4d projection(iProjection.Transpose() * camera);

        // Screen transformation
        TReal x = projection.mX / projection.mW;
        TReal y = projection.mY / projection.mW;
        TReal z = projection.mZ / projection.mW;

        TReal screenX = viewportCenter.iX * (x + 1);
        TReal screenY = viewportCenter.iY * (y + 1);
//        TReal screenZ = (z + 1) / 2;

        TBool visible = (x > -1 && x < 1) && (y > -1 && y < 1) && (z > -1 && z < 1);
        TInt centered = (screenX - viewportCenter.iX) * (screenX - viewportCenter.iX) + (screenY - viewportCenter.iY) * (screenY - viewportCenter.iY);

        if (visible && centered < mostCentered)
        {
            mostCentered = centered; 
            focused = i;
        }
    }

    return focused;
}

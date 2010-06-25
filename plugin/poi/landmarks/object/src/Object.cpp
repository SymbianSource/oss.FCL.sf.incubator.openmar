/*
 * Name        : Object.cpp
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

#include "Object.h"

#include "IconLoader.h"
#include "Vector3d.h"

#include <bitdev.h>
#include <bitstd.h>
#include <fbs.h>
#include <GulIcon.h>
#include <GLES/egl.h>

#include <Landmarks_0x2002E1AF.mbg>

#include "Logger.h"

CLMXObject* CLMXObject::NewL(OpenMAR::CPOIProvider* aProvider)
{
    CLMXObject* self = new(ELeave) CLMXObject(aProvider);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CLMXObject::~CLMXObject()
{
    ::glDeleteTextures(1, &iTextureId);
}

CLMXObject::CLMXObject(OpenMAR::CPOIProvider* aProvider)
    : OpenMAR::CPOIObject(*aProvider)
{}

/*
 * Landmark objects will make use of an SVG file for rendering (demo purposes)
 */
void CLMXObject::ConstructL()
{
    _LIT(KIconFile, "\\resource\\apps\\Landmarks_0x2002E1AF.mif");

    CGulIcon* icon = CreateIconL(KIconFile, EMbmLandmarks_0x2002e1afIcon, EMbmLandmarks_0x2002e1afIcon_mask);
    CleanupStack::PushL(icon);

    CFbsBitmap* bitmap = icon->Bitmap();    // Ownership NOT transferred
    CFbsBitmap* mask   = icon->Mask();      // Ownership NOT transferred

    // Always expect 16M bitmap to make conversion to GL_RGBA easier 
    if (bitmap->DisplayMode() != EColor16M)
    {
        bitmap = new(ELeave) CFbsBitmap;
        CleanupStack::PushL(bitmap);

        User::LeaveIfError(bitmap->Create(icon->Bitmap()->SizeInPixels(), EColor16M));

        CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
        CleanupStack::PushL(bitmapDevice);

        CFbsBitGc* bitmapContext = 0;
        User::LeaveIfError(bitmapDevice->CreateContext(bitmapContext));
        CleanupStack::PushL(bitmapContext);

        bitmapContext->BitBlt(TPoint(0, 0), icon->Bitmap());

        CleanupStack::PopAndDestroy(2, bitmapDevice);

        icon->SetBitmap(bitmap);    // Ownership transferred

        CleanupStack::Pop(bitmap);
    }

    // Always expect 256 mask to make conversion to GL_RGBA easier 
    if (mask->DisplayMode() != EGray256)
    {
        mask = new(ELeave) CFbsBitmap;
        CleanupStack::PushL(mask);

        User::LeaveIfError(mask->Create(icon->Mask()->SizeInPixels(), EGray256));

        CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(mask);
        CleanupStack::PushL(bitmapDevice);

        CFbsBitGc* bitmapContext = 0;
        User::LeaveIfError(bitmapDevice->CreateContext(bitmapContext));
        CleanupStack::PushL(bitmapContext);

        bitmapContext->BitBlt(TPoint(0, 0), icon->Mask());

        CleanupStack::PopAndDestroy(2, bitmapDevice);

        icon->SetMask(mask);    // Ownership transferred

        CleanupStack::Pop(mask);
    }

    // Now bitmap and mask point to either original or converted bitmaps, 
    // and ownership belongs to icon

    const TSize bitmapSize = bitmap->SizeInPixels();

    // sizeof(TUint32) == sizeof(RGBA)
    const TInt dataSize = bitmapSize.iWidth * bitmapSize.iHeight * sizeof(TUint32);
    TUint8* data = new(ELeave) TUint8[dataSize];

    // Perform copy and conversion from BGR(A) to RGB(A)
    bitmap->LockHeap();
    mask->LockHeap();

    // TODO: Alpha component removed, as it seems to be corrupted from
    // subsequent reads from SVG file

    TUint8* rgb = reinterpret_cast<TUint8*>(bitmap->DataAddress());
//    TUint8* alpha = reinterpret_cast<TUint8*>(mask->DataAddress());

    for(TInt i = 0, j = 0; i < dataSize; i += 4, j += 3)
    {
        data[i + 0] = rgb[j + 2];
        data[i + 1] = rgb[j + 1];
        data[i + 2] = rgb[j + 0];
        data[i + 3] = 0xc0; //alpha[i / 4];
    }

    // Generate OpenGL texture
    ::glGenTextures(1, &iTextureId); 
    ::glBindTexture(GL_TEXTURE_2D, iTextureId);

    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmapSize.iWidth, bitmapSize.iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    mask->UnlockHeap();
    bitmap->UnlockHeap();

    delete data;

    CleanupStack::PopAndDestroy(icon);
}

void CLMXObject::Render() const
{
    /*
     * This is a special case for points or planes in space. Given that a usual POI
     * does not provide of any transformation to be applied on it, we determine the
     * angle in the XY plane with respect to the observer and apply that rotation to 
     * the object.
     *
     * This way, all POIs will face the observer regardless their position. Other
     * objects (3D Landmarks, etc) should provide their own transformations in some
     * proper way.   
     */

    TReal angle = 0;
    Math::ATan(angle, iPosition.mX, iPosition.mY);
    angle *= -KRadToDeg;

    /*
     * We could also apply some scaling calculation to compensate the size of
     * objects projected at a long distance from the camera.
     * For now we just use a constant value
     */

    TReal scale = 64.0f;

    // Apply transformations to the object and render it

    const TInt unity = 1 << 16;

    GLfixed vertices[] = {
        -unity / 2, 0, 0,
         unity / 2, 0, 0,
        -unity / 2, 0, unity,
         unity / 2, 0, unity,
    };

    GLfixed texCoords[] = {
        0,      unity,
        unity,  unity,
        0,      0,
        unity,  0
    };

    ::glPushMatrix();

    ::glTranslatef(iPosition.mX, iPosition.mY, iPosition.mZ);
    ::glRotatef(angle, 0, 0, 1);
    ::glScalef(scale, 1.0f, scale);

    ::glEnable(GL_TEXTURE_2D);
    ::glBindTexture(GL_TEXTURE_2D, iTextureId);
    ::glVertexPointer(3, GL_FIXED, 0, vertices);
    ::glTexCoordPointer(2, GL_FIXED, 0, texCoords);
    ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ::glDisable(GL_TEXTURE_2D);

    ::glPopMatrix();
}

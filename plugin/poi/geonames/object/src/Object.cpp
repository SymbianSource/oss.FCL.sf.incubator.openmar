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

#include "Vector3d.h"

#include <AknUtils.h>
#include <bitdev.h>
#include <bitstd.h>
#include <fbs.h>
#include <GLES/egl.h>

#include "Logger.h"

Geonames::CObject* Geonames::CObject::NewL(OpenMAR::CPOIProvider* aProvider)
{
    CObject* self = new(ELeave) Geonames::CObject(aProvider);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

Geonames::CObject::~CObject()
{
    ::glDeleteTextures(1, &iTextureId);
}

Geonames::CObject::CObject(OpenMAR::CPOIProvider* aProvider)
    : OpenMAR::CPOIObject(*aProvider)
{}

/*
 * Create texture to represent the object
 */
void Geonames::CObject::ConstructL()
{
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);

    // Use a 64x64 bitmap
    const TSize bitmapSize(64, 64);
    User::LeaveIfError(bitmap->Create(bitmapSize, EColor16MA));

    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
    CleanupStack::PushL(bitmapDevice);

    CFbsBitGc* bitmapContext = 0;
    User::LeaveIfError(bitmapDevice->CreateContext(bitmapContext));
    CleanupStack::PushL(bitmapContext);

    // Define colors to use
    bitmapContext->SetBrushColor(KRgbBlue);
    bitmapContext->SetBrushStyle(CFbsBitGc::ESolidBrush);

    bitmapContext->SetPenColor(KRgbRed);
    bitmapContext->SetPenStyle(CFbsBitGc::ESolidPen);

    const CFont* font = AknLayoutUtils::FontFromId(ELatinPlain12, 0);
    bitmapContext->UseFont(font);

    bitmapContext->Clear();

    const TRect bitmapRect(TPoint(0, 0), bitmapSize);
    bitmapContext->DrawRect(bitmapRect);

    // Set some text if needed
    _LIT(KTitle, "Test");
    TPoint titlePos(font->MaxNormalCharWidthInPixels(), font->HeightInPixels());
    bitmapContext->DrawText(KTitle(), titlePos);

    // Convert to GL_RGBA
    const TInt dataSize = bitmapSize.iWidth * bitmapSize.iHeight * sizeof(TUint32);
    TUint8* data = new(ELeave) TUint8[dataSize];

    bitmap->LockHeap();

    TUint8* rgb = reinterpret_cast<TUint8*>(bitmap->DataAddress());

    for(TInt i = 0; i < dataSize; i += 4)
    {
        data[i + 0] = rgb[i + 2];
        data[i + 1] = rgb[i + 1];
        data[i + 2] = rgb[i + 0];
        data[i + 3] = rgb[i + 3];
    }

    ::glGenTextures(1, &iTextureId); 
    ::glBindTexture(GL_TEXTURE_2D, iTextureId);

    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmapSize.iWidth, bitmapSize.iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    bitmap->UnlockHeap();

    delete data;

    CleanupStack::PopAndDestroy(3, bitmap);
}

void Geonames::CObject::Render() const
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

    TReal scale = 128.0f;

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

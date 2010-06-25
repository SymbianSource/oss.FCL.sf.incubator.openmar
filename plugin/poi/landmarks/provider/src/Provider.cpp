/*
 * Name        : Provider.cpp
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

#include "Provider.h"

#include <Hash.h>
#include <Uri8.h>

#include "Logger.h"

/*
 * Generate a unique id using coordinate and name as seeds
 * 
 */

TUint32 GenerateIdL(const CPosLandmark& aLandmark)
{
    TPtrC name;
    aLandmark.GetLandmarkName(name);

    TLocality coordinate;
    aLandmark.GetPosition(coordinate);

    HBufC8* name8 = HBufC8::NewLC(name.Length());
    name8->Des().Copy(name);

    TRealFormat realFormat;
    realFormat.iPoint  = TChar('.');    // Override TLocale::DecimalSeparator() as separator
    realFormat.iWidth  = 8;             // Set max width of number
    realFormat.iPlaces = 5;             // Set max width of decimal portion

    TBuf8<KDefaultRealWidth> latitude;
    latitude.Num(coordinate.Latitude(), realFormat);

    TBuf8<KDefaultRealWidth> longitude;
    longitude.Num(coordinate.Longitude(), realFormat);

    CSHA1* hash = CSHA1::NewL();
    CleanupStack::PushL(hash);
    hash->Update(*name8);
    hash->Update(latitude);
    TPtrC8 result = hash->Final(longitude);

    const TUint32* ptr = reinterpret_cast<const TUint32*>(result.Ptr());
    TUint32 value = *ptr;

    CleanupStack::PopAndDestroy(2, name8);  // hash, name8

    return value;
}


CProvider* CProvider::NewL(const TDesC8& aProviderUri)
{
    CProvider* self = new(ELeave) CProvider;
    CleanupStack::PushL(self);
    self->ConstructL(aProviderUri);
    CleanupStack::Pop(self);

    return self;
}

CProvider::~CProvider()
{
    delete iLoader;

    ReleaseLandmarkResources();
}

CProvider::CProvider()
{}

void CProvider::ConstructL(const TDesC8& aProviderUri)
{
    TUriParser8 uri;
    uri.Parse(aProviderUri);

    _LIT8(KHttpScheme, "http");

    if (uri.IsSchemeValid() && uri.Extract(EUriScheme).Compare(KHttpScheme) == 0)
        iLoader = CRemoteLoader::NewL(*this, aProviderUri);
    else
        iLoader = CLocalLoader::NewL(*this, aProviderUri);
}

void CProvider::LandmarkLoaderOpenedL(TInt aError)
{
    if (iObserver)
        iObserver->POIProviderLoadedL(this, aError);
}

void CProvider::LandmarkLoaderItemCreatedL(const CPosLandmark& aLandmark)
{
    if (iObserver)
    {
        TPosLmItemId id = aLandmark.LandmarkId();

        if (id == KPosLmNullItemId)
            id = GenerateIdL(aLandmark);

        _LIT8(KObject, "lmx");
        OpenMAR::CPOIObject* object = OpenMAR::CPOIObject::NewL(KObject, *this);
        CleanupStack::PushL(object);

        object->SetIdentifier(id);

        TLocality locality;
        aLandmark.GetPosition(locality);
        object->SetCoordinate(locality);

        iObserver->POIObjectCreatedL(object);   // Transfers ownership

        CleanupStack::Pop(object);
    }
}

void CProvider::LandmarkLoaderItemUpdatedL(const CPosLandmark& aLandmark)
{
}

void CProvider::LandmarkLoaderItemDeletedL(const CPosLandmark& aLandmark)
{
}

void CProvider::RetrieveL(const TCoordinate& aCoordinate, TReal32 aRadius)
{
    LOGARG("[LANDMARKS] Retrieving landmarks from %f %f %f", aCoordinate.Latitude(), aCoordinate.Longitude(), aCoordinate.Altitude());

    iLoader->RequestL(aCoordinate, aRadius);
}

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
 * Generates a unique id using name and coordinates as a seed
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

    // Use only 32 bits from hash
    const TUint32* ptr = reinterpret_cast<const TUint32*>(result.Ptr());
    TUint32 value = *ptr;

    CleanupStack::PopAndDestroy(2, name8);  // hash, name8

    return value;
}

// Create instance of concrete ECOM interface implementation
CProvider* CProvider::NewL()
{
    CProvider* self = new(ELeave) CProvider();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CProvider::~CProvider()
{
    delete iLoader;
}

CProvider::CProvider()
{
}

void CProvider::ConstructL()
{
    iLoader = CLoader::NewL(*this);
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

        _LIT8(KObject, "geonames");
        OpenMAR::CPOIObject* object = OpenMAR::CPOIObject::NewL(KObject, *this);
        CleanupStack::PushL(object);

        object->SetIdentifier(id);

        TLocality locality;
        aLandmark.GetPosition(locality);
        object->SetCoordinate(locality);

        TPtrC name;
        aLandmark.GetLandmarkName(name);
        object->SetNameL(name);

        iObserver->POIObjectCreatedL(object);   // Transfers ownership

        CleanupStack::Pop(object);
    }
}

void CProvider::RetrieveL(const TCoordinate& aCoordinate, TReal32 aRadius)
{
    LOGARG("[GEONAMES] Retrieving entries from %f %f %f", aCoordinate.Latitude(), aCoordinate.Longitude(), aCoordinate.Altitude());

    iLoader->RequestL(aCoordinate, aRadius);
}

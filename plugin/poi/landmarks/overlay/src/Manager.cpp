/*
 * Name        : Manager.cpp
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

#include "Manager.h"

#include <e32math.h>

#include "Logger.h"

CManager* CManager::NewL()
{
    CManager* self = new(ELeave) CManager;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CManager::~CManager()
{
    iObjectList.ResetAndDestroy();
    iObjectList.Close();

    iProviderList.ResetAndDestroy();
    iProviderList.Close();
}

CManager::CManager()
{}

void CManager::ConstructL()
{
    _LIT8(KUri, "");

    _LIT8(KLMXProvider, "lmx");
    OpenMAR::CPOIProvider* lmx = OpenMAR::CPOIProvider::NewL(KLMXProvider, KUri);
    lmx->SetObserver(this);
    iProviderList.AppendL(lmx);

    _LIT8(KGeonamesProvider, "geonames");
    OpenMAR::CPOIProvider* geonames = OpenMAR::CPOIProvider::NewL(KGeonamesProvider, KUri);
    geonames->SetObserver(this);
    iProviderList.AppendL(geonames);
}

void CManager::POIProviderLoadedL(OpenMAR::CPOIProvider* aProvider, TInt aError)
{
    LOGARG("Landmark resource %x loaded with error %d", aProvider, aError);

    // Handle error in some way
}

void CManager::POIObjectCreatedL(OpenMAR::CPOIObject* aPOIObject)
{
    CleanupStack::PushL(aPOIObject);

    // Check if this POI already exists
    TInt index = iObjectList.Find(aPOIObject, OpenMAR::CPOIObject::IdentityRelation);

    if (index == KErrNotFound)
    {
        // Set world position
        TCoordinate coordinate = aPOIObject->GetCoordinate();
        aPOIObject->SetPosition(Transform(coordinate));

        LOGARG("Appending new POI (%d)", aPOIObject->GetIdentifier());

        iObjectList.AppendL(aPOIObject);

        CleanupStack::Pop(aPOIObject);
    }
    else
    {
        LOGARG("POI already exists (%d). Destroying..", aPOIObject->GetIdentifier());

        CleanupStack::PopAndDestroy(aPOIObject);
    }
}

void CManager::POIObjectUpdatedL(OpenMAR::CPOIObject* aPOIObject)
{}

/**
 * @brief Transform between WGS84 to Euclidean space
 * 
 * Our Euclidean space will be determined by:
 * +X : tangential to the ground and pointing to the EAST
 * +Y : tangential to the ground and pointing to the NORTH
 * +Z : perpendicular to the ground (pointing to the sky)
 */
const Vector3d CManager::Transform(const TCoordinate& aCoordinate)
{
    TReal32 distance = 0;
    iOrigin.Distance(aCoordinate, distance);

    TReal32 bearing = 0;
    iOrigin.BearingTo(aCoordinate, bearing);

    TReal32 angle = bearing * KDegToRad;

    TReal cos = 0;
    TReal sin = 0;
    Math::Cos(cos, angle);
    Math::Sin(sin, angle);

    // X = sin(bearing) * d = cos(90 - bearing) * d = sin(bearing) * d
    // Y = cos(bearing) * d = sin(90 - bearing) * d = cos(bearing) * d

    TReal x = sin * distance;
    TReal y = cos * distance;
    TReal z = aCoordinate.Altitude();

    return Vector3d(x, y, z);
}

void CManager::SetOrigin(const TCoordinate& aCoordinate)
{
    iOrigin = aCoordinate;
}

void CManager::RequestL(const TCoordinate& aCoordinate)
{
    const TReal32 radius = 3000;

    for (TInt i = 0; i < iProviderList.Count(); ++i)
        iProviderList[i]->RetrieveL(aCoordinate, radius);
}

void CManager::Render() const
{
    for (TInt i = 0; i < iObjectList.Count(); ++i)
        iObjectList[i]->Render();
}

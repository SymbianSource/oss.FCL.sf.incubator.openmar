/*
 * Name        : POIProvider.h
 * Description : ECOM interface definition
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

#ifndef POIPROVIDER_H_
#define POIPROVIDER_H_

#include "InterfaceUid.hrh"

#include <ecom/ecom.h>
#include <LbsPosition.h>

namespace OpenMAR {

class CPOIProvider;
class CPOIObject;

/**
 * @brief POI provider notifier
 */
class MPOIProviderObserver
{
public:
    virtual void POIProviderLoadedL(CPOIProvider* aProvider, TInt aError) = 0;
    virtual void POIObjectCreatedL(CPOIObject* aPOIObject) = 0;
    virtual void POIObjectUpdatedL(CPOIObject* aPOIObject) = 0;
};

/**
 * @brief POI provider interface definition
 * 
 * Retrieves geolocated points of interest from a local or remote resource, 
 * creating corresponding CPosLandmark objects
 */
class CPOIProvider : public CBase
{
public:
    static CPOIProvider* NewL(const TDesC8& aMatch, const TDesC8& aProviderUri);
    static CPOIProvider* NewL(const TUid aImplementationUid, const TDesC8& aProviderUri);
    ~CPOIProvider();

    void SetObserver(MPOIProviderObserver* aObserver);

    virtual void RetrieveL(const TCoordinate& aCoordinate, TReal32 aRadius) = 0;

protected:
    MPOIProviderObserver* iObserver;

private:
    TUid iDtor_ID_Key;
};

#include "POIProvider.inl"

}

#endif  // POIPROVIDER_H_

/*
 * Name        : Manager.h
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

#ifndef MANAGER_H_
#define MANAGER_H_

#include "POIProvider.h"
#include "POIObject.h"

#include "Vector3d.h"

/**
 * @brief Manages points of interest
 * 
 * The manager connects to a group of POI providers and manages a list of POIs
 * A caching and filtering mechanism could be implemented
 */
class CManager : public CBase, public OpenMAR::MPOIProviderObserver
{
    friend class COverlay;

public:
    static CManager* NewL();
    ~CManager();

protected:
    CManager();
    void ConstructL();

protected:
    // From OpenMAR::MPOIProviderObserver
    void POIProviderLoadedL(OpenMAR::CPOIProvider* aProvider, TInt aError);
    void POIObjectCreatedL(OpenMAR::CPOIObject* aPOIObject);
    void POIObjectUpdatedL(OpenMAR::CPOIObject* aPOIObject);

protected:
    const Vector3d Transform(const TCoordinate& aCoordinate);

public:
    void SetOrigin(const TCoordinate& aCoordinate);

    void RequestL(const TCoordinate& aCoordinate);
    void Render() const;

private:
    RPointerArray<OpenMAR::CPOIProvider> iProviderList;

    TCoordinate iOrigin;
    RPointerArray<OpenMAR::CPOIObject> iObjectList;
};

#endif

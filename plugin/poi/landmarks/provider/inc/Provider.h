/*
 * Name        : Provider.h
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

#ifndef PROVIDER_H_
#define PROVIDER_H_

#include "POIProvider.h"
#include "POIObject.h"

#include "Loader.h"
#include "Local.h"
#include "Remote.h"

/**
 * @brief Provider implementation for local and remote landmarks (lmx)
 */
class CProvider : public OpenMAR::CPOIProvider, public MLandmarkLoaderObserver
{
public:
    static CProvider* NewL(const TDesC8& aProviderUri);
    ~CProvider();

protected:
    CProvider();
    void ConstructL(const TDesC8& aProviderUri);

    // From MLandmarkLoaderObserver
    void LandmarkLoaderOpenedL(TInt aError);
    void LandmarkLoaderItemCreatedL(const CPosLandmark& aLandmark);
    void LandmarkLoaderItemUpdatedL(const CPosLandmark& aLandmark);
    void LandmarkLoaderItemDeletedL(const CPosLandmark& aLandmark);

public:
    void RetrieveL(const TCoordinate& aCoordinate, TReal32 aRadius);

private:
    MLandmarkLoader* iLoader;
};

#endif  // PROVIDER_H_

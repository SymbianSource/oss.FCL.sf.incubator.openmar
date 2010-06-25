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

class CProvider : public OpenMAR::CPOIProvider, public MLandmarkLoaderObserver
{
public:
    static CProvider* NewL();
    ~CProvider();

protected:
    CProvider();
    void ConstructL();

    // From MLandmarkLoaderObserver
    void LandmarkLoaderOpenedL(TInt aError);
    void LandmarkLoaderItemCreatedL(const CPosLandmark& aLandmark);

public:
    void RetrieveL(const TCoordinate& aCoordinate, TReal32 aRadius);

private:
    CLoader* iLoader;
};

#endif

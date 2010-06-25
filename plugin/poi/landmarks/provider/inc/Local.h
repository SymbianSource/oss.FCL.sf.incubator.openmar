/*
 * Name        : Local.h
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

#ifndef LOCAL_H_
#define LOCAL_H_

#include <e32base.h>
#include <EPos_Landmarks.h>

#include "Loader.h"

class CPosLandmarkDatabase;
class TCoordinate;

/**
 * @brief Retrieves landmarks from a local database
 */
class CLocalLoader : public CActive, public MLandmarkLoader
{
public:
    static CLocalLoader* NewL(MLandmarkLoaderObserver& aObserver, const TDesC8& aProviderUri);
    ~CLocalLoader();

protected:
    CLocalLoader(MLandmarkLoaderObserver& aObserver);
    void ConstructL(const TDesC8& aProviderUri);

    // From CActive
    void RunL();
    void DoCancel();

public:
    // From MLandmarkLoader
    void RequestL(const TCoordinate& aCoordinate, TReal32 aRadius);

private:
    MLandmarkLoaderObserver& iObserver;

    CPosLandmarkDatabase* iLandmarkDb;
    TPosLmEvent iEvent;
};

#endif  // LOCAL_H_

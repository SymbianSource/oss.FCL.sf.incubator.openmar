/*
 * Name        : Remote.h
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

#ifndef REMOTE_H_
#define REMOTE_H_

#include <e32base.h>
#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkParser.h>
#include <http/MHttpTransactionCallback.h>

#include "Loader.h"

class TCoordinate;
class CHttpClient;

/**
 * @brief Retrieves landmarks from a remote database
 */

class CRemoteLoader : public CActive, public MLandmarkLoader, public MHTTPTransactionCallback
{
public:
    static CRemoteLoader* NewL(MLandmarkLoaderObserver& aObserver, const TDesC8& aProviderUri);
    ~CRemoteLoader();

protected:
    CRemoteLoader(MLandmarkLoaderObserver& aObserver);
    void ConstructL(const TDesC8& aProviderUri);

    void RunL();
    void DoCancel();

public:
    // From MLandmarkLoader
    void RequestL(const TCoordinate& aCoordinate, TReal32 aRadius);

protected:
    // From MHTTPTransactionCallback
    void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
    TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

private:
    MLandmarkLoaderObserver& iObserver;

    HBufC8* iBaseUri;
    CHttpClient* iClient;
    RBuf8 iBuffer;

    CPosLandmarkParser* iParser;
    CPosLmOperation* iOperation;
    TReal32 iProgress;
};

#endif  // REMOTE_H_

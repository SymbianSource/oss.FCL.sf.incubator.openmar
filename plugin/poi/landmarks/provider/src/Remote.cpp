/*
 * Name        : Remote.cpp
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

#include "Remote.h"

#include "HttpClient.h"

#include <AknUtils.h>
#include <lbspositioninfo.h>
#include <http/mhttpdatasupplier.h>
#include <HttpErr.h>
#include <utf.h>

#include "Logger.h"

CRemoteLoader* CRemoteLoader::NewL(MLandmarkLoaderObserver& aObserver, const TDesC8& aProviderUri)
{
    CRemoteLoader* self = new(ELeave) CRemoteLoader(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aProviderUri);
    CleanupStack::Pop(self);

    return self;
}

CRemoteLoader::~CRemoteLoader()
{
    iBuffer.Close();

    delete iParser;
    delete iClient;
    delete iBaseUri;
}

CRemoteLoader::CRemoteLoader(MLandmarkLoaderObserver& aObserver)
    : CActive(CActive::EPriorityStandard), iObserver(aObserver)
{
    CActiveScheduler::Add(this);
}

void CRemoteLoader::ConstructL(const TDesC8& aProviderUri)
{
    LOGARG("[LANDMARKS] Creating remote landmark loader: %S", &aProviderUri);

    iBaseUri = aProviderUri.AllocL();
    iClient = CHttpClient::NewL(*this);

    _LIT8(KMimeType, "application/vnd.nokia.landmarkcollection+xml");
    iParser = CPosLandmarkParser::NewL(KMimeType);
}

void CRemoteLoader::RunL()
{
    if (iStatus == KPosLmOperationNotComplete || iStatus == KErrNone)
    {
        CPosLandmark* landmark = iParser->LandmarkLC();
        iObserver.LandmarkLoaderItemCreatedL(*landmark);
        CleanupStack::PopAndDestroy(landmark);
    }

    if (iStatus == KPosLmOperationNotComplete)
    {
        iOperation->NextStep(iStatus, iProgress);
        SetActive();
    }
}

void CRemoteLoader::DoCancel()
{
    // TODO: Add cancellation code
}

void CRemoteLoader::RequestL(const TCoordinate& aCoordinate, TReal32 aRadius)
{
    TRealFormat realFormat;
    realFormat.iPoint  = TChar('.');    // Override TLocale::DecimalSeparator() as separator
    realFormat.iWidth  = 8;             // Set max width of number
    realFormat.iPlaces = 5;             // Set max width of decimal portion

    TBuf8<KDefaultRealWidth> latitude;
    latitude.Num(aCoordinate.Latitude(), realFormat);

    TBuf8<KDefaultRealWidth> longitude;
    longitude.Num(aCoordinate.Longitude(), realFormat);

    // Convert to integer
    TInt radius = static_cast<TInt>(aRadius);

    TBuf8<256> uri(*iBaseUri);
    _LIT8(KParam, "?lat=%S&lon=%S&radius=%d");
    uri.AppendFormat(KParam, &latitude, &longitude, radius);

    LOGARG("[LANDMARKS] Making http request %S", &uri);

    iClient->GetL(uri);
}

void CRemoteLoader::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
{
    switch (aEvent.iStatus)
    {
        case THTTPEvent::EGotResponseHeaders:
        {
            RHTTPResponse resp = aTransaction.Response();
            TInt status = resp.StatusCode();

            LOGARG("[LANDMARKS] HTTP response status %d", status);

            break;
        }

        case THTTPEvent::EGotResponseBodyData:
        {
            MHTTPDataSupplier* body = aTransaction.Response().Body();

            TPtrC8 dataChunk;
            body->GetNextDataPart(dataChunk);

            // Check if there is enough remaining space
            const TInt newLength = iBuffer.Length() + dataChunk.Length();
            if (iBuffer.MaxLength() < newLength)
                iBuffer.ReAllocL(newLength);
            iBuffer.Append(dataChunk);

            body->ReleaseData();

            break;
        }

        case THTTPEvent::EResponseComplete:
            LOGTXT("[LANDMARKS] HTTP response complete");

            break ;

        case THTTPEvent::ESucceeded:
        {
            RHTTPResponse resp = aTransaction.Response();
            TInt status = resp.StatusCode();

            LOGARG("[LANDMARKS] HTTP response succeeded %d", status);

            aTransaction.Close();

            iObserver.LandmarkLoaderOpenedL(KErrNone);
            
            iParser->SetInputBuffer(iBuffer);

            iOperation = iParser->ParseContentL();

            iOperation->NextStep(iStatus, iProgress);
            SetActive();

            break;
        }

        case THTTPEvent::EFailed:
        {
            RHTTPResponse resp = aTransaction.Response();
            TInt status = resp.StatusCode();

            LOGARG("[LANDMARKS] HTTP response failed %d", status);

            aTransaction.Close();

            iObserver.LandmarkLoaderOpenedL(KErrCouldNotConnect);
            break;
        }

        default:
            LOGARG("[LANDMARKS] HTTP unknown event %d", aEvent.iStatus);

            aTransaction.Close();

            iObserver.LandmarkLoaderOpenedL(KErrUnknown);
            break;
    }
}

TInt CRemoteLoader::MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
{
    iObserver.LandmarkLoaderOpenedL(aError);

    return KErrNone;
}

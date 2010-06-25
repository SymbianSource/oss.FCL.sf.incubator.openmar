/*
 * Name        : Local.cpp
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

#include "Local.h"

#include <EPos_CPosLandmarkDatabase.h>
#include <LbsPositionInfo.h>

#include "Logger.h"

CLocalLoader* CLocalLoader::NewL(MLandmarkLoaderObserver& aObserver, const TDesC8& aProviderUri)
{
    CLocalLoader* self = new(ELeave) CLocalLoader(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aProviderUri);
    CleanupStack::Pop(self);

    return self;
}

CLocalLoader::~CLocalLoader()
{
    Cancel();

    delete iLandmarkDb;
}

CLocalLoader::CLocalLoader(MLandmarkLoaderObserver& aObserver)
    : CActive(CActive::EPriorityStandard), iObserver(aObserver)
{
    CActiveScheduler::Add(this);
}

void CLocalLoader::ConstructL(const TDesC8& aProviderUri)
{
    LOGARG("[LANDMARKS] Creating local landmark loader: %S", &aProviderUri);

    HBufC* uri = HBufC::NewLC(aProviderUri.Length());
    uri->Des().Copy(aProviderUri);

//    iLandmarkDb = CPosLandmarkDatabase::OpenL(*uri);
    iLandmarkDb = CPosLandmarkDatabase::OpenL();

    CleanupStack::PopAndDestroy(uri);

    CPosLmOperation* initialization = iLandmarkDb->InitializeL();
    ExecuteAndDeleteLD(initialization);

    iLandmarkDb->NotifyDatabaseEvent(iEvent, iStatus);
    SetActive();
}

void CLocalLoader::RunL()
{
    LOGARG("[LANDMARKS] Landmark event %d", iEvent.iEventType);

    if (iStatus == KErrNone)
        switch (iEvent.iEventType)
        {
            case EPosLmEventLandmarkCreated:
            {
                LOGTXT("[LANDMARKS] Got new landmark. Notifying observer");

                CPosLandmark* landmark = iLandmarkDb->ReadLandmarkLC(iEvent.iLandmarkItemId);
                iObserver.LandmarkLoaderItemCreatedL(*landmark);
                CleanupStack::PopAndDestroy(landmark);

                break;
            }

            case EPosLmEventLandmarkUpdated:
            {
                LOGTXT("[LANDMARKS] Got updated landmark. Notifying observer");

                CPosLandmark* landmark = iLandmarkDb->ReadLandmarkLC(iEvent.iLandmarkItemId);
                iObserver.LandmarkLoaderItemUpdatedL(*landmark);
                CleanupStack::PopAndDestroy(landmark);

                break;
            }

            case EPosLmEventLandmarkDeleted:
            {
                LOGTXT("[LANDMARKS] Landmark was deleted. Notifying observer");

                CPosLandmark* landmark = CPosLandmark::NewLC();
//                landmark->SetLandmarkIdL(iEvent.iLandmarkItemId);
                iObserver.LandmarkLoaderItemDeletedL(*landmark);
                CleanupStack::PopAndDestroy(landmark);

                break;
            }

            case EPosLmEventNewDefaultDatabaseLocation:
            case EPosLmEventMediaRemoved:
            default:
                break;
        }

    iLandmarkDb->NotifyDatabaseEvent(iEvent, iStatus);
    SetActive();
}

void CLocalLoader::DoCancel()
{
    iLandmarkDb->CancelNotifyDatabaseEvent();
}

void CLocalLoader::RequestL(const TCoordinate& aCoordinate, TReal32 aRadius)
{
    iObserver.LandmarkLoaderOpenedL(KErrNone);

    // TODO: Should use CPosLmAreaCriteria to filter

    CPosLmItemIterator* landmarkIterator = iLandmarkDb->LandmarkIteratorL();
    CleanupStack::PushL(landmarkIterator);

    for (TPosLmItemId id = landmarkIterator->NextL();
            id != KPosLmNullItemId;
            id = landmarkIterator->NextL())
    {
        CPosLandmark* landmark = iLandmarkDb->ReadLandmarkLC(id);
        iObserver.LandmarkLoaderItemCreatedL(*landmark);
        CleanupStack::PopAndDestroy(landmark);
    }

    CleanupStack::PopAndDestroy(landmarkIterator);
}

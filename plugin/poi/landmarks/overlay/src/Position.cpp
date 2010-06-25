/*
 * Name        : Position.cpp
 * Description : Position helper class
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

#include "Position.h"

#include "Logger.h"

CPosition* CPosition::NewL(MPositionObserver& aObserver)
{
    CPosition* self = new(ELeave) CPosition(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CPosition::~CPosition()
{
    Cancel();

    iPositioner.Close();
    iServer.Close();
}

CPosition::CPosition(MPositionObserver& aObserver)
    : CActive(CActive::EPriorityStandard), iObserver(aObserver), iState(EUnknown)
{
    CActiveScheduler::Add(this);
}

void CPosition::ConstructL()
{
    User::LeaveIfError(iServer.Connect());
    User::LeaveIfError(iPositioner.Open(iServer));

    _LIT(KRequestor, "OpenMAR" );
    User::LeaveIfError(iPositioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KRequestor));

    const TInt KSecond = 1000000;
    TPositionUpdateOptions updateOptions;
    updateOptions.SetUpdateInterval(TTimeIntervalMicroSeconds(10 * KSecond));
    updateOptions.SetUpdateTimeOut(TTimeIntervalMicroSeconds(30 * KSecond));
    updateOptions.SetMaxUpdateAge(TTimeIntervalMicroSeconds(1 * KSecond));
    updateOptions.SetAcceptPartialUpdates(EFalse);

    User::LeaveIfError(iPositioner.SetUpdateOptions(updateOptions));
}

void CPosition::Request()
{
    if (IsActive())
        Cancel();

    iPositioner.GetLastKnownPosition(iPositionInfo, iStatus);
    SetActive();

    iState = ECache;
}

void CPosition::RunL()
{
    TPosition position;
    iPositionInfo.GetPosition(position);

    TTime now;
    now.UniversalTime();

    TTimeIntervalSeconds interval = 0;
    now.SecondsFrom(position.Time(), interval);

    LOGARG("Interval between retrieved position and current time: %d secs", interval.Int());

    // Compare that retrieved data is not outdated
    if (iStatus == KErrNone && interval.Int() < 300)
    {
        iObserver.PositionUpdateL(iStatus.Int(), position);
    }
    else if (iStatus == KErrTimedOut)
    {
        iObserver.PositionUpdateL(iStatus.Int(), position);
    }
    else
    {
        iPositioner.NotifyPositionUpdate(iPositionInfo, iStatus);
        SetActive();

        iState = EGps;
    }
}

void CPosition::DoCancel()
{
    switch (iState)
    {
        case ECache:
            iPositioner.CancelRequest(EPositionerGetLastKnownPosition);
            break;

        case EGps:
            iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
            break;

        default:
            break;
    };
}

TInt CPosition::RunError(TInt aError)
{
    return KErrNone;
}

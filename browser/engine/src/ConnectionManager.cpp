/*
 * Name        : ConnectionManager.cpp
 * Description : Manages application connection
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

#include "ConnectionManager.h"

#include "SettingManager.h"

#include <CommDbConnPref.h>
#include <es_enum.h>

#include "Logger.h"

CConnectionManager* CConnectionManager::NewL(MConnectionManager& aObserver, CSettingManager& aSetting)
{
    CConnectionManager* self = new(ELeave) CConnectionManager(aObserver, aSetting);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CConnectionManager::~CConnectionManager()
{
    Cancel();

    iMonitor.Close();
    iConnection.Close();
    iSocketServ.Close();
}

CConnectionManager::CConnectionManager(MConnectionManager& aObserver, CSettingManager& aSetting)
    : CActive(CActive::EPriorityStandard), iObserver(aObserver), iSetting(aSetting)
{
    CActiveScheduler::Add(this);
}

void CConnectionManager::ConstructL()
{
    LOGTXT("Initializing connection..");

    User::LeaveIfError(iSocketServ.Connect());
    User::LeaveIfError(iConnection.Open(iSocketServ));

    iMonitor.ConnectL();
    iMonitor.NotifyEventL(*this);

//    _LIT8(KDefault, "/application/destination");
    TUint destinationId = 0;
//    iSettings.Get(KDefault, destinationId, EHex);

    if (destinationId == 0)
    {
        // "Always ask"
        TCommDbConnPref pref;
        pref.SetDialogPreference(ECommDbDialogPrefPrompt);

        iConnection.Start(pref, iStatus);
    }
    else
    {
        TConnSnapPref pref;
        pref.SetSnap(destinationId);

        iConnection.Start(pref, iStatus);
    }

    SetActive();
}

void CConnectionManager::RunL()
{
    _LIT(KIapId, "IAP\\Id");
    iConnection.GetIntSetting(KIapId, iIapId);

    LOGARG("Connected with iap %u", iIapId);

    iObserver.ConnectionStartedL(iStatus.Int());
}

void CConnectionManager::DoCancel()
{
    // Unfortunately no RConnection::StartCancel() available..
}

void CConnectionManager::EventL(const CConnMonEventBase& aEvent)
{
    switch (aEvent.EventType())
    {
/*
        case EConnMonConnectionStatusChange:
        {
            const CConnMonNetworkStatusChange& event = static_cast<const CConnMonNetworkStatusChange&>(aEvent);
            break;
        }
*/
        case EConnMonDeleteConnection:
        {
            TUint count = 0;
            iConnection.EnumerateConnections(count);

            if (count == 0)
                LOGTXT("Connection dropped");

/*
                if (info().iIapId == iIapId)
                {
                    connFound = ETrue;
                    break;
                }
*/
            break;
        }

        default:
            break;
    }
}

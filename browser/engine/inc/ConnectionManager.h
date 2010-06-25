/*
 * Name        : ConnectionManager.h
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

#ifndef CONNECTIONMANAGER_H_
#define CONNECTIONMANAGER_H_

#include <e32base.h>
#include <es_sock.h>
#include <rconnmon.h>

class CSettingManager;

/**
 * @brief Notifies the status of the started connection
 */
class MConnectionManager
{
public:
    virtual void ConnectionStartedL(TInt aError) = 0;
};

/**
 * @brief Initializes a working connection
 * 
 */
class CConnectionManager : public CActive, public MConnectionMonitorObserver
{
public:
    static CConnectionManager* NewL(MConnectionManager& aObserver, CSettingManager& aSetting);
    ~CConnectionManager();

protected:
    CConnectionManager(MConnectionManager& aObserver, CSettingManager& aSetting);
    void ConstructL();

    // From CActive
    void RunL();
    void DoCancel();

    // From MConnectionMonitorObserver
    void EventL(const CConnMonEventBase& aEvent);

private:
    MConnectionManager& iObserver;
    CSettingManager& iSetting;

    RSocketServ iSocketServ;
    RConnection iConnection;
    RConnectionMonitor iMonitor;

    TUint32 iIapId;
};

#endif

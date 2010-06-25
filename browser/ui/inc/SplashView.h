/*
 * Name        : SplashView.h
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

#ifndef SPLASHVIEW_H_
#define SPLASHVIEW_H_

#include <AknView.h>

#include "ConnectionManager.h"

class CSettingManager;
class CSplashContainer;

class CSplashView : public CAknView,
        public MConnectionManager
{
public:
    static CSplashView* NewL(CSettingManager& aSetting);
    ~CSplashView();

protected:
    CSplashView(CSettingManager& aSetting);
    void ConstructL();

    // From MConnectionManager
    void ConnectionStartedL(TInt aError);

protected:
    TUid Id() const;
    void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
    void DoDeactivate();

    void HandleCommandL(TInt aCommand);

private:
    CSettingManager& iSetting;
    CConnectionManager* iConnection;
    CSplashContainer* iContainer;
};

#endif  // SPLASHVIEW_H_

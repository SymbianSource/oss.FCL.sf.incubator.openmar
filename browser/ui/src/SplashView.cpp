/*
 * Name        : SplashView.cpp
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

#include "SplashView.h"

#include <AknUtils.h>
#include <AknViewAppui.h>

#include <OpenMAR_0x2002E1AB.rsg>

#include "Application.hrh"
#include "SettingManager.h"
#include "SplashContainer.h"

#include "Logger.h"

CSplashView* CSplashView::NewL(CSettingManager& aSetting)
{
    CSplashView* self = new(ELeave) CSplashView(aSetting);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CSplashView::~CSplashView()
{
    delete iContainer;
    delete iConnection;
}

CSplashView::CSplashView(CSettingManager& aSetting)
    : iSetting(aSetting)
{}

void CSplashView::ConstructL()
{
    BaseConstructL(R_SPLASH_VIEW);

    LOGTXT("Initializing splash screen");

    // Set up connection which should be used by other components
    // (providers, etc) to enable faster connections
    iConnection = CConnectionManager::NewL(*this, iSetting);
}

void CSplashView::ConnectionStartedL(TInt aError)
{
    LOGARG("Connection manager returned %d", aError);

    if (aError)
    {
        // TODO: Handle error condition. Some kind of 'offline mode' should be allowed,
        // thus enabling the use of the application in a restricted way 

//        if (!AppUi()->IsAppShutterRunning())
//            AppUi()->RunAppShutter();
    }

    // Perform other setup..

    // Initial set up complete. Switch to main view 
    AppUi()->ActivateLocalViewL(TUid::Uid(EARView));
}

TUid CSplashView::Id() const
{
    return TUid::Uid(ESplashView);
}

void CSplashView::DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage)
{
    if (!iContainer)
    {
        // Make status pane invisible
        CEikStatusPane* statusPane = StatusPane();
        if (statusPane)
            statusPane->MakeVisible(EFalse);

        iContainer = new(ELeave) CSplashContainer(*this);
        iContainer->SetMopParent(this);
        iContainer->ConstructL(AppUi()->ApplicationRect(), 0);

        AppUi()->AddToStackL(*this, iContainer);
    }
}

void CSplashView::DoDeactivate()
{
    if (iContainer)
    {
        AppUi()->RemoveFromViewStack(*this, iContainer);

        delete iContainer;
        iContainer = 0;
    }
}

void CSplashView::HandleCommandL(TInt aCommand)
{
    AppUi()->HandleCommandL(aCommand);
}

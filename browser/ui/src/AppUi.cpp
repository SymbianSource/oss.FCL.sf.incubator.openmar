/*
 * Name        : AppUi.cpp
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

#include "AppUi.h"

#include <avkon.hrh>
#include <AknMessageQueryDialog.h>
#include <AknNoteWrappers.h>
#include <stringloader.h>
#include <hlplch.h>

#include <OpenMAR_0x2002E1AB.rsg>

#include "Application.hrh"
#include "Application.pan"
#include "Application.h"

#include "SettingManager.h"

#include "SplashView.h"
#include "ARView.h"

#include "Logger.h"

void CAppUi::ConstructL()
{
    LOGTXT("Creating AppUi");

    BaseConstructL(EAknEnableSkin | EAppOrientationLandscape);
//    BaseConstructL(EAknEnableSkin);

    iSetting = CSettingManager::NewL();

    // Set up splash view and make it default
    CAknView* splashView = CSplashView::NewL(*iSetting);
    CleanupStack::PushL(splashView);
    AddViewL(splashView);
    CleanupStack::Pop(splashView);

    SetDefaultViewL(*splashView);

    CAknView* arView = CARView::NewL();
    CleanupStack::PushL(arView);
    AddViewL(arView);
    CleanupStack::Pop(arView);
}

CAppUi::~CAppUi()
{
    delete iSetting;

    REComSession::FinalClose();
}

void CAppUi::HandleCommandL(TInt aCommand)
{
    switch (aCommand)
    {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;
/*
        case EHelp:
        {
            CArrayFix<TCoeHelpContext>* buf = CCoeAppUi::AppHelpContextL();
            HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
        }
        break;
*/
        case EAbout:
        {
            CAknMessageQueryDialog* dlg = new (ELeave)CAknMessageQueryDialog();
            dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
            HBufC* title = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
            dlg->QueryHeading()->SetTextL(*title);
            CleanupStack::PopAndDestroy(title);
            HBufC* msg = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
            dlg->SetMessageTextL(*msg);
            CleanupStack::PopAndDestroy(msg);
            dlg->RunLD();
        }
        break;

        default:
//            Panic();
            break;
    }
}

CArrayFix<TCoeHelpContext>* CAppUi::HelpContextL() const
{
#warning "Please see comment about help and UID3..."
// Note: Help will not work if the application uid3 is not in the
// protected range.  The default uid3 range for projects created
// from this template (0xE0000000 - 0xEFFFFFFF) are not in the protected range so that they
// can be self signed and installed on the device during testing.
// Once you get your official uid3 from Symbian Ltd. and find/replace
// all occurrences of uid3 in your project, the context help will
// work.
/*
    CArrayFixFlat<TCoeHelpContext>* array = new(ELeave) CArrayFixFlat<TCoeHelpContext>(1);
    CleanupStack::PushL(array);
    array->AppendL(TCoeHelpContext(KUidApplication, KGeneral_Information));
    CleanupStack::Pop(array);
    return array;
*/
    return 0;
}

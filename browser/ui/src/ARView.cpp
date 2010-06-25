/*
 * Name        : ARView.cpp
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

#include "ARView.h"

#include <AknViewAppui.h>

#include <OpenMAR_0x2002E1AB.rsg>

#include "Application.hrh"

#include "ARContainer.h"

#include <AknUtils.h>

#include "Logger.h"

CARView* CARView::NewL()
{
    CARView* self = new(ELeave) CARView();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CARView::~CARView()
{
    DoDeactivate();
}

CARView::CARView()
{}

void CARView::ConstructL()
{
    BaseConstructL(R_AR_VIEW);
}

TUid CARView::Id() const
{
    return TUid::Uid(EARView);
}

void CARView::DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage)
{
    LOGTXT("CARView::DoActivateL()");

    if (!iRenderer)
    {
        iPrevViewId = aPrevViewId;

        // Make status pane invisible
        CEikStatusPane* statusPane = StatusPane();
        if (statusPane)
            statusPane->MakeVisible(EFalse);

        iRenderer = CARContainer::NewL(*this, AppUi()->ApplicationRect());

        AppUi()->AddToStackL(*this, iRenderer);
    }
}

void CARView::DoDeactivate()
{
    if (iRenderer)
    {
        AppUi()->RemoveFromViewStack(*this, iRenderer);

        delete iRenderer;
        iRenderer = 0;
    }
}

void CARView::DynInitToolbarL(TInt aResourceId, CAknToolbar* aToolbar)
{
}

void CARView::OfferToolbarEventL(TInt aCommand)
{
//    HandleCommandL(aCommand);
}

void CARView::HandleCommandL(TInt aCommand)
{
    switch (aCommand)
    {
        case EAknSoftkeyBack:
            AppUi()->ActivateLocalViewL(iPrevViewId.iViewUid);
            break;

        default:
            AppUi()->HandleCommandL(aCommand);
            break;
    }
}

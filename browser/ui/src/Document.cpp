/*
 * Name        : Document.cpp
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

#include "Document.h"

#include "AppUi.h"

#include "Logger.h"

CDocument* CDocument::NewL(CEikApplication& aApp)
{
    CDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
}

CDocument* CDocument::NewLC(CEikApplication& aApp)
{
    CDocument* self = new(ELeave) CDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

void CDocument::ConstructL()
{
}

CDocument::CDocument(CEikApplication& aApp)
    : CAknDocument(aApp)
{
}

CDocument::~CDocument()
{
    LOGCLOSE
}

CEikAppUi* CDocument::CreateAppUiL()
{
    return new(ELeave) CAppUi;
}

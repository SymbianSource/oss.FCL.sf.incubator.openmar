/*
 * Name        : Application.cpp
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

#include "Application.h"

#include <eikstart.h>

#include "Application.hrh"
#include "Document.h"

CApaDocument* CApplication::CreateDocumentL()
{
    return CDocument::NewL(*this);
}

TUid CApplication::AppDllUid() const
{
    return KUidApplication;
}

LOCAL_C CApaApplication* NewApplication()
{
    return new CApplication;
}

GLDEF_C TInt E32Main()
{
    return EikStart::RunApplication(NewApplication);
}

/*
 * Name        : Document.h
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

#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include <akndoc.h>

class CAppUi;
class CEikApplication;

class CDocument : public CAknDocument
{
public:
    static CDocument* NewL(CEikApplication& aApp);
    static CDocument* NewLC(CEikApplication& aApp);
    virtual ~CDocument();

public:
    /**
     * CreateAppUiL
     * From CEikDocument, CreateAppUiL.
     * Create a CcamAppUi object and return a pointer to it.
     * The object returned is owned by the Uikon framework.
     * @return Pointer to created instance of AppUi.
     */
    CEikAppUi* CreateAppUiL();

private:
    void ConstructL();
    CDocument(CEikApplication& aApp);
};

#endif // __DOCUMENT_H__

/*
 * Name        : Loader.h
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

#ifndef LOADER_H_
#define LOADER_H_

#include <e32base.h>
#include <http/MHttpTransactionCallback.h>
#include <xml/ContentHandler.h>
#include <xml/Parser.h>

#include <EPos_CPosLandmark.h>

//class CPosLandmark;
class TCoordinate;
class CHttpClient;
class CEntry;

class MLandmarkLoaderObserver
{
public:
    virtual void LandmarkLoaderOpenedL(TInt aError) = 0;
    virtual void LandmarkLoaderItemCreatedL(const CPosLandmark& aLandmark) = 0;
};

class CLoader : public CBase, public MHTTPTransactionCallback, public Xml::MContentHandler
{
public:
    static CLoader* NewL(MLandmarkLoaderObserver& aObserver);
    ~CLoader();

protected:
    CLoader(MLandmarkLoaderObserver& aObserver);
    void ConstructL();

public:
    void RequestL(const TCoordinate& aPosition, TReal32 aRadius);

protected:
    // From MHTTPTransactionCallback
    void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
    TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

protected:
    // from MContentHandler
    void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode);
    void OnEndDocumentL(TInt aErrorCode);
    void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
    void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
    void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
    void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
    void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
    void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
    void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
    void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
    void OnError(TInt aErrorCode);
    TAny* GetExtendedInterface(const TInt32 aUid);

    void NotifyPOIEntryL();

private:
    MLandmarkLoaderObserver& iObserver;

    CHttpClient* iClient;
    Xml::CParser* iParser;

    CEntry* iEntry;

    enum TState {
        EInvalid,
        EValid
    } iState;
};

#endif

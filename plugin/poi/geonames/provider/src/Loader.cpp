/*
 * Name        : Loader.cpp
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

#include "Loader.h"

#include "HttpClient.h"
#include "Entry.h"

#include <AknUtils.h>
#include <LbsPositionInfo.h>
#include <Http/mhttpdatasupplier.h>
#include <HttpErr.h>

#include <EPos_CPosLandmark.h>
#include <utf.h>

#include "Logger.h"

CLoader* CLoader::NewL(MLandmarkLoaderObserver& aObserver)
{
    CLoader* self = new(ELeave) CLoader(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CLoader::~CLoader()
{
    delete iEntry;
    delete iParser;
    delete iClient;
}

CLoader::CLoader(MLandmarkLoaderObserver& aObserver)
    : iObserver(aObserver)
{
}

void CLoader::ConstructL()
{
    LOGTXT("[GEONAMES] Creating POI loader");

    iClient = CHttpClient::NewL(*this);

    _LIT8(KXmlMimeType, "text/xml");
    iParser = Xml::CParser::NewL(KXmlMimeType, *this);

    iEntry = CEntry::NewL();
}

void CLoader::RequestL(const TCoordinate& aPosition, TReal32 aRadius)
{
    // Regardless of system locale, always use point as decimal separator character
    // Also use a maximum of 6 chars to represent each float
    TRealFormat realFormat;
    realFormat.iPoint  = TChar('.');
    realFormat.iWidth  = 8;
    realFormat.iPlaces = 5;

    TBuf8<KDefaultRealWidth> latitude;
    latitude.Num(aPosition.Latitude(), realFormat);

    TBuf8<KDefaultRealWidth> longitude;
    longitude.Num(aPosition.Longitude(), realFormat);

    // Convert to kilometers
    const TInt radius = static_cast<TInt>(aRadius / 1000);

    TBuf8<2> langCode;

    // Use device's language to retrieve entries
    HBufC* tag = AknLangUtils::DisplayLanguageTagL();
    langCode.Copy(tag->Left(2));
    delete tag;

    // Set a limit to entries retrieved
    const TInt maxRows = 20;

    TBuf8<256> uri;
    _LIT8(KUri, "http://ws.geonames.org/findNearbyWikipedia?lat=%S&lng=%S&radius=%d&lang=%S&maxRows=%d");
    uri.Format(KUri, &latitude, &longitude, radius, &langCode, maxRows);

    iClient->GetL(uri);
}

void CLoader::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
{
    switch (aEvent.iStatus)
    {
        case THTTPEvent::EGotResponseHeaders:
        {
            RHTTPResponse resp = aTransaction.Response();
            TInt status = resp.StatusCode();

            // Treat any 2xx HTTP code as successful request
            TInt error = HTTPStatus::IsSuccessful(status)? KErrNone : KErrGeneral;

            if (error == KErrNone)
                iParser->ParseBeginL();

            iObserver.LandmarkLoaderOpenedL(error);

            break;
        }

        case THTTPEvent::EGotResponseBodyData:
        {
            MHTTPDataSupplier* body = aTransaction.Response().Body();

            TPtrC8 dataChunk;
            body->GetNextDataPart(dataChunk);
            iParser->ParseL(dataChunk);
            body->ReleaseData();

            break;
        }

        case THTTPEvent::EResponseComplete:
            LOGTXT("[GEONAMES] HTTP response complete");

            iParser->ParseEndL();
            break ;

        case THTTPEvent::ESucceeded:
        case THTTPEvent::EFailed:
        {
            RHTTPResponse resp = aTransaction.Response();
            TInt status = resp.StatusCode();

            LOGARG("[GEONAMES] HTTP response status code %d", status);

            aTransaction.Close();

            break;
        }

        default:
            LOGARG("[GEONAMES] HTTP unknown event %d", aEvent.iStatus);

            iObserver.LandmarkLoaderOpenedL(KErrUnknown);

            aTransaction.Close();
            break;
    }
}

TInt CLoader::MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
{
    return KErrNone;
}

void CLoader::OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode)
{}

void CLoader::OnEndDocumentL(TInt aErrorCode)
{}

void CLoader::OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode)
{
    _LIT8(KEntryTag, "entry");
    if (aElement.LocalName().DesC() == KEntryTag)
        iState = EValid;
    else if (iState == EValid)
        iEntry->EnterState(aElement.LocalName().DesC());
}

void CLoader::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode)
{
    _LIT8(KEntryTag, "entry");
    if (aElement.LocalName().DesC() == KEntryTag)
    {
        NotifyPOIEntryL();

        iEntry->Reset();
        iState = EInvalid;
    }
    else
        iEntry->ExitState();
}

void CLoader::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
{
    iEntry->FeedState(aBytes);
}

void CLoader::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt /*aErrorCode*/)
{}

void CLoader::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt /*aErrorCode*/)
{}

void CLoader::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
{}

void CLoader::OnSkippedEntityL(const RString& /*aName*/, TInt /*aErrorCode*/)
{}

void CLoader::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt /*aErrorCode*/)
{}

void CLoader::OnError(TInt /*aErrorCode*/)
{}

TAny* CLoader::GetExtendedInterface(const TInt32 /*aUid*/)
{
    return 0;
}

/*
 * Notify observer of new XML entry
 */
void CLoader::NotifyPOIEntryL()
{
    CPosLandmark* landmark = CPosLandmark::NewLC();

    HBufC* title = CnvUtfConverter::ConvertToUnicodeFromUtf8L(iEntry->iTitle);
    CleanupStack::PushL(title);
    landmark->SetLandmarkNameL(*title);
    CleanupStack::PopAndDestroy(title);

    HBufC* description = CnvUtfConverter::ConvertToUnicodeFromUtf8L(iEntry->iSummary);
    CleanupStack::PushL(description);
    landmark->SetLandmarkDescriptionL(*description);
    CleanupStack::PopAndDestroy(description);

    TLex8 lex;

    lex.Assign(iEntry->iLat);
    TReal32 lat = 0;
    lex.Val(lat);

    lex.Assign(iEntry->iLng);
    TReal32 lng = 0;
    lex.Val(lng);

    lex.Assign(iEntry->iElevation);
    TReal32 elevation = 0;
    lex.Val(elevation);

    TCoordinate coordinate(lat, lng, elevation);
    landmark->SetPositionL(TLocality(coordinate, 0, 0));

    HBufC* url = HBufC::NewLC(iEntry->iWikipediaUrl.Length());
    url->Des().Copy(iEntry->iWikipediaUrl);
    landmark->SetPositionFieldL(EPositionFieldMediaLinksStart, url->Left(KPosLmMaxTextFieldLength));
    CleanupStack::PopAndDestroy(url);

    iObserver.LandmarkLoaderItemCreatedL(*landmark);

    CleanupStack::PopAndDestroy(landmark);
}

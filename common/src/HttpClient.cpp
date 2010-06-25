/*
 * Name        : HttpClient.cpp
 * Description : HTTP helper class
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

#include "HttpClient.h"

#include <Http.h>
#include <http/RHttpHeaders.h>

#include <cmmanager.h>
#include <cmdestination.h>
#include <CommDbConnPref.h> 

#include <es_enum.h>

#include "Logger.h"

CHttpClient* CHttpClient::NewL(MHTTPTransactionCallback& aCallback)
{
    CHttpClient* self = new(ELeave) CHttpClient(aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CHttpClient::~CHttpClient()
{
    iSession.Close();
    iConnection.Close();
    iSocketServ.Close();
}

CHttpClient::CHttpClient(MHTTPTransactionCallback& aCallback)
    : iCallback(aCallback)
{
}

void CHttpClient::ConstructL()
{
    User::LeaveIfError(iSocketServ.Connect());

    User::LeaveIfError(iConnection.Open(iSocketServ));

    TUint connectionCount = 0;
    TInt error = iConnection.EnumerateConnections(connectionCount);

//    if (error || connectionCount < 1)

    TPckgBuf<TConnectionInfo> connectionInfo;
    error = iConnection.GetConnectionInfo(1, connectionInfo); // First connection

//    if (error)
//        LOGARG("Error %d retrieving connection info", error);

    error = iConnection.Attach(connectionInfo, RConnection::EAttachTypeNormal);

//    LOGARG("Connection attached %d", error);

//    LOGTXT("Opening http session");

    iSession.OpenL();

    RStringPool strPool = iSession.StringPool();
    RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();
    connInfo.SetPropertyL(strPool.StringF(HTTP::EHttpSocketServ,
        RHTTPSession::GetTable()), THTTPHdrVal(iSocketServ.Handle()));
    TInt connPtr = reinterpret_cast<TInt>(&iConnection);
    connInfo.SetPropertyL(strPool.StringF(HTTP::EHttpSocketConnection,
        RHTTPSession::GetTable()), THTTPHdrVal(connPtr));
}

RHTTPTransaction CHttpClient::GetL(const TDesC8& aUri)
{
    TUriParser8 uri;
    uri.Parse(aUri);

    LOGARG("URI submitted is %S", &aUri);

    RStringPool strP = iSession.StringPool();
    RStringF method = strP.StringF(HTTP::EGET, RHTTPSession::GetTable());

    RHTTPTransaction transaction = iSession.OpenTransactionL(uri, iCallback, method);

    _LIT8(KUserAgent, "OpenMAR/1.0 (Symbian)");
    _LIT8(KAccept, "*/*");

    RHTTPHeaders hdr = transaction.Request().GetHeaderCollection();
    // Add headers appropriate to all methods
    SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
    SetHeaderL(hdr, HTTP::EAccept, KAccept);

    transaction.SubmitL();

    return transaction;
}

void CHttpClient::Stop(RHTTPTransaction& aTransaction)
{
    aTransaction.Cancel();
}

void CHttpClient::SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue)
{
    RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
    THTTPHdrVal val(valStr);
    aHeaders.SetFieldL(iSession.StringPool().StringF(aHdrField, RHTTPSession::GetTable()), val);
    valStr.Close();
}

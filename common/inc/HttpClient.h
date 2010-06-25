/*
 * Name        : HttpClient.h
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

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include <http/RHttpSession.h>
#include <http/MHttpTransactionCallback.h>

#include <es_sock.h>

class CHttpClient : public CBase
{
public:
    static CHttpClient* NewL(MHTTPTransactionCallback& aCallback);
    ~CHttpClient();

protected:
	CHttpClient(MHTTPTransactionCallback& aCallback);
    void ConstructL();

public:
    RHTTPTransaction GetL(const TDesC8& aUri);
    void Stop(RHTTPTransaction& aTransaction);

protected:
    void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue);

private:
    MHTTPTransactionCallback& iCallback;

    RHTTPSession iSession;
    RSocketServ iSocketServ;
    RConnection iConnection;
};

#endif  // HTTPSOURCE_H_

#pragma once
#include "EMessage.h"
#include "EClient.h"
#include "ETransport.h"
#include "EWrapper.h"
#include "EClientMsgSink.h"
#include "EDecoder.h"

class TwsTemplate;
class EClientSocket2;

// **********************
// Modeled after ESocket class.
class ESocket2 : public ETransport {

public:
	//overrides of ETransport virtual functions
	int send(EMessage* pMsg);
};

// **********************
// Modeled after EReader class.
class EReader2 {
public:
	EReader2(EClientSocket2* pC_);
	EClientSocket2* pC;
	EDecoder d;
};


// **********************
// Modeled after EClientSocket class.
class EClientSocket2 : public EClient, public EClientMsgSink {
public:
	EClientSocket2(EWrapper* pW);
	EWrapper* pW;

	//overrides of EClient virtual functions
public:
	void eDisconnect(bool resetState);
private:
	int receive(char* buf, size_t sz);
	void prepareBufferImpl(std::ostream&) const;
	void prepareBuffer(std::ostream&) const;
	bool closeAndSend(std::string msg, unsigned offset = 0);
	bool isSocketOK() const;

public:
	// overrides of EClientMsgSink
	void serverVersion(int version, const char* time);
	void redirect(const char* host, int port);

};

// **********************
// Modeled after TestCppClient class.
class TwsTemplate : public EWrapper {
public:
	TwsTemplate();
	~TwsTemplate();

	EClientSocket2* const pC;
	std::unique_ptr<EReader2> pR;

	/* recommended implementations:
	bool connect(const char * host, int port, int clientId = 0);
	void disconnect() const;
	bool isConnected() const;
	*/

	//overrides of EWrapper virtual functions
	//The prototypes file changes a lot, just include it.
	//This way, the *.cpp file will be easy to fix when you update the TWS API.
#define virtual
#define EWRAPPER_VIRTUAL_IMPL
#include "EWrapper_prototypes.h"
#undef EWRAPPER_VIRTUAL_IMPL
#undef virtual
};


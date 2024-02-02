#include "TwsTemplate.h"


#define TBD_RETURN 0

int ESocket2::send(EMessage* pMsg) {
	return TBD_RETURN;
}

// **********************

EReader2::EReader2(EClientSocket2* pC_)
	: d(pC_->EClient::serverVersion(), pC_->getWrapper(), pC_)
	, pC(pC_)
{
}


// **********************

EClientSocket2::EClientSocket2(EWrapper* pW_) :
	pW(pW_)
	, ::EClient(pW_, new ESocket2())
{
}

void EClientSocket2::eDisconnect(bool resetState) {
}



int EClientSocket2::receive(char* buf, size_t sz) {
	return TBD_RETURN;
}

void EClientSocket2::prepareBufferImpl(std::ostream& buf) const {
}
void EClientSocket2::prepareBuffer(std::ostream& buf) const {
}
bool EClientSocket2::closeAndSend(std::string msg, unsigned offset) {
	return TBD_RETURN;
}
bool EClientSocket2::isSocketOK() const {
	return TBD_RETURN;
}


void EClientSocket2::serverVersion(int version, const char* time) {
}

void EClientSocket2::redirect(const char* host, int port) {
}


// **********************

TwsTemplate::TwsTemplate()
	: pC(new EClientSocket2(this))
{
}


TwsTemplate::~TwsTemplate() {
	
	// destroy the reader before the client
	if (pR)
		pR.reset();
	delete pC;
}

//Copied from EWrapper_prototypes.h, version 10.26. Removed "virtual" and " EWRAPPER_VIRTUAL_IMPL".
void TwsTemplate::tickPrice(TickerId tickerId, TickType field, double price, const TickAttrib& attrib){}
void TwsTemplate::tickSize(TickerId tickerId, TickType field, Decimal size){}
void TwsTemplate::tickOptionComputation(TickerId tickerId, TickType tickType, int tickAttrib, double impliedVol, double delta,
	double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice){}
void TwsTemplate::tickGeneric(TickerId tickerId, TickType tickType, double value){}
void TwsTemplate::tickString(TickerId tickerId, TickType tickType, const std::string& value){}
void TwsTemplate::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints,
	double totalDividends, int holdDays, const std::string& futureLastTradeDate, double dividendImpact, double dividendsToLastTradeDate){}
void TwsTemplate::orderStatus(OrderId orderId, const std::string& status, Decimal filled,
	Decimal remaining, double avgFillPrice, int permId, int parentId,
	double lastFillPrice, int clientId, const std::string& whyHeld, double mktCapPrice){}
void TwsTemplate::openOrder(OrderId orderId, const Contract&, const Order&, const OrderState&){}
void TwsTemplate::openOrderEnd(){}
void TwsTemplate::winError(const std::string& str, int lastError){}
void TwsTemplate::connectionClosed(){}
void TwsTemplate::updateAccountValue(const std::string& key, const std::string& val,
	const std::string& currency, const std::string& accountName){}
void TwsTemplate::updatePortfolio(const Contract& contract, Decimal position,
	double marketPrice, double marketValue, double averageCost,
	double unrealizedPNL, double realizedPNL, const std::string& accountName){}
void TwsTemplate::updateAccountTime(const std::string& timeStamp){}
void TwsTemplate::accountDownloadEnd(const std::string& accountName){}
void TwsTemplate::nextValidId(OrderId orderId){}
void TwsTemplate::contractDetails(int reqId, const ContractDetails& contractDetails){}
void TwsTemplate::bondContractDetails(int reqId, const ContractDetails& contractDetails){}
void TwsTemplate::contractDetailsEnd(int reqId){}
void TwsTemplate::execDetails(int reqId, const Contract& contract, const Execution& execution){}
void TwsTemplate::execDetailsEnd(int reqId){}
void TwsTemplate::error(int id, int errorCode, const std::string& errorString, const std::string& advancedOrderRejectJson){}
void TwsTemplate::updateMktDepth(TickerId id, int position, int operation, int side,
	double price, Decimal size){}
void TwsTemplate::updateMktDepthL2(TickerId id, int position, const std::string& marketMaker, int operation,
	int side, double price, Decimal size, bool isSmartDepth){}
void TwsTemplate::updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage, const std::string& originExch){}
void TwsTemplate::managedAccounts(const std::string& accountsList){}
void TwsTemplate::receiveFA(faDataType pFaDataType, const std::string& cxml){}
void TwsTemplate::historicalData(TickerId reqId, const Bar& bar){}
void TwsTemplate::historicalDataEnd(int reqId, const std::string& startDateStr, const std::string& endDateStr){}
void TwsTemplate::scannerParameters(const std::string& xml){}
void TwsTemplate::scannerData(int reqId, int rank, const ContractDetails& contractDetails,
	const std::string& distance, const std::string& benchmark, const std::string& projection,
	const std::string& legsStr){}
void TwsTemplate::scannerDataEnd(int reqId){}
void TwsTemplate::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
	Decimal volume, Decimal wap, int count){}
void TwsTemplate::currentTime(long time){}
void TwsTemplate::fundamentalData(TickerId reqId, const std::string& data){}
void TwsTemplate::deltaNeutralValidation(int reqId, const DeltaNeutralContract& deltaNeutralContract){}
void TwsTemplate::tickSnapshotEnd(int reqId){}
void TwsTemplate::marketDataType(TickerId reqId, int marketDataType){}
void TwsTemplate::commissionReport(const CommissionReport& commissionReport){}
void TwsTemplate::position(const std::string& account, const Contract& contract, Decimal position, double avgCost){}
void TwsTemplate::positionEnd(){}
void TwsTemplate::accountSummary(int reqId, const std::string& account, const std::string& tag, const std::string& value, const std::string& curency){}
void TwsTemplate::accountSummaryEnd(int reqId){}
void TwsTemplate::verifyMessageAPI(const std::string& apiData){}
void TwsTemplate::verifyCompleted(bool isSuccessful, const std::string& errorText){}
void TwsTemplate::displayGroupList(int reqId, const std::string& groups){}
void TwsTemplate::displayGroupUpdated(int reqId, const std::string& contractInfo){}
void TwsTemplate::verifyAndAuthMessageAPI(const std::string& apiData, const std::string& xyzChallange){}
void TwsTemplate::verifyAndAuthCompleted(bool isSuccessful, const std::string& errorText){}
void TwsTemplate::connectAck(){}
void TwsTemplate::positionMulti(int reqId, const std::string& account, const std::string& modelCode, const Contract& contract, Decimal pos, double avgCost){}
void TwsTemplate::positionMultiEnd(int reqId){}
void TwsTemplate::accountUpdateMulti(int reqId, const std::string& account, const std::string& modelCode, const std::string& key, const std::string& value, const std::string& currency){}
void TwsTemplate::accountUpdateMultiEnd(int reqId){}
void TwsTemplate::securityDefinitionOptionalParameter(int reqId, const std::string& exchange, int underlyingConId, const std::string& tradingClass,
	const std::string& multiplier, const std::set<std::string>& expirations, const std::set<double>& strikes){}
void TwsTemplate::securityDefinitionOptionalParameterEnd(int reqId){}
void TwsTemplate::softDollarTiers(int reqId, const std::vector<SoftDollarTier>& tiers){}
void TwsTemplate::familyCodes(const std::vector<FamilyCode>& familyCodes){}
void TwsTemplate::symbolSamples(int reqId, const std::vector<ContractDescription>& contractDescriptions){}
void TwsTemplate::mktDepthExchanges(const std::vector<DepthMktDataDescription>& depthMktDataDescriptions){}
void TwsTemplate::tickNews(int tickerId, time_t timeStamp, const std::string& providerCode, const std::string& articleId, const std::string& headline, const std::string& extraData){}
void TwsTemplate::smartComponents(int reqId, const SmartComponentsMap& theMap){}
void TwsTemplate::tickReqParams(int tickerId, double minTick, const std::string& bboExchange, int snapshotPermissions){}
void TwsTemplate::newsProviders(const std::vector<NewsProvider>& newsProviders){}
void TwsTemplate::newsArticle(int requestId, int articleType, const std::string& articleText){}
void TwsTemplate::historicalNews(int requestId, const std::string& time, const std::string& providerCode, const std::string& articleId, const std::string& headline){}
void TwsTemplate::historicalNewsEnd(int requestId, bool hasMore){}
void TwsTemplate::headTimestamp(int reqId, const std::string& headTimestamp){}
void TwsTemplate::histogramData(int reqId, const HistogramDataVector& data){}
void TwsTemplate::historicalDataUpdate(TickerId reqId, const Bar& bar){}
void TwsTemplate::rerouteMktDataReq(int reqId, int conid, const std::string& exchange){}
void TwsTemplate::rerouteMktDepthReq(int reqId, int conid, const std::string& exchange){}
void TwsTemplate::marketRule(int marketRuleId, const std::vector<PriceIncrement>& priceIncrements){}
void TwsTemplate::pnl(int reqId, double dailyPnL, double unrealizedPnL, double realizedPnL){}
void TwsTemplate::pnlSingle(int reqId, Decimal pos, double dailyPnL, double unrealizedPnL, double realizedPnL, double value){}
void TwsTemplate::historicalTicks(int reqId, const std::vector<HistoricalTick>& ticks, bool done){}
void TwsTemplate::historicalTicksBidAsk(int reqId, const std::vector<HistoricalTickBidAsk>& ticks, bool done){}
void TwsTemplate::historicalTicksLast(int reqId, const std::vector<HistoricalTickLast>& ticks, bool done){}
void TwsTemplate::tickByTickAllLast(int reqId, int tickType, time_t time, double price, Decimal size, const TickAttribLast& tickAttribLast, const std::string& exchange, const std::string& specialConditions){}
void TwsTemplate::tickByTickBidAsk(int reqId, time_t time, double bidPrice, double askPrice, Decimal bidSize, Decimal askSize, const TickAttribBidAsk& tickAttribBidAsk){}
void TwsTemplate::tickByTickMidPoint(int reqId, time_t time, double midPoint){}
void TwsTemplate::orderBound(long long orderId, int apiClientId, int apiOrderId){}
void TwsTemplate::completedOrder(const Contract& contract, const Order& order, const OrderState& orderState){}
void TwsTemplate::completedOrdersEnd(){}
void TwsTemplate::replaceFAEnd(int reqId, const std::string& text){}
void TwsTemplate::wshMetaData(int reqId, const std::string& dataJson){}
void TwsTemplate::wshEventData(int reqId, const std::string& dataJson){}
void TwsTemplate::historicalSchedule(int reqId, const std::string& startDateTime, const std::string& endDateTime, const std::string& timeZone, const std::vector<HistoricalSession>& sessions){}
void TwsTemplate::userInfo(int reqId, const std::string& whiteBrandingId){}


//============================================================================
// Name        : example.cpp
// Author      : guofuhui
// Version     : 1.0.0
// Copyright   : nanhua
// Description : nhmdapi example
//============================================================================

#include <string.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <sstream>
#include "api/NHMdApi.h"

using namespace std;

class CMdTest:public CNhMdSpi
{
public:
	CMdTest(string svrAddr, string routeKey, int cpuid)
	{
		_svrAddr = svrAddr;
		_routeKey = routeKey;
		_cpuid = cpuid;

		_requestID = 0;
		_recCount = 0;
		_last_sequence = 0;
		_api = CNhMdApi::CreateMdApi();
	}

	~CMdTest()
	{
		unSubscribe();
		usleep(2);
		_api->Release();
	}
public:
	void OnFrontConnected()
	{
		cout << "OnFrontConnected" << endl;
		login();
	}

	void OnFrontDisConnected()
	{
		cout << "OnFrontDisconnected" << endl;
	}

	void OnRspError(ERRORMSGINFO_t &pRspInfo,TSequenceIDType nRequestID)
	{
		cout << "OnRspError:" << pRspInfo.error_message << endl;
	}

	void OnRtnMarketData(STKMarketData_t &pData)
	{
		printf("%s\n", __FUNCTION__);
		cout << "\t trading_day:" << pData.trading_day << endl
			 << "\t update_time:" << pData.update_time << endl
			 << "\t update_millisec:" << pData.update_millisec << endl
			 << "\t instrument_id:" << pData.instrument_id << endl
			 << "\t last_price:" << pData.last_price << endl
			 << "\t volume:" << pData.volume << endl
			 << "\t upper_limit_price:" << pData.upper_limit_price << endl
			 << "\t lower_limit_price:" << pData.lower_limit_price << endl
			 << "\t bid_price1:" << pData.bid_price1 << endl
			 << "\t bid_volume1:" << pData.bid_volume1 << endl
			 << "\t ask_price1:" << pData.ask_price1 << endl
			 << "\t ask_volume1:" << pData.ask_volume1 << endl
			;
	}

	void OnRspUtpLogin(const RspUtpLoginField_t& rsp,TSequenceIDType nRequestID)
	{
		cout << "OnRspUtpLogin" << endl;
		if (Err_Succeed == rsp.response_code)
		{
			
			//usleep(1000);
			subscribe();
		}
	}

	void OnRspUtpLogout(const RspUtpLogoutField_t& rsp,TSequenceIDType nRequestID)
	{
		cout << "OnRspUtpLogout" << endl;
	}

	void OnRspSubscribe(const RspSubscribeField_t& rsp,TSequenceIDType nRequestID)
	{
		cout << "OnRspSubscribe:" << rsp.response_string << endl;
	}

	void OnRspUnSubscribe(const RspUnSubscribeField_t& rsp,TSequenceIDType nRequestID)
	{
		cout << "OnRspUnSubscribe" << endl;
	}

	void OnRspQryExchange(const RspQryExchangeField_t& rsp,TSequenceIDType nRequestID)
	{
		cout << nRequestID << "|" << rsp.exchange_id << "|" << rsp.exchange_name 
			<< "|" << rsp.exchange_status << endl;
	}

	void OnRspQryInstrument(const RspQryInstrumentField_t& rsp,TSequenceIDType nRequestID)
	{
		cout << nRequestID << "|" << rsp.exchange_id << "|" << rsp.instrument_id 
			<< "|" << rsp.instrument_name << endl;

	}
public:
	void init()
	{
		_api->RegisterSpi(this);
		_api->RegisterFront((char*)_svrAddr.c_str());
		_api->Init();
	}

	int login()
	{
		int reqId = _requestID++;
		ReqUtpLoginField_t request = {0};
		strcpy(request.developer_code,"xuwanxin");
		strcpy(request.developer_license,"xwx123");
		strcpy(request.user_id,"xwx");
		strcpy(request.user_password,"123456");
		int rc = _api->ReqUtpLogin(request,reqId);
		if (0 != rc)
		{
			cout <<"ReqUtpLogin:ret:" << rc << "|reqId:" << reqId << endl;
		}
		return rc;
	}

	int logout()
	{
		int reqId = _requestID++;
		int rc = _api->ReqUtpLogout(reqId);
		if (0 != rc)
		{
			cout <<"ReqUtpLogout:ret:" << rc << "|reqId:" << reqId << endl;
		}
		return rc;
	}

	int subscribe()
	{
		int reqId = _requestID++;
		ReqSubscribeField_t request = {0};

		strcpy(request.routing_key[0],_routeKey.c_str());

		int rc = _api->ReqSubscribe(request,reqId);
		if (0 != rc)
		{
			cout <<"ReqSubscribe:ret:" << rc << "|reqId:" << reqId << endl;
		}
		return rc;
	}

	int unSubscribe()
	{
		int reqId = _requestID++;
		ReqUnSubscribeField_t request = {0};
		strcpy(request.routing_key[0],_routeKey.c_str());

		int rc = _api->ReqUnSubscribe(request,reqId);
		if (0 != rc)
		{
			cout <<"ReqUnSubscribe:ret:" << rc << "|reqId:" << reqId << endl;
		}
		return rc;
	}

	int qryExchange()
	{
		int reqId = _requestID++;
		ReqQryExchangeField_t request = {0};
		request.oms_server_id = 1;
		int rc = _api->ReqQryExchange(request,reqId);
		if (0 != rc)
		{
			cout <<"ReqQryExchange:ret:" << rc << "|reqId:" << reqId << endl;
		}
		return rc;
	}

	int qryInstrument()
	{
		int reqId = _requestID++;
		ReqQryInstrumentField_t request = {0};
		request.oms_server_id = 1;
		strcpy(request.exchange_id,"CBT"); 
		int rc = _api->ReqQryInstrument(request,reqId);
		if (0 != rc)
		{
			cout <<"ReqQryInstrument:ret:" << rc << "|reqId:" << reqId << endl;
		}
		return rc;
	}
	void showCount()
	{
		cout << _recCount << endl;
	}

private:
	string _svrAddr;
	string _routeKey;
	int _cpuid;

	TMillisecType _last_sequence;

	int _recCount;
	int _requestID;
	CNhMdApi *_api;
};

CMdTest *testApi = NULL;

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		printf("params error: addr routekey cpuid  .\n\n\n", argc);
		printf("run like: \n");
		printf("\t./nhmdapi.out \"udp://239.130.111.90:4662|local_ip \"  \".\"\n");
		printf("\n\n");
		return 1;
	}

	string strAddr = argv[1];
	string routekey = argv[2];
	int cpuid = atoi(argv[3]);

	testApi = new CMdTest(strAddr, routekey, cpuid);
	testApi->init();

	char c = '0';
	while (!((c == 'q') || (c == 'Q')))
	{
		cout << "Enter q or Q to exit. " << endl;
		c = getchar();
	}

	delete testApi;
	return 0;
}

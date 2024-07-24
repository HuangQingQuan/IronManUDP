#include <string.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include "udpsocket.hpp"
#include "api/NHMdApi.h"

using namespace std;

constexpr const char* IP = "47.98.117.222";
constexpr uint16_t PORT = 36888;

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
		// read config
		ifstream  fin;
		fin.open("config",ios::in);
		if(!fin)
		{
			std::cerr<<"cannot open the file";
		}
		char buf[1024]={0};
		while (fin >> buf)
		{
			md_InstrumentID.push_back(buf);
		}
		cout << "Instrument Size:" << md_InstrumentID.size() << endl;
	}

	~CMdTest()
	{
		try
		{
			unSubscribe();
			usleep(2);
			_api->Release();
			if(!m_pUdpSocket)
			{
				m_pUdpSocket = NULL;
			}
		}
		catch(...)
		{
			cout << "Error : ~CMdTest"  << endl;
		}
		
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
		if(!m_pUdpSocket)
		{
			m_pUdpSocket = NULL;
		}
		usleep(2);
	}

	void OnRspError(ERRORMSGINFO_t &pRspInfo,TSequenceIDType nRequestID)
	{
		cout << "OnRspError:" << pRspInfo.error_message << endl;
	}

	void OnRtnMarketData(STKMarketData_t &pData)
	{
		// if (std::find(md_InstrumentID.begin(), md_InstrumentID.end(), pData.instrument_id) != md_InstrumentID.end())
		// if(strchr(pData.instrument_id, '-') == 0)
		if(pData.instrument_id[7] == '\0')
		{
			string datagram = "";
			datagram += pData.instrument_id;
			datagram += "|";
			datagram += pData.exchange_id;
			datagram += "|";
			datagram += to_string((pData.last_price > 10000000) ? 0 : pData.last_price);
			datagram += "|";
			datagram += to_string((pData.open_price > 10000000) ? 0 : pData.open_price);
			datagram += "|";
			datagram += to_string((pData.highest_price > 10000000) ? 0 : pData.highest_price);
			datagram += "|";
			datagram += to_string((pData.lowest_price > 10000000) ? 0 : pData.lowest_price);
			datagram += "|";
			datagram += to_string((pData.upper_limit_price > 10000000) ? 0 : pData.upper_limit_price);
			datagram += "|";
			datagram += to_string((pData.lower_limit_price > 10000000) ? 0 : pData.lower_limit_price);
			datagram += "|";
			datagram += to_string(pData.volume);
			datagram += "|";
			datagram += to_string(pData.open_interest);
			datagram += "|";
			datagram += to_string((pData.bid_price1 > 10000000) ? 0 : pData.bid_price1);
			datagram += "|";
			datagram += to_string(pData.bid_volume1);
			datagram += "|";
			datagram += to_string((pData.ask_price1 > 10000000) ? 0 : pData.ask_price1);
			datagram += "|";
			datagram += to_string(pData.ask_volume1);
			datagram += "|";
			datagram += to_string((pData.bid_price2 > 10000000) ? 0 : pData.bid_price2);
			datagram += "|";
			datagram += to_string(pData.bid_volume2);
			datagram += "|";
			datagram += to_string((pData.ask_price2 > 10000000) ? 0 : pData.ask_price2);
			datagram += "|";
			datagram += to_string(pData.ask_volume2);
			datagram += "|";
			datagram += to_string((pData.bid_price3 > 10000000) ? 0 : pData.bid_price3);
			datagram += "|";
			datagram += to_string(pData.bid_volume3);
			datagram += "|";
			datagram += to_string((pData.ask_price3 > 10000000) ? 0 : pData.ask_price3);
			datagram += "|";
			datagram += to_string(pData.ask_volume3);
			datagram += "|";
			datagram += to_string((pData.bid_price4 > 10000000) ? 0 : pData.bid_price4);
			datagram += "|";
			datagram += to_string(pData.bid_volume4);
			datagram += "|";
			datagram += to_string((pData.ask_price4 > 10000000) ? 0 : pData.ask_price4);
			datagram += "|";
			datagram += to_string(pData.ask_volume4);
			datagram += "|";
			datagram += to_string((pData.bid_price5 > 10000000) ? 0 : pData.bid_price5);
			datagram += "|";
			datagram += to_string(pData.bid_volume5);
			datagram += "|";
			datagram += to_string((pData.ask_price5 > 10000000) ? 0 : pData.ask_price5);
			datagram += "|";
			datagram += to_string(pData.ask_volume5);
			datagram += "|";
			datagram += pData.update_time;
			datagram += "|";
			datagram += to_string(pData.update_millisec);
			datagram += "#";
			//
			m_pUdpSocket->SendTo(datagram, IP, PORT);
		}
	}

	void OnRspUtpLogin(const RspUtpLoginField_t& rsp,TSequenceIDType nRequestID)
	{
		cout << "OnRspUtpLogin:" << rsp.response_code << endl;
		if (Err_Succeed == rsp.response_code)
		{
			ConnectSocket();
			usleep(1000);
			subscribe();
		}
	}
	
	void ConnectSocket()
	{
		try
		{
			if(!m_pUdpSocket)
			{
				m_pUdpSocket = NULL;
			}
			usleep(1000);
			m_pUdpSocket = new UDPSocket<>(true);
			m_pUdpSocket->Connect(IP, PORT);
		}
		catch(exception& e)
		{
			cout << "Standard exception: " << e.what() << endl;
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
		cout << nRequestID << "|" << rsp.exchange_id << "|" << rsp.exchange_name << "|" << rsp.exchange_status << endl;
	}

	void OnRspQryInstrument(const RspQryInstrumentField_t& rsp,TSequenceIDType nRequestID)
	{
		cout << nRequestID << "|" << rsp.exchange_id << "|" << rsp.instrument_id << "|" << rsp.instrument_name << endl;

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
			if(rc == 16)
			{
				ConnectSocket();
			}
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
	// 指向UDPSocket实例的指针
	UDPSocket<> *m_pUdpSocket = NULL;
	vector<string> md_InstrumentID;
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
		// cout << "Enter q or Q to exit. " << endl;
		c = getchar();
	}

	delete testApi;
	return 0;
}

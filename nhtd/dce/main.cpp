#include "udpsocket.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <vector>
#include "QdFtdcMdApi.h"

using namespace std;
constexpr const char* IP = "47.98.117.222";
constexpr uint16_t PORT = 36888;

class CSimpleHandler : public CQdFtdcMduserSpi
{
public:
	CSimpleHandler(CQdFtdcMduserApi *pUserApi) : m_pUserMdApi(pUserApi) {
		
	}

	~CSimpleHandler() {
		if(m_pUserMdApi)
		{
			m_pUserMdApi->Release();
		}
	}
	
	void ReqServerConnect()
	{
		if(m_pUdpSocket)
		{
			m_pUdpSocket->Close();
		}
		m_pUdpSocket = new UDPSocket<>(true);
		m_pUdpSocket->Connect(IP, PORT);
	}
	
	void OnFrontConnected()
	{
		cout << "OnFrontConnected" << endl;
		ReqUserLogin();
	}

	void OnFrontDisconnected()
	{
		printf("OnFrontDisconnected.\n");
	}

	void ReqUserLogin()
	{
		CQdFtdcReqUserLoginField reqUserLogin;
		strcpy(reqUserLogin.TradingDay, m_pUserMdApi->GetTradingDay());
		strcpy(reqUserLogin.BrokerID, "2016");
		strcpy(reqUserLogin.UserID, "01020249");
		strcpy(reqUserLogin.Password, "830914zsz");
		int num = m_pUserMdApi->ReqUserLogin(&reqUserLogin, nRequestID++);
		cout << "ReqUserLogin:" << num << endl;
	}
	
	void SubscribeMarketData()
	{
		int md_num = 0;
		char **ppInstrumentID = new char*[5000];
		// read config
		vector<string> md_InstrumentID;
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
		cout << "SubscribeMarketData, size=" << md_InstrumentID.size() << endl;
		for (int count1 = 0; count1 <= md_InstrumentID.size() / 500; count1++)
		{
			if (count1 < md_InstrumentID.size() / 500)
			{
				int a = 0;
				for (a; a < 500; a++)
				{
					ppInstrumentID[a] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
					md_num++;
				}
				int result = m_pUserMdApi->SubMarketData(ppInstrumentID, a);
			}
			else if (count1 == md_InstrumentID.size() / 500)
			{
				int count2 = 0;
				for (count2; count2 < md_InstrumentID.size() % 500; count2++)
				{
					ppInstrumentID[count2] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
					md_num++;
				}
				int result = m_pUserMdApi->SubMarketData(ppInstrumentID, count2);
				cout << "SubscribeMarketData:" << result << endl;
			}
		}
	}

	void OnRspUserLogin(CQdFtdcRspUserLoginField *pRspUserLogin, CQdFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		if (pRspInfo->ErrorID == 0) {
			SubscribeMarketData();
			ReqServerConnect();
		}
	}

	void OnRtnDepthMarketData(CQdFtdcDepthMarketDataField *pDepthMarketData)
	{
		if (pDepthMarketData)
		{
			std::string datagram = "";
			datagram += pDepthMarketData->InstrumentID;
			datagram += "|";
			datagram += pDepthMarketData->ExchangeID;
			datagram += "|";
			datagram += to_string((pDepthMarketData -> LastPrice > 10000000) ? 0 : pDepthMarketData -> LastPrice);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> OpenPrice > 10000000) ? 0 : pDepthMarketData -> OpenPrice);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> HighestPrice > 10000000) ? 0 : pDepthMarketData -> HighestPrice);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> LowestPrice > 10000000) ? 0 : pDepthMarketData -> LowestPrice);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> UpperLimitPrice > 10000000) ? 0 : pDepthMarketData -> UpperLimitPrice);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> LowerLimitPrice > 10000000) ? 0 : pDepthMarketData -> LowerLimitPrice);
			datagram += "|";
			datagram += to_string(pDepthMarketData->Volume);
			datagram += "|";
			datagram += to_string(pDepthMarketData->OpenInterest);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> BidPrice1 > 10000000) ? 0 : pDepthMarketData -> BidPrice1);
			datagram += "|";
			datagram += to_string(pDepthMarketData->BidVolume1);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> AskPrice1 > 10000000) ? 0 : pDepthMarketData -> AskPrice1);
			datagram += "|";
			datagram += to_string(pDepthMarketData->AskVolume1);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> BidPrice2 > 10000000) ? 0 : pDepthMarketData -> BidPrice2);
			datagram += "|";
			datagram += to_string(pDepthMarketData->BidVolume2);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> AskPrice2 > 10000000) ? 0 : pDepthMarketData -> AskPrice2);
			datagram += "|";
			datagram += to_string(pDepthMarketData->AskVolume2);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> BidPrice3 > 10000000) ? 0 : pDepthMarketData -> BidPrice3);
			datagram += "|";
			datagram += to_string(pDepthMarketData->BidVolume3);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> AskPrice3 > 10000000) ? 0 : pDepthMarketData -> AskPrice3);
			datagram += "|";
			datagram += to_string(pDepthMarketData->AskVolume3);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> BidPrice4 > 10000000) ? 0 : pDepthMarketData -> BidPrice4);
			datagram += "|";
			datagram += to_string(pDepthMarketData->BidVolume4);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> AskPrice4 > 10000000) ? 0 : pDepthMarketData -> AskPrice4);
			datagram += "|";
			datagram += to_string(pDepthMarketData->AskVolume4);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> BidPrice5 > 10000000) ? 0 : pDepthMarketData -> BidPrice5);
			datagram += "|";
			datagram += to_string(pDepthMarketData->BidVolume5);
			datagram += "|";
			datagram += to_string((pDepthMarketData -> AskPrice5 > 10000000) ? 15 : pDepthMarketData -> AskPrice5);
			datagram += "|";
			datagram += to_string(pDepthMarketData->AskVolume5);
			datagram += "|";
			datagram += pDepthMarketData->UpdateTime;
			datagram += "|";
			datagram += to_string(pDepthMarketData->UpdateMillisec);
			datagram += "#";
			if(m_pUdpSocket)
			{
				m_pUdpSocket->SendTo(datagram, IP, PORT);
			}
		}
	}

	void OnRspError(CQdFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspError:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	}

private:
	CQdFtdcMduserApi *m_pUserMdApi;
	UDPSocket<> *m_pUdpSocket = nullptr;
	int nRequestID = 0;
};

int main()
{

	CQdFtdcMduserApi *pUserApi = CQdFtdcMduserApi::CreateFtdcMduserApi();
	CSimpleHandler *sh = new CSimpleHandler(pUserApi);
	pUserApi->RegisterSpi(sh);
	std::string mdAddr = "tcp://59.46.178.11:29895";
	pUserApi->RegisterFront(const_cast<char *>(mdAddr.c_str()));
	pUserApi->Init();

	// pUserApi->Join();
    pause();
    return 0;
}

#include "udpsocket.hpp"
#include "ThostFtdcMdApi.h"
#include "string.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
constexpr const char* IP = "47.98.117.222";
constexpr uint16_t PORT = 36888;
//行情类
class CSimpleMdHandler : public CThostFtdcMdSpi
{
public:
	// 构造函数，需要一个有效的指向CThostFtdcMduserApi实例的指针
	CSimpleMdHandler(CThostFtdcMdApi *pUserApi) : m_pUserMdApi(pUserApi) {}
	~CSimpleMdHandler() {
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

	void RegisterFensUserInfo()
	{
	}

	void ReqUserLogin()
	{
		CThostFtdcReqUserLoginField reqUserLogin = { 0 };
		std::string borkerID = "8050";
		std::string userID = "70211502";
		strcpy(reqUserLogin.BrokerID, borkerID.c_str());
		strcpy(reqUserLogin.UserID, userID.c_str());
		int num = m_pUserMdApi->ReqUserLogin(&reqUserLogin, nRequestID++);
		cout << "ReqUserLogin:" << num << endl;
	}

	void ReqUserLogout()
	{
		cout << "ReqUserLogout" << endl;
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
				int result = m_pUserMdApi->SubscribeMarketData(ppInstrumentID, a);
			}
			else if (count1 == md_InstrumentID.size() / 500)
			{
				int count2 = 0;
				for (count2; count2 < md_InstrumentID.size() % 500; count2++)
				{
					ppInstrumentID[count2] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
					md_num++;
				}
				int result = m_pUserMdApi->SubscribeMarketData(ppInstrumentID, count2);
				cout << "SubscribeMarketData:" << result << endl;
			}
		}
	}
	
	///订阅询价请求
	void SubscribeForQuoteRsp()
	{
	}

	void UnSubscribeMarketData()
	{
	}

	void UnSubscribeForQuoteRsp()
	{
	}

	void ReqQryMulticastInstrument()
	{
	}

	virtual void OnFrontConnected()
	{
		cout << "OnFrontConnected" << endl;
		ReqUserLogin();
	}

	virtual void OnHeartBeatWarning(int nTimeLapse)
	{
		cout << "OnHeartBeatWarning, nTimeLapse=" << nTimeLapse << endl;
	}

	// 当客户端与交易托管系统通信连接断开时，该方法被调用
	virtual void OnFrontDisconnected(int nReason)
	{
		cout << "OnFrontDisconnected, nReason=" << nReason << endl;
	}

	// 当客户端发出登录请求之后，该方法会被调用，通知客户端登录是否成功
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cout << "ErrorMsg=" << pRspInfo->ErrorID << ",ErrorID=" << pRspInfo->ErrorMsg;
		if (pRspInfo->ErrorID == 0) {
			SubscribeMarketData();
			ReqServerConnect();
		}
	}

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
	}

	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
	};

	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
	};

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
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
	};


	virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
	};

	virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
	};

	///询价通知
	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
	{
	}

	virtual void OnRspQryMulticastInstrument(CThostFtdcMulticastInstrumentField *pMulticastInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
	};

private:
	// 指向CThostFtdcMduserApi实例的指针
	CThostFtdcMdApi *m_pUserMdApi;
	// 指向UDPSocket实例的指针
	UDPSocket<> *m_pUdpSocket = nullptr;
	int nRequestID = 0;
};

int main()
{
    CThostFtdcMdApi *pUserMdApi = CThostFtdcMdApi::CreateFtdcMdApi(".\\flow\\", false, false);
	CSimpleMdHandler ash(pUserMdApi);
	pUserMdApi->RegisterSpi(&ash);
	std::string mdAddr = "tcp://180.169.30.170:41215";
	pUserMdApi->RegisterFront(const_cast<char *>(mdAddr.c_str()));
	pUserMdApi->Init();
    /*
    string input;
    getline(cin, input);
    while (input != "exit")
    {
        getline(cin, input);
    }
    */
    pause();
    return 0;
}

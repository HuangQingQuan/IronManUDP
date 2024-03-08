#include "tcpsocket.hpp"
#include "ThostFtdcMdApi.h"
#include "string.h"
#include <string>
#include <iostream>
#include <vector>

using namespace std;

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
		m_pTcpSocket->Close();
		delete m_pTcpSocket;
	}
	
	vector<string> Split(const string &str, const string &pattern)
	{
		//const char* convert to char*
		char * strc = new char[strlen(str.c_str()) + 1];
		strcpy(strc, str.c_str());
		vector<string> resultVec;
		char* tmpStr = strtok(strc, pattern.c_str());
		while (tmpStr != NULL)
		{
			resultVec.push_back(string(tmpStr));
			tmpStr = strtok(NULL, pattern.c_str());
		}
		delete[] strc;
		return resultVec;
	}
	
	void ReqServerConnect()
	{
		m_pTcpSocket = new TCPSocket<>([](int errorCode, std::string errorMessage){
			cout << "Socket creation error:" << errorCode << " : " << errorMessage << endl;
		});

		m_pTcpSocket->onRawMessageReceived = [](const char* message, int length) {
			cout << "Message from the Server: " << message << "(" << length << ")" << endl;
		};
		m_pTcpSocket->onSocketClosed = [](int errorCode){
			cout << "Connection closed: " << errorCode << endl;
		};
		m_pTcpSocket->Connect("127.0.0.1", 36888, [&] {
			cout << "Socket Connected" << endl;
		},
		[](int errorCode, std::string errorMessage){
			cout << errorCode << " : " << errorMessage << endl;
		});
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
		int num = m_pUserMdApi->ReqUserLogin(&reqUserLogin, 2);
		cout << "ReqUserLogin:" <<num << endl;
	}

	void ReqUserLogout()
	{
		cout << "ReqUserLogout" << endl;
	}

	void SubscribeMarketData()//收行情
	{
		int md_num = 0;
		char **ppInstrumentID = new char*[5000];
		std::string instruments = "ss2407:ss2406:ss2409:ss2403:ss2412:ss2411:ss2408:ss2404:ss2501:ss2402:rb2403:rb2410:rb2501:rb2408:rb2411:rb2409:hc2501:hc2405:hc2411:hc2402:hc2409:hc2408:hc2406:hc2404:hc2407:hc2403:hc2412:hc2410:sn2407:sn2411:sn2410:sn2405:sn2403:sn2404:sn2501:sn2408:sn2409:sn2406:sn2402:sn2412:ru2406:ru2404:ru2408:ru2411:ru2410:ru2407:ru2405:ru2409:ru2403:ru2501:ss2405:ss2410:ag2407:ag2405:ag2404:ag2412:al2410:al2404:al2407:al2402:al2411:al2403:al2408:al2406:al2409:al2412:al2501:al2405:rb2404:rb2412:rb2407:rb2402:rb2405:rb2406:fu2407:fu2412:fu2405:fu2408:fu2409:fu2411:fu2406:fu2403:fu2501:fu2404:fu2410:fu2402:cu2403:cu2410:cu2409:cu2402:cu2501:cu2405:cu2407:cu2404:cu2408:cu2411:cu2412:cu2406:zn2410:zn2411:zn2406:zn2501:zn2404:zn2403:zn2409:zn2407:zn2412:zn2408:zn2402:zn2405:wr2404:wr2412:wr2410:wr2402:wr2407:wr2403:wr2405:wr2409:wr2406:wr2501:wr2411:wr2408:ag2411:ag2406:ag2403:ag2501:ag2408:ag2409:ag2410:ag2402:ni2408:ni2405:ni2412:ni2409:ni2406:ni2403:ni2404:ni2501:ni2402:ni2407:ni2411:ni2410:pb2405:pb2407:pb2404:pb2410:pb2411:pb2409:pb2403:pb2406:pb2501:pb2402:pb2412:pb2408:bc2404:bc2412:bc2408:bc2411:bc2406:bc2405:bc2402:bc2407:bc2403:bc2409:bc2501:bc2410:sp2402:sp2409:sp2403:sp2406:sp2404:sp2407:sp2412:sp2405:sp2501:sp2408:sp2411:sp2410:bu2506:bu2407:bu2512:bu2501:bu2404:bu2503:bu2410:bu2405:bu2403:bu2409:bu2408:bu2402:bu2412:bu2509:bu2411:bu2406:nr2406:nr2403:nr2405:nr2501:nr2407:nr2408:nr2404:nr2409:nr2411:nr2410:nr2402:nr2412:sc2405:sc2406:sc2403:sc2506:sc2501:sc2408:sc2603:sc2612:sc2509:sc2410:sc2606:sc2503:sc2512:sc2404:sc2411:sc2409:sc2412:sc2609:sc2407:sc2402:au2502:au2403:au2408:au2404:au2412:au2402:au2410:au2406:lu2409:lu2501:lu2404:lu2412:lu2403:lu2407:lu2411:lu2408:lu2406:lu2405:lu2410:lu2402:br2402:br2412:br2406:br2501:br2410:br2403:br2405:ec2412:ec2410:ec2408:ec2406:ec2404:ao2407:ao2405:ao2412:ao2501:ao2410:ao2409:ao2408:ao2406:ao2402:ao2411:ao2404:ao2403:br2409:br2407:br2408:br2404:br2411";
		vector<string> md_InstrumentID = Split(instruments, ":");
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
		if(m_pTcpSocket)
		{
			m_pTcpSocket->Close();
		}
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
			m_pTcpSocket->Send(datagram);
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
	// 指向TCPSocket实例的指针
	TCPSocket<> *m_pTcpSocket;
};

int main()
{
    CThostFtdcMdApi *pUserMdApi = CThostFtdcMdApi::CreateFtdcMdApi(".\\flow\\", false, false);
	CSimpleMdHandler ash(pUserMdApi);
	pUserMdApi->RegisterSpi(&ash);
	std::string mdAddr = "tcp://180.169.30.170:41215";
	pUserMdApi->RegisterFront(const_cast<char *>(mdAddr.c_str()));
	pUserMdApi->Init();

    string input;
    getline(cin, input);
    while (input != "exit")
    {
        getline(cin, input);
    }

    return 0;
}

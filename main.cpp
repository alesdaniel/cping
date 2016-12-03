/*
 *  This file is part of cping.
 * 
 *  cping is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  cping is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with bcsv.  If not, see <http://www.gnu.org/licenses/>.
 */
/* 
 * File:   main.cpp
 * Author: Ales Daniel <alesdaniel77@gmail.com>
 *
 * Created on 10 de noviembre de 2016, 18:42
 * basado en http://poco.roundsquare.net/2011/03/20/poconeticmpclient/
 */
 
#include <Poco/Format.h>
#include <Poco/Net/ICMPClient.h>
#include <Poco/Net/IPAddress.h>
#include <Poco/Net/ICMPSocket.h>
#include <Poco/Net/ICMPEventArgs.h>
#include <Poco/Delegate.h>
#include <Poco/Exception.h>

#include <Poco/DOM/AutoPtr.h>
#include <Poco/Util/IniFileConfiguration.h>


#include <string>
#include <iostream>

using namespace std;
using Poco::AutoPtr;
using Poco::Util::IniFileConfiguration;

class cping
{
    public:
	cping(Poco::Net::ICMPClient& client): //constructor
                p_client	(client) //inicializa
	{
            p_client.pingBegin += Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onBegin);                    
            p_client.pingReply += Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onReply);
	    p_client.pingError += Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onError);            
            p_client.pingEnd   += Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onEnd);            
	}
	~cping() //destructor
	{
            p_client.pingBegin -= Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onBegin);                    
            p_client.pingReply -= Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onReply);
	    p_client.pingError -= Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onError);                        
            p_client.pingEnd   -= Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onEnd);                        
	}
    void onBegin(const void* pSender, Poco::Net::ICMPEventArgs& args)
    {
      cout << "Iniciando" << endl;
    }
    void onReply(const void* pSender, Poco::Net::ICMPEventArgs& args)
    {
      cout << "Respuesta Host " << args.hostName() << " Tam " << args.dataSize() << " Tiempo " << args.replyTime() << " TTL " <<	args.ttl() << endl;      
    }    
    void onError(const void* pSender, Poco::Net::ICMPEventArgs& args)
    {
	cout << Poco::format("Error: %s ", args.error()) << endl;
    }
    void onEnd(const void* pSender, Poco::Net::ICMPEventArgs& args)
    {
    /*	int received = args.received();
	m_msg.Message(Poco::format("  Ping statistics for %s", args.hostName()));
	m_msg.Message(Poco::format("   Packets: Sent=%d, Received=%d, Lost=%d (%3.3f%% loss)",
			args.sent(),
			received,
			args.repetitions() - received,
			100.0 - args.percent()));
	m_msg.Message(Poco::format("   Approximate round trip times: Minimum=%dms, Maximum=%dms, Average=%dms",
			args.minRTT(),
			args.maxRTT(),
			args.avgRTT()));*/
    }    
    int ping(Poco::Net::SocketAddress& address, int repeat) const
    {
	return p_client.ping(address, repeat);
    }

    private:
        
    protected:    
        Poco::Net::ICMPClient&	p_client;
};

int main(int /*argc*/, char** /*argv*/)
{
    
try 
   {    
    Poco::Net::ICMPClient icmpClient(Poco::Net::IPAddress::IPv4);
//  Poco::Net::SocketAddress socketAddress("192.168.1.49:80");
    Poco::Net::SocketAddress socketAddress;  
    
    AutoPtr<IniFileConfiguration> pConf(new IniFileConfiguration("maquinas.txt"));	
   
    //std::string path = pConf->hasProperty("MyApplication");
    std::string secc;
    std::string ips;
    std::string maq;
    IniFileConfiguration::Keys keys;
    IniFileConfiguration::Keys keys1;
    pConf->keys(keys);
    pConf->keys(keys1);

    pConf->keys("sec", keys);
    cout << keys.size() << endl;
    for (int c = 0;c < keys.size();c++) // escanea las secciones 
    { 
        cout << keys.at(c).data() << endl;
    //   secc = "sec." + keys.at(c);
        secc = "sec.";
        cout << " +" <<  pConf->getString(secc + keys.at(c)) << endl;
        ips = pConf->getString(secc + keys.at(c));
    // cout << pConf->getString(ips);
        pConf->keys(ips, keys1); 
        for (int c1 = 0;c1 < keys1.size();c1++)
        {
           maq = ips +"."+ keys1.at(c1);
        cout  << "  +" << pConf->getString(maq) << endl; 
        socketAddress =  Poco::Net::SocketAddress(pConf->getString(maq)+":80");
        cping p(icmpClient);    
        p.ping(socketAddress,5);
        }   
    }    
  
    }
catch(Poco::IOException& e)
    {
    cout << "Error :" << e.displayText() << endl;
    return -1; 
    }    
    return 0;
}

/*Poco::XML::InputSource src("maquinas.xml");*/
	
/*try       
    {    
    Poco::Net::ICMPClient icmpClient(Poco::Net::IPAddress::IPv4);
//  Poco::Net::SocketAddress socketAddress("192.168.1.49:80");
    Poco::Net::SocketAddress socketAddress;

    Poco::XML::DOMParser parser;
    Poco::XML::AutoPtr<Poco::XML::Document> pDoc = parser.parse(&src);
		
    //Poco::XML::NodeIterator it(pDoc, Poco::XML::NodeFilter::SHOW_ALL);
    Poco::XML::NodeIterator it(pDoc, Poco::XML::NodeFilter::SHOW_ALL );    
    Poco::XML::Node* pNode = it.nextNode();
    
    while (pNode)
    {
        //if (pNode->nodeName()[0] != '#') 
        //if (!pNode->nodeValue().empty())
    //    if (pNode->nodeValue() == "Maquina") // pNode->nodeType() -> Nivel
        
	   std::cout << pNode->nodeName() << ":" << pNode->nodeValue() << std::endl;
/*     if (pNode->nodeName() == "ip")
        {
        pNode = it.nextNode(); //siguiente nodo          
        std::cout << "ip:" << pNode->nodeValue() << std::endl;               
        }*/
    /*pNode = it.nextNode();
    }*/
 /*   return 0;
    
    socketAddress =  Poco::Net::SocketAddress("192.168.1.49:80");
    cping p(icmpClient);    
    p.ping(socketAddress,5);
    }
catch(Poco::IOException& e)
    {
    cout << "Error :" << e.displayText() << endl;
    return -1; 
    }    
    return 0;
}
*/
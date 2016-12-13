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

#include <curses.h>
#include <panel.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using Poco::AutoPtr;
using Poco::Util::IniFileConfiguration;

class cping
{
    public:
        int error;         
	cping(Poco::Net::ICMPClient& client): //constructor
                p_client	(client) //inicializa
	{
            p_client.pingBegin += Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onBegin);                    
            p_client.pingReply += Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onReply);
	    p_client.pingError += Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onError);            
            p_client.pingEnd   += Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onEnd);            
            iniciascr();
	}
	~cping() //destructor
	{
            p_client.pingBegin -= Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onBegin);                    
            p_client.pingReply -= Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onReply);
	    p_client.pingError -= Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onError);                        
            p_client.pingEnd   -= Poco::Delegate<cping, Poco::Net::ICMPEventArgs>(this, &cping::onEnd);                        
            cierrascr();
	}
    void onBegin(const void* pSender, Poco::Net::ICMPEventArgs& args)
    {
    //  cout << "Iniciando" << endl;
    }
    void onReply(const void* pSender, Poco::Net::ICMPEventArgs& args)
    {
    //  linl++;
      //mvwprintw(boxl, linl, 1, "Resp: %s Tam %d Tie %d TTL %d \n", args.hostName().c_str(), args.dataSize(),args.replyTime(),args.ttl());
      wprintw(boxl, " Resp: %s Tam %d Tie %d TTL %d \n", args.hostName().c_str(), args.dataSize(),args.replyTime(),args.ttl());
      box(boxl, 0, 0);    
      update_panels();
      doupdate();
      if (error < 0) 
          error = 0;
      error++;
      //  cout << "Respuesta Host " << args.hostName() << " Tam " << args.dataSize() << " Tiempo " << args.replyTime() << " TTL " <<	args.ttl() << endl;      
    }    
    void onError(const void* pSender, Poco::Net::ICMPEventArgs& args)
    {
  //    line++;
     // mvwprintw(boxe, line, 1, "Error: %s \n",args.error().c_str());
      wprintw(boxe," Error: %s \n",args.error().c_str());
      box(boxe, 0, 0);
      update_panels();
      doupdate();              
      error--;
     //cout << Poco::format("Error: %s ", args.error()) << endl;
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
    void muestra_p(std::string dato)
    {
 //    linp++;   
   //  mvwprintw(boxp, linp, 1, "%s", dato.c_str());
     wprintw(boxp, " %s \n", dato.c_str());
     box(boxp, 0, 0);     
    }

    private:
        WINDOW *boxp, *boxl, *boxe;    
        PANEL  *panelp, *panell, *panele;
 //       int linp = 0, linl = 0, line = 0;
    void iniciascr()
    {   
        initscr();
        cbreak();
        noecho();
	boxp = newwin(25, 30, 0, 0);
	boxl = newwin(12, 50, 0, 30);
	boxe = newwin(13, 50, 12, 30);;
        
        box(boxp, 0, 0);
        box(boxl, 0, 0);
        box(boxe, 0, 0);
    
        scrollok(boxp,TRUE);
        scrollok(boxl,TRUE);
        scrollok(boxe,TRUE);
    
        panelp = new_panel(boxp);
        panell = new_panel(boxl);    
        panele = new_panel(boxe);
        
        update_panels();
        doupdate();
        top_panel(panelp);      
    }
    
    void cierrascr()
    {
    getch();    
    delwin(boxp);
    delwin(boxl);
    delwin(boxp);
    endwin();    
    }
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
    cping p(icmpClient);  
    
    AutoPtr<IniFileConfiguration> pConf(new IniFileConfiguration("maquinas.txt"));	
    //std::string path = pConf->hasProperty("MyApplication");
    std::string secc;
    std::string ips;
    std::string maq;
    std::string msgerror;
    IniFileConfiguration::Keys keys;
    IniFileConfiguration::Keys keys1;
    pConf->keys(keys);
    pConf->keys(keys1);

    pConf->keys("sec", keys);
//    cout << keys.size() << endl;
    for (int c = 0;c < keys.size();c++) // escanea las secciones 
    { 
     //   cout << keys.at(c).data() << endl;
        p.muestra_p(keys.at(c).data());
    //   secc = "sec." + keys.at(c);
        secc = "sec.";
        p.error = 0;        
       //cout << " +" <<  pConf->getString(secc + keys.at(c)) << endl;
        p.muestra_p(" +" + pConf->getString(secc + keys.at(c)));
            
        ips = pConf->getString(secc + keys.at(c));
    // cout << pConf->getString(ips);
        pConf->keys(ips, keys1); 
        for (int c1 = 0;c1 < keys1.size();c1++)
        {
           maq = ips +"."+ keys1.at(c1);
        //cout  << "  +" << pConf->getString(maq) << endl;
        if (keys1.at(c1) == "err") {
    //        p.muestra_p(" --Err" + pConf->getString(maq));
           msgerror = pConf->getString(maq);
        } else
        {
          p.muestra_p("  +" + pConf->getString(maq));        
          socketAddress =  Poco::Net::SocketAddress(pConf->getString(maq)+":80");        
          p.ping(socketAddress,5);
        }  
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        if (p.error < 0)
        {
          cout << "Error en: "<<  msgerror << " <" << p.error << "> " << endl; 
          return 1;
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
#ifndef GLP_SERVERSOCKET_H_
#define GLP_SERVERSOCKET_H_

#include "eventhandler.h"
#include "seqmap.h"
#include "clientsocket.h"

namespace Tool{
	class DataProcessBase;
	class Reactor;
	class Counter;
	typedef SeqMap<ClientSocket> ClientMap;

	class ListenSocketBase : public FDEventHandler
	{
	public:
		ListenSocketBase(int port,Reactor *pReactor,int timeout = 3)
			:FDEventHandler(pReactor),m_port(port)
		  {
				  m_timeout = timeout;
				  if(m_timeout > 2100)
					  m_timeout = 2100;
		  }
		  virtual ~ListenSocketBase(){}

		  int Listen();
		  virtual void OnFDRead();
		  virtual void OnAccept(int fd) = 0;
		  virtual void OnFDWrite(){}
		  
	protected:
		int m_port;
		int m_timeout;
	};

	/*
	class ListenSocketBase2 : public ListenSocketBase
	{
	public:
		ListenSocketBase2(int port,Reactor *pReactor,Counter *pCounter
			,DataProcessBase *pDecoder,ClientMap *pClientMap,int timeout = 3) 
			: ListenSocketBase(port,pReactor,timeout)
			,m_pCounter(pCounter)
			,m_pDecoder(pDecoder)
			,m_pClientMap(pClientMap){
		  }
		  virtual void OnAccept(int fd);
		  virtual ClientSocket* CreateClient() = 0;
	private:
		Counter *m_pCounter;
		DataProcessBase *m_pDecoder;
		ClientMap *m_pClientMap;
	};

	/*
	class ListenSocket : public ListenSocketBase2,public ObjectManager<ClientSocket>
	{
	public:
		ListenSocket(int port,Reactor *pReactor,ObjectAllocator<ClientSocket> * pObjectAllocator
			,Counter *pCounter,DataDecoderBase *pDecoder,ClientMap *pClientMap,int timeout = 3) :
		  ListenSocketBase2(port,pReactor,pCounter,pDecoder,pClientMap,timeout),
			  ObjectManager<ClientSocket>(pObjectAllocator)
		  {}
		  virtual ClientSocket* CreateClient()
		  {
			  return Create();
		  }
		  virtual ~ListenSocket(){}
	};
	

	class UdpListenSocket : public FDEventHandler
	{
	public:
		UdpListenSocket(Reactor *pReactor,int port) 
			:FDEventHandler(pReactor),m_port(port)
		{
		}
		int Listen();
		virtual void OnFDRead();
		virtual void OnFDWrite(){}
	private:
		int m_port;
	};
	*/
	
}

#endif//GLP_SERVERSOCKET_H_

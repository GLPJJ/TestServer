#include "clientsocket.h"
#include "dataprocess.h"
#include "log.h"

namespace Tool
{
	ClientUdpSocket::ClientUdpSocket()
		:m_port()
	{
	}

	ClientUdpSocket::ClientUdpSocket(Reactor *pReactor,const char* host,short port)
		: FDEventHandler(pReactor)
		,m_port(port)
	{
		strncpy(m_host,host,sizeof(m_host));
	}

	int ClientUdpSocket::Init()
	{
		m_fd = socket(AF_INET,SOCK_DGRAM,0);
		if(m_fd == INVALID_SOCKET)
			return -1;

		int i = 100;
		while(i-- > 0)
		{
			struct sockaddr_in local={0};
			local.sin_family = AF_INET;
			local.sin_port = htons(m_port); ///监听端口
			local.sin_addr.s_addr = inet_addr("127.0.0.1"); ///本机
			if(bind(m_fd,(struct sockaddr*)&local,sizeof(local)) == SOCKET_ERROR)
			{
				m_port++;
				continue;
			}
			else
				break;
		}
		if(i <= 0)
			return -1;

		registerRead();
		return 0;
	}

	//重置网络描述符的状态
	void ClientSocketBase::reset()
	{

	}

	void ClientSocketBase::onFDRead()
	{
		char buf[16384] = {0};/* 16*1024 */
		int len = ::recv(m_fd, buf, sizeof(buf), 0);//接收网络数据

		if (len == 0)
		{
			onSocketClose();
			close();
			return;
		}

		if (len == SOCKET_ERROR)
		{
#ifdef WIN32
			DWORD derrno = GetLastError();
			if (derrno != WSAEWOULDBLOCK)
			{
				onSocketRecvError(derrno);
				close();
			}
#else//Linux
            int errorcode = errno;
			if(errorcode!=EAGAIN)
			{
				onSocketRecvError(errorcode);
				close();
			}
#endif
			return;
		}

		if (!m_pDecoder)
			return;

		if (m_recvdata.append(buf,len) != len)
		{
			onNetLevelError(EC_RECV_BUFFER);
			close();
			return;
		}
		//解析网络数据，不足一个包的部分会有相应处理
		if (m_pDecoder->process(this) < 0)
		{
			onNetLevelError(EC_STREAM);
			close();
			return;
		}
	}
	void ClientSocketBase::onFDWrite()
	{
        MutexScoped lock(m_pCSSendData);
		size_t buflen = m_senddata.getPos();
		int len = ::send(m_fd,m_senddata.getBuf(),(int)buflen,0);
		if (len == SOCKET_ERROR)
		{
#ifdef WIN32
			DWORD derrno = GetLastError();
			if( derrno != WSAEWOULDBLOCK )
			{
				close();
				onSocketSendError(derrno);
			}
#else//Linux
			if(errno != EAGAIN)
			{
				close();
				onSocketSendError(errno);
			}
#endif
			return;
		}
		if((size_t)len == buflen)
		{
			m_senddata.initPos();
			unRegisterWrite();
			return;
		}
		else if((size_t)len < buflen)
		{
			m_senddata.copy(0,m_senddata.getBuf() + len , buflen - len);
		}
	}
	int ClientSocketBase::addBuf(const char* buf,size_t buflen)
	{
        MutexScoped lock(m_pCSSendData);
		if(m_senddata.append(buf,buflen) != buflen)
		{
            LOGE("%s : SendData Append Failed\n",__FUNCTION__);
			return -1;
		}
		registerWrite();
		return 0;
	}
	const char* ClientSocketBase::getPeerIp()
	{
		return GetPeerIp(m_fd);
	}
	const char* ClientSocketBase::GetIpv4FromHostname(const char* name)
	{
		static char sIp[300] = {0};
		memset(sIp,0,sizeof(sIp));
		if(!name)
			return sIp;
		// 返回地址信息
		hostent* host = gethostbyname(name);
		if (!host)
		{
			LOGE("%s : gethostbyname return null[%s], check the internet permission or internet connect.\n",__FUNCTION__,name);
			return sIp;
		}

		// 解析地址信息
		for (char **p = host->h_addr_list; *p ; p++)
		{
			char *temp;
			temp = inet_ntoa(*(struct in_addr*)*p);
			strcpy(sIp,temp);
			break;
		}
		return sIp;
	}

	const char* ClientSocketBase::GetPeerIp(int fd)
	{
		sockaddr_in addr;
#ifdef NETUTIL_MAC
		size_t len = sizeof(sockaddr_in);
#else
		int len = sizeof(sockaddr_in);
#endif
		getpeername(fd,(struct sockaddr*)&addr,&len);
		static char ip[32] = {0};
		strncpy(ip,inet_ntoa(addr.sin_addr),sizeof(ip));
		return ip;
	}

	void ClientSocketBase::close()
	{
		m_bIsClosed = true;
		m_recvdata.initPos();
		m_senddata.initPos();
		FDEventHandler::close();
	}
/////////////////////////ClientSocket 实现/////////////////////////////////////////////////
	void ClientSocket::onTimeOut()
	{
		switch(m_waitType){
			case wait_for_connect:
				{
					onSocketConnectTimeout();
					ClientSocketBase::close();
					break;
				}
			case wait_for_write:
				{
					Log("客户端写socket超时... fd=%d\n",m_fd);
					break;
				}
				
		}
		m_waitType = wait_for_none;
		
	}
	void ClientSocket::close()
	{
		/*
			有可能Connect的时候没有网，网卡禁用的时候Connect居然返回成功，
			这时候需要注销超时事件,但是既没有连接上，也没有在等待连接完成，所以去掉这个if语句
		*/
		//if (m_isConnected || m_isWaitingConnectComplete)
		{
			m_isConnected = false;
			m_waitType = wait_for_none;
			ClientSocketBase::close();
			TMEventHandler::close();
		}
	}
	
	void ClientSocket::onFDWrite()
	{
		if(m_isConnected)//如果已经连接上了
		{
			ClientSocketBase::onFDWrite();
			return;
		}

		Log("客户端连接服务器成功 fd=%d\n",m_fd);
		//如果是第一次回调，则说明网络描述符可用了
		m_isConnected = true;
		m_waitType = wait_for_none;
		open();
		unRegisterTimer();//注销timer，从连接超时列表中去除
		unRegisterWrite();//注销写fd_set

		if(onSocketConnect())
			registerRead();
	}

	int ClientSocket::connect(const char* host,short port,int to)
	{
		//已经连接，或者正在连接
		if (m_isConnected || m_waitType == wait_for_connect)
			return 0;
		//创建网络描述符 TCP
		m_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_fd == INVALID_SOCKET)
			return -1;

		strncpy(m_host, host, sizeof(m_host));
		m_port = port;
		//定义连接地址IP PORT
		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(host);
		clientService.sin_port = htons(port);

		//设置非阻塞
		if (setNonBlocking() != 0)
			return -1;

		int errorCode;
#ifdef WIN32
		if (::connect(m_fd, (struct sockaddr*)&clientService, sizeof(clientService)) == SOCKET_ERROR 
			&& !(((errorCode = WSAGetLastError()) == WSAEWOULDBLOCK) || (errorCode==WSAEINPROGRESS)))
		{
#else
		if(::connect(m_fd, (struct sockaddr*)&clientService, sizeof(clientService)) == -1 
			&& !(((errorCode=errno) == EAGAIN) || (errorCode == EINPROGRESS)))
		{
#endif
			onSocketConnectError(errorCode);
			closeSocket();
			return -1;
		}

		Log("客户端等待服务器连接 fd=%d\n",m_fd);
		//如果为connect 返回-1 并且errorno为 EAGAIN
		m_waitType = wait_for_connect;
		registerWrite();//注册到写fd_set中 等待 OnFDWrite回调，第一次的话就说明这个fd可用了。
		registerTimer(to);//注册超时处理

		return 0;
	}

	bool ClientSocket::sendBuf(WriteStream &stream)
	{
		return sendBuf(stream.getData(),stream.getSize());
	}

	bool ClientSocket::sendBuf(const char* buf,size_t buflen)
	{
		if(addBuf(buf,buflen) != 0)
		{
			close();
			return false;
		}
		return true;
	}
};


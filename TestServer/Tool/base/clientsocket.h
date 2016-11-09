#ifndef GLP_CLIENTSOCKET_H_
#define GLP_CLIENTSOCKET_H_

#include "eventhandler.h"
#include "datablock.h"
#include "rwstream.h"
#include "mutex.h"

namespace Tool
{
	enum _Errorcode{
		//�������Buf�쳣
		EC_RECV_BUFFER,
		//�������쳣
		EC_STREAM,
	};

	class DataBlock;
	class DataDecoderBase;

	class UdpSocket : public FDEventHandler
	{
	public:
		virtual ~UdpSocket(){}
		UdpSocket():m_port(){}
		UdpSocket(Reactor *pReactor,const char* host,short port) : FDEventHandler(pReactor),m_port(port)
		{
			strncpy(m_host,host,sizeof(m_host));
		}
		virtual void onFDWrite(){}
		virtual int Init()
		{
			m_fd = socket(AF_INET,SOCK_DGRAM,0);
			if(m_fd == INVALID_SOCKET)
				return -1;

			int i = 100;
			while(i-- > 0)
			{
				struct sockaddr_in local={0};
				local.sin_family = AF_INET;
				local.sin_port = htons(m_port); ///�����˿�
				local.sin_addr.s_addr = inet_addr("127.0.0.1"); ///����
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
		short Getport(){
			return m_port;
		}
	private:
		short m_port;
		char m_host[20];
	};

	//////////////////////////////////////////////////////////////////////////

	class ClientSocketBase : public FDEventHandler
	{
	public:
		virtual ~ClientSocketBase(){if(m_pCSSendData)delete m_pCSSendData;}
        
		ClientSocketBase() : m_pDecoder(NULL),m_pCSSendData(NULL),m_bIsClosed(true){
            m_pCSSendData = Mutex::CreateCriticalSection();
        }
		ClientSocketBase(Reactor *pReactor) : FDEventHandler(pReactor),m_pDecoder(NULL),m_bIsClosed(true){
            m_pCSSendData = Mutex::CreateCriticalSection();
        }
		void setDecoder(DataDecoderBase* pDecoder){m_pDecoder = pDecoder;}
		//����ɶ���ʱ��recv����
		virtual void onFDRead();
		//�����д��ʱ��send����
		virtual void onFDWrite();
		//�رմ���
		virtual void closeSocket();

		inline bool isClosed(){return m_bIsClosed;}
		DataBlock* getRB(){return &m_recvdata;}
		DataBlock* getWB(){return &m_senddata;}
		//add buffer��ʱ��ע��дfd_set���������Ա�ִ�е�OnFDWrite();
		virtual int addBuf(const char* buf,unsigned int buflen);
		char* getPeerIp();

		//�������н�����Ip��ַ,ֻ���ص�һ������������,�ַ��������ھ�̬�ռ��У�����ֵ����Ҫ�ͷţ�
		static const char* GetIpv4FromHostname(const char* name);
	protected:
		void open(){m_bIsClosed = false;}
	public:
		// ���ӳɹ�
		virtual bool onSocketConnect() = 0; 
		// ���ӳ�ʱ
		virtual void onSocketConnectTimeout() = 0;
		// �����ر�(�����ر�),recv == 0�����
		virtual void onSocketClose() = 0;
		// errcodeΪ������(socket�ṩ)
		virtual void onSocketConnectError(int errCode) = 0;
		virtual void onSocketRecvError(int errCode) = 0;
		virtual void onSocketSendError(int errCode) = 0;
		// ��������(errCode����㶨��)
		virtual void onNetLevelError(int errCode) = 0;

	private:
		DataBlock m_recvdata;
		DataBlock m_senddata;
		DataDecoderBase *m_pDecoder;
		bool m_bIsClosed;
    public:
        Mutex* m_pCSSendData;
	};
	class ClientSocket : public ClientSocketBase ,public TMEventHandler
	{
	public:
		virtual ~ClientSocket(){}
		ClientSocket(Reactor *pReactor) : ClientSocketBase(pReactor),m_isConnected(false),
			m_isWaitingConnectComplete(false),m_port(0){ }

		//���ӶԷ�����������������жϡ�
		virtual void onFDWrite();
		//���ӳ�ʱ
		virtual void onTimeOut();
		virtual void closeSocket();

		int Connect(const char* host,short port,int to = 10);
		bool IsConnect(){return m_isConnected;}
		//��ӵ�sendbuf�У���ע�ᵽдfd_set��
		bool SendBuf(BinaryWriteStream &stream);
		bool SendBuf(const char* buf,unsigned int buflen);

		inline const char* Gethost(){return m_host;}
		inline short Getport(){return m_port;}
	public:
		virtual bool onSocketConnect() {return true;} 
		virtual void onSocketConnectTimeout() {}
		virtual void onSocketConnectError(int errCode) {}
		virtual void onSocketClose() {}
		virtual void onSocketRecvError(int errCode) {}
		virtual void onSocketSendError(int errCode) {}
		virtual void onNetLevelError(int errCode) {}

	private:
		bool m_isConnected;
		bool m_isWaitingConnectComplete;
		char m_host[20];
		short m_port;
	};
};

#endif//GLP_CLIENTSOCKET_H_


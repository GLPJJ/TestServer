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
	class DataProcessBase;

	class ClientUdpSocket : public FDEventHandler
	{
	public:
		ClientUdpSocket();
		ClientUdpSocket(Reactor *pReactor,const char* host,short port);
		virtual ~ClientUdpSocket(){}

	public:
		virtual void onFDWrite(){}
		virtual int Init();
		short getport(){return m_port;}

	private:
		short m_port;
		char m_host[20];
	};

	class ClientSocketBase : public FDEventHandler
	{
	public:
		ClientSocketBase() : m_pDecoder(NULL),m_bIsClosed(true){
			m_pCSSendData = Mutex::CreateCriticalSection();
		}
		ClientSocketBase(Reactor *pReactor) : FDEventHandler(pReactor),m_pDecoder(NULL),m_bIsClosed(true){
			m_pCSSendData = Mutex::CreateCriticalSection();
		}
		virtual ~ClientSocketBase(){if(m_pCSSendData)delete m_pCSSendData;}
        
	public:
		void setDecoder(DataProcessBase* pDecoder){m_pDecoder = pDecoder;}
		//����ɶ���ʱ��recv����
		virtual void onFDRead();
		//�����д��ʱ��send����
		virtual void onFDWrite();
		//�رմ���
		virtual void close();

		inline bool isClosed(){return m_bIsClosed;}
		DataBlock* getRB(){return &m_recvdata;}
		DataBlock* getWB(){return &m_senddata;}
		//add buffer��ʱ��ע��дfd_set���������Ա�ִ�е�OnFDWrite();
		virtual int addBuf(const char* buf,size_t buflen);
		char* getPeerIp();

		//�������н�����Ip��ַ,ֻ���ص�һ������������,�ַ��������ھ�̬�ռ��У�����ֵ����Ҫ�ͷţ�
		static const char* GetIpv4FromHostname(const char* name);
	protected:
		void open(){m_bIsClosed = false;}

	public:
		// ���ӳɹ�
		virtual bool onSocketConnect() {return true;} 
		// ���ӳ�ʱ
		virtual void onSocketConnectTimeout() {}
		// �����ر�(�����ر�),recv == 0�����
		virtual void onSocketClose() {}
		// errcodeΪ������(socket�ṩ)
		virtual void onSocketConnectError(int errCode) {}
		virtual void onSocketRecvError(int errCode) {}
		virtual void onSocketSendError(int errCode) {}
		// ��������(errCode����㶨��)
		virtual void onNetLevelError(int errCode) {}

	private:
		DataBlock m_recvdata;
		DataBlock m_senddata;
		DataProcessBase *m_pDecoder;
		bool m_bIsClosed;
    public:
        Mutex* m_pCSSendData;
	};

	class ClientSocket : public ClientSocketBase ,public TMEventHandler
	{
	public:
		enum  eWaitType{
			wait_for_none,
			wait_for_connect,
			wait_for_write,
			wait_for_read
		};
	public:
		virtual ~ClientSocket(){}
		ClientSocket(Reactor *pReactor) : ClientSocketBase(pReactor),m_isConnected(false)
			,m_port(0),m_waitType(wait_for_none){ }

		//���ӶԷ�����������������жϡ�
		virtual void onFDWrite();
		//���ӳ�ʱ
		virtual void onTimeOut();
		virtual void close();

		int connect(const char* host,short port,int to = 10);
		bool isConnect(){return m_isConnected;}
		//��ӵ�sendbuf�У���ע�ᵽдfd_set��
		bool sendBuf(BinaryWriteStream &stream);
		bool sendBuf(const char* buf,size_t buflen);

		inline const char* gethost(){return m_host;}
		inline short getport(){return m_port;}
	public:

	private:
		bool m_isConnected;
		char m_host[20];
		short m_port;

		eWaitType  m_waitType;
	};
};

#endif//GLP_CLIENTSOCKET_H_


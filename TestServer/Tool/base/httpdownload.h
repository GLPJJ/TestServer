#ifndef HTTPDOWNLOAD_H_
#define HTTPDOWNLOAD_H_
#include "ClientSocket.h"
#include "Timer.h"
#include "HttpDownloadType.h"
#include "../zlib/zlib.h"

namespace Tool {

typedef struct _DownloadState
{
	// ����״̬
	typedef enum _eDownloadState
	{
		DS_IDLE,//����
		DS_BUSY,//������
		DS_STOP,// ��ͣ����
	}eDownloadState;
	volatile eDownloadState 		state;//����״̬
	size_t									downsize;//�Ѿ����ش�С
	size_t									filesize;//�ļ��ܴ�С

	_DownloadState():state(DS_IDLE),downsize(0L),filesize(0L){}
	float progress()const{ if(filesize==0L)return 0.0f;return downsize*1.0f/filesize;}
}DownloadState;

class Reactor;
class CHttpDownloadMgr;
class CHttpDownload : public ClientSocket
{
public:
	friend class CHttpDownloadMgr;
	enum DOWNLOAD_STATE
	{
		DOWNLOAD_ERROR = -1,
		DOWNLOAD_OK = 0,
		DOWNLOAD_FINISH = 1,
		DOWNLOAD_REDIRECTURL = 2,
		DOWNLOAD_FILEEXIST =  3,
		RANGE_NOT_SATISFIABLE =4,
		NO_RESPONSE_DATA = 5,
		RESPONSE_HEADER_NOT_FINISH = 7,
	};
public:
	CHttpDownload(Reactor *pReactor,CHttpDownloadMgr* pMgr);
	virtual ~CHttpDownload();
    /*
     У���ļ�MD5
     @param fileName[in]:�ļ���
     @param md5[in]:���ڱȽϵ�md5ֵ
     
     @return : ���md5Ϊ�գ������ļ�У��MD5ֵһ�µĻ�����true;��������false
     */
    static bool checkMD5(const std::string fileName,const  std::string md5);

	size_t writeZlibDeBuffer(const unsigned char* p,size_t len);
private:
	bool validHostChar( char c );
	void parseURL( const char* url, char * protocol, int lprotocol, char * host, int lhost, char * request, int lrequest, short * port );
	int formatRequestHeader( char* SendHeader, size_t SendHeaderSize, char* Request, size_t RequestSize, char* Host
			, size_t From, size_t To, char* Data, size_t DataSize );

	long getFileSize(const char* file);
	int getFieldValue(const char *szRequestHeader, const char *szSection, char *nValue, const int nMaxLen);
	int parseResponseHeader();
	size_t getResponseHeader(DataBlock* pDb);
	int recvResponseHeader(DataBlock* pDb);

	bool onFinishDowndFile();
	DOWNLOAD_STATE dealWithTransferEncodingAndCommon(DataBlock* pDb);
	virtual DOWNLOAD_STATE onDownloadSaveData(const char* pData,size_t nLenData);
	virtual void onDownloadByNewUrl();
	virtual void onDownloadError();
	virtual void onDownloadFinish();
protected:
	virtual void onFDRead();

	virtual bool onSocketConnect();
	virtual void onSocketConnectTimeout();
	virtual void onSocketConnectError(int errCode);
	virtual void onSocketClose();
	virtual void onSocketRecvError(int errCode);
	virtual void onSocketSendError(int errCode);
	virtual void onNetLevelError(int errCode);
public:
	void clearReDownloadTimes() { m_nTryDownload = 0; }
	//�����س����ʱ��pInfo��ΪNULL��������¼�������أ�����ָ��pInfo
	void initDownload(const char* url,ONPROGRESS funProgress,DownloadInfo* pInfo);
	void uninitDownload();
	bool startDownload();
	bool isIdle(){return (m_gDownloadState.state != DownloadState::DS_BUSY);}
private:
	CHttpDownloadMgr*   m_pMgr;
	bool 								m_bInit;
	bool							m_isNeedSaveData;

	bool							m_bTransferEncodingChunked;
	enum eContentEncoding{
		CE_Null,
		CE_Deflate,
		CE_Gzip,
	};
	eContentEncoding	m_eContentEncoding;
	int 							m_nTryDownload;

	std::string					m_sOldUrl;

	std::string 				m_sUrl;
	char							m_sProtocol[20];
	char 							m_sHost[256];
	char							m_sIp[256];
	char							m_sRequest[1024];	// Э�顢��������������������
	short							m_nPort;
	//�ϵ�����
	size_t						m_nFrom;

	char*						m_sPostData;
	size_t						m_nPostDataLen;

	char*						m_sRequestHeader;
	size_t 						m_nRequestHeaderSize;
	char*						m_sResponseHeader;
	size_t 						m_nResponseHeaderSize;

	bool							m_bIsRecvResponseHeader;

	//chunk ����,���http��Ӧ����û��content lenght�Ĵ���
	size_t						m_nCurChunkSize;// ��ǰ��Ĵ�С��
	size_t						m_nCurChunkDownloadSize;// ��ǰ���Ѿ������ֽ�����
	size_t						m_nTotalChunkSize;// ���п��ܹ����ۼ��ֽ�����
	//��������״̬
	DownloadState        m_gDownloadState;
	ONPROGRESS          m_funProgress;

	FILE*							m_fileTmp;
	Timer						m_gTimer;

	DataBlock					m_gUnEncodingContent;
public:
	enum {
		ZLIB_UNINIT,
		ZLIB_INIT,
		ZLIB_INIT_GZIP,
		ZLIB_GZIP_INFLATING,
		ZLIB_GZIP_HEADER,
	};
	int								zlib_init;
	z_stream						z;

	//����������Ϣ
	DownloadInfo         m_gDownloadInfo;
};

} /* namespace NetworkUtil */
#endif /* HTTPDOWNLOAD_H_ */

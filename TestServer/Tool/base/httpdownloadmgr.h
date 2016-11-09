#ifndef HTTPDOWNLOADMGR_H_
#define HTTPDOWNLOADMGR_H_

#include "HttpDownload.h"
#include "reactor.h"
#include "thread.h"
#include <list>
#include "mutex.h"


namespace Tool {
//ֻͬʱ����һ������
#define MAX_DOWNLOAD 1

typedef std::list<TaskDownload> LISTTASK;
typedef std::set<std::string> SETURL;


class CHttpDownloadMgr
{
public:
	CHttpDownloadMgr();
	virtual ~CHttpDownloadMgr();

	static bool HttpDownload(ThreadObj pParam);
private:
	//-1 �޿��У�����Ϊ���п��õ�ID��������
	int checkIdle();
public:
	bool initDownload();
	/*
     ����һ��Http����
     @param task[in]���ṹ�п���ָ�����Ȼص�����
        ����ָ����ɻص��������ж��Ƿ����سɹ�
        DownloadInfo ����Ҫָ��fileName������·�������·������mandatory�������MD5Ҳ����ָ�����ļ���С���ڲ�������ȥ��
        ��ʱ�ļ���Ҳ�����һ���Ĺ������ɡ�
     @param priority[in]���Ƿ���������

     @return��0����ӳɹ���-1�����ʧ��;-2���ļ��Ѿ����ڣ�����Ҫ����;-3,URL����Ϊ��
     
     @ע1���ر����ѣ�TaskDownload.DownloadInfo�е�saveBuf���������ж�����������ص��ļ�������Լ������ڴ�ռ�
     ������finish��ʱ����ͷ��ڴ棬�û������Լ������ڴ棬ֻҪʹ�ü��ɡ�
     
     @ע2:���ڷ�ǿ�����ص������������Ҫ�����ص��ļ��봫�ݵ�MD5(�ǡ���)��һ�µĻ��������õ�OnFinish����,��Ϊ�������
	*/
	int addTask(TaskDownload& task,bool priority=false);
	void doNext(bool success,CHttpDownload* worker);
private:
	Thread*					m_pThread;
	LISTTASK					m_listTask;
	SETURL 					m_setUrl;
	TaskDownload			m_gCurTask;

	CHttpDownload*      m_gHttpDownload[MAX_DOWNLOAD];
	Mutex*						m_pCS;
public:
	NetReactor				m_gReactor;
};

} /* namespace NetworkUtil */
#endif /* HTTPDOWNLOADMGR_H_ */

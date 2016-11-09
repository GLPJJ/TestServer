/*
 * HttpDownloadMgr.cpp
 *
 *  Created on: 2014-5-23
 *      Author: glp
 */

#include "HttpDownloadMgr.h"
#include <assert.h>
#include "log.h"

#define SAVE_BUF_SIZE			1048576/*1024 * 1024*/   // ���ջ�������С

namespace Tool {

CHttpDownloadMgr::CHttpDownloadMgr()
:m_pThread(NULL)
,m_pCS(NULL)
{
	// TODO Auto-generated constructor stub
	for(int i=0;i<MAX_DOWNLOAD;i++)
		m_gHttpDownload[i] = new CHttpDownload(&m_gReactor,this);

	m_pThread = Thread::CreateThread(&CHttpDownloadMgr::HttpDownload,(void*)this);

	m_pCS = Mutex::CreateCriticalSection();
}

CHttpDownloadMgr::~CHttpDownloadMgr() {
	// TODO Auto-generated destructor stub
	m_gReactor.stop();
	if(m_pThread)
	{
		if(!m_pThread->Stop())
			m_pThread->Terminate(0);
		delete m_pThread;
	}

	for(int i=0;i<MAX_DOWNLOAD;i++)
	{
		if(m_gHttpDownload[i])
			delete m_gHttpDownload[i];
	}
}

bool CHttpDownloadMgr::HttpDownload(ThreadObj pParam)
{
	CHttpDownloadMgr* pMgr = (CHttpDownloadMgr*)pParam;
	if(pMgr)
		return pMgr->m_gReactor.run();

	return false;
}

int CHttpDownloadMgr::checkIdle()
{
	for(int i=0;i<MAX_DOWNLOAD;i++)
	{
		if(m_gHttpDownload[i] && m_gHttpDownload[i]->isIdle())
			return i;
	}

	return -1;
}

bool CHttpDownloadMgr::initDownload()
{
	if(!m_pThread)
		return false;
	unsigned int threadID;
	return m_pThread->Start(threadID);
}

int CHttpDownloadMgr::addTask(TaskDownload& task,bool priority)
{
	if(strlen(task.url) == 0)
		return -3;

	MutexScoped lock(m_pCS);
	SETURL::const_iterator it = m_setUrl.find(task.url);
	if(it != m_setUrl.end())//�Ѿ������ض�����
		return 0;

	if(!task.info.download)
	{//����������ص��ļ�
		if(!task.info.saveBuf)//����ⲿû�з����ڴ棬��һ��Ĭ�ϴ�С���ڴ�
		{
			task.info.saveBuf = (unsigned char*) malloc(SAVE_BUF_SIZE);
			task.info.saveBufLen = SAVE_BUF_SIZE;
		}
		if(!task.info.saveBuf)
			return -1;
		
		task.info.saveTmpBuf = task.info.saveBuf;
	}
    else
    {
        //���MD5�Ƿ�һ�£�һ��������
        if(task.info.MD5[0] != '\0')//ȷ����Ϊ��
        {
            //�����Ҫ��ǿ�����أ������ļ�У��md5һ�µĻ����򲻼������ض��У���Ϊ���سɹ�
            if(!task.info.mandatory && CHttpDownload::checkMD5(task.info.fileName,task.info.MD5))
            {
                //if(task.onFinish)
                //    task.onFinish(&task,true,task.userData);
                return -2;
            }
        }

		task.info.saveBuf = NULL;
		task.info.saveBufLen = 0;
		task.info.saveTmpBuf = NULL;
    }

	m_setUrl.insert(task.url);
	if(priority)
		m_listTask.push_front(task);
	else
		m_listTask.push_back(task);

	int nIndex = checkIdle();
	if(nIndex != -1 )
	{
		m_gHttpDownload[nIndex]->clearReDownloadTimes();
		m_gHttpDownload[nIndex]->initDownload(task.url,task.onProgress,&task.info);

		memcpy(&m_gCurTask,&task,sizeof(task));
		if(!m_gHttpDownload[nIndex]->startDownload())
		{
			LOGE("%s : %s , start download failed\n",__FUNCTION__,task.url);
			doNext(false,NULL);
			return -1;
		}
		else
			return 0;
	}
	return 0;
}

void CHttpDownloadMgr::doNext(bool success,CHttpDownload* worker)
{
	TaskDownload info;
	{
		MutexScoped lock(m_pCS);
		if(!m_gCurTask.info.download && worker && success)
			m_gCurTask.info.saveBufLen = worker->m_gDownloadInfo.saveBufLen;
		memcpy(&info,&m_gCurTask,sizeof(info));
		if (worker)
			worker->uninitDownload();
	}
	//�����Ļص����Լ��ڴ�Ļ���
	if(info.onFinish)
		info.onFinish(&info,success,info.userData);
	if (info.info.saveBuf)//�ͷ��ڴ�
		free(info.info.saveBuf);

	MutexScoped lock(m_pCS);
	m_setUrl.erase(info.url);
	if(strcmp(m_listTask.front().url , info.url) == 0)
		m_listTask.pop_front();
	else
	{
		LISTTASK::iterator it = m_listTask.begin();
		for (;it!=m_listTask.end();it++)
		{
			if(strcmp(it->url , info.url) == 0)
			{
				m_listTask.erase(it);
				//��set���ƶ����е������ǲ�ͬ��URL
				break;
			}
		}
	}

	if(m_listTask.empty())
		return ;//�Ѿ����������

	//һ����˵������һ��Ӧ���ǿ��еģ����������OnFinish�����һ������Ļ�������Ͳ��ǿ���״̬�ˣ����Ի�����Ҫ�ж�һ��
	int nIndex = checkIdle();

	if(nIndex != -1)
	{
		//ȡ�����еĵ�һ��
		TaskDownload next = m_listTask.front();
		m_gHttpDownload[nIndex]->clearReDownloadTimes();
		m_gHttpDownload[nIndex]->initDownload(next.url,next.onProgress,&next.info);
		if(!m_gHttpDownload[nIndex]->startDownload())
		{
			LOGE("%s : %s , start download failed\n",__FUNCTION__,next.url);
			if(next.onFinish)
				next.onFinish(&next,false,next.userData);
			//����������һ��
			doNext(false,NULL);
		}
		else
		{
			MutexScoped lock(m_pCS);
			memcpy(&m_gCurTask,&next,sizeof(TaskDownload));
		}
	}
}

} /* namespace NetworkUtil */

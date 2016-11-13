#ifndef GLP_DATABLOCK_H_
#define GLP_DATABLOCK_H_

#define	DEFAULT_BLOCK_SIZE  1024
#define	MAX_BLOCK_SIZE  1048576/*1024*1024*/

namespace Tool
{
	class DataBlock
	{
	public:
		DataBlock(size_t size = DEFAULT_BLOCK_SIZE);
		virtual ~DataBlock();
		//加到buffer的末尾
		/*返回当前添加进去的字节数*/
		size_t append(const char *buf,size_t buflen) {return copy(m_pos,buf,buflen);}
		//拷贝数据到指定位置
		/*返回当前拷贝进去的字节数*/
		size_t copy(size_t pos,const char *buf,size_t buflen);
		//获取整个buffer
		char* getBuf() const {return m_buf;}
		//获取当前位置
		size_t getPos() const {return m_pos;}
		//清空
		void initPos() {m_pos = 0;}
		//获取大小
		size_t getSize()const{return m_size;}
	private:
		char *m_buf;
		size_t m_pos;
		size_t m_size;
	};
}
#endif//GLP_DATABLOCK_H_

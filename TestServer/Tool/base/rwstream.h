#ifndef PROTOCOLSTREAM__H__
#define PROTOCOLSTREAM__H__

#include <string>
#include <string.h>

namespace Tool
{

	typedef enum _PACKAGELEN_TYPE
	{
		BINARY_PACKLEN_LEN = 2,
		TEXT_PACKLEN_LEN = 4
	}PACKAGELEN_TYPE;

#define PACKAGE_MAXLEN	 0xffff//65535

	class ReadStream
	{
	public:
		ReadStream(PACKAGELEN_TYPE htype_=BINARY_PACKLEN_LEN,bool net_=true):htype(htype_),net(net_){}
		virtual ~ReadStream(){}

		virtual bool readCom(/*out*/void* buffer,/*in*/unsigned int len_to_read,/*out*/unsigned int* len_readed) = 0;
		virtual unsigned int getSize() = 0;
		virtual const char* getData() = 0;
		virtual unsigned int tell() = 0;
		virtual bool isEmpty() const= 0;
		virtual bool isEnd() const= 0;
		virtual bool skip(int offset,bool peek=true)=0;

		bool Read(/* out */ int & i);
		bool Read(/* out */ short & i);
		bool Read(/* out */ char & c);
		bool Read(/* out */ char* str,/* in */  unsigned int strlen, /* out */ unsigned int & len);
		unsigned int readAll(/* out */ char * szBuffer, /*in*/unsigned int iLen);
	private:
		bool readLength(unsigned int & len);
		bool readLengthWithoutOffset(unsigned int & outlen);
	private:
		bool							  net;
	public:
		PACKAGELEN_TYPE htype;
	};

	class WriteStream
	{
	public:
		WriteStream(PACKAGELEN_TYPE htype_=BINARY_PACKLEN_LEN,bool net_=true):htype(htype_),net(net_){}
		virtual ~WriteStream(){}
		bool Write(const char* str, unsigned int length);
		bool writeWithoutLength(const char* str, unsigned int length);
		bool Write(int i);
		bool Write(short i);
		bool Write(char c);
		bool writeLength(unsigned int len);

		virtual bool writeCom(const void* buffer,unsigned int len_to_write,unsigned int* len_writed) = 0;
		virtual void flush() = 0;
		virtual const char* getData() const = 0;
		virtual unsigned int getSize() const = 0;
		virtual bool isValid() const = 0;

		//当peek为true时，只是查看下还有多少的内容，够不够offset的大小
		//当peek为false时，跳过指定的offset数据内容。
		virtual bool skip(int offset,bool peek=true)=0;
	protected:
		bool							  net;
		PACKAGELEN_TYPE htype;
	};

	class BinaryReadStream : public ReadStream
	{
	private:

		BinaryReadStream(const BinaryReadStream&);
		BinaryReadStream& operator=(const BinaryReadStream&);

	public:
		BinaryReadStream(const char* ptr_, unsigned int len_,PACKAGELEN_TYPE htype_=BINARY_PACKLEN_LEN);

		virtual bool readCom(/*out*/void* buffer,/*in*/unsigned int len_to_read,/*out*/unsigned int* len_readed);

		virtual unsigned int getSize(){ return len;}
		virtual const char* getData(){return start;}
		virtual unsigned int tell() {return (unsigned int)(cur-start);};
		virtual bool isEmpty() const {return len<=(unsigned int)htype;}
		virtual bool isEnd() const {return cur == end;}
		virtual bool skip(int offset,bool peek=true);

	private:
		const char* const start;
		const unsigned int len;
		const char* cur;
		const char* end;
	};

	class BinaryWriteStream : public WriteStream
	{
	private:
		BinaryWriteStream(const BinaryWriteStream&);
		BinaryWriteStream& operator=(const BinaryWriteStream&);

	public:
		BinaryWriteStream(char* ptr, unsigned int len,PACKAGELEN_TYPE htype_=BINARY_PACKLEN_LEN);
		virtual bool writeCom(/*in*/const void* buffer,/*in*/unsigned int len_to_write,/*out*/unsigned int* len_writed);

		virtual bool skip(int offset,bool peek=false);
		virtual unsigned int getSize() const {return (unsigned int)(cur - start);}
		virtual bool isValid() const;
		virtual void flush();
		virtual const char* getData() const {return start;}
	private:
		char* const start;
		const unsigned int len;
		char*  cur;
		char*  end;
	};
}
#endif//PROTOCOLSTREAM__H__

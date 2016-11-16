#ifndef GLP_PACKAGE_H_
#define GLP_PACKAGE_H_

#include "base.h"
#include "datablock.h"

namespace Tool{

	class ClientId{
	public:
		ClientId():m_clientid(0){}

		void setClientID(int id){m_clientid = id;}
		int getClientID() const {return m_clientid;}
	private:
		int m_clientid;
	};

	class Package : public ClientId
	{
		DISALLOW_COPY_AND_ASSIGN(Package);
	public: 
		Package(HeadType type,const char* buf,size_t buflen);
		Package(HeadType type,char* buf,size_t buflen);

		void initBuf(const char* buf,size_t buflen);
		size_t getBuflen() const;
		char *getBuf() const;
		HeadType getHeadType() const;
	private:
		HeadType type;
		DataBlock data;
	};

}


#endif//GLP_PACKAGE_H_

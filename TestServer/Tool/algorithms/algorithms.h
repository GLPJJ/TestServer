#ifndef ALGORITHMS__H__
#define ALGORITHMS__H__

#include <string>

namespace Tool{

class Algorithms {
	
public:

	static std::string EncodeWithMd5(const char* file);
	static std::string EncodeWithMd5(const unsigned char* data,unsigned int len);

};

}

#endif//ALGORITHMS__H__
#include "stdafx.h"
using namespace std;
class RssBdException:public exception
{
public:
	RssBdException(const char* errstr) :exception(errstr){}
	~RssBdException(){}
};
class RssMNGException :public exception
{
public:
	RssMNGException(const char* errstr) :exception(errstr){}
	~RssMNGException(){}
};
// TestMD5.cpp : Defines the entry point for the console application.
//

#include "pch.h"

#include "md5.h"

#include <iostream>
#include <string>
using namespace std;

namespace kagula
{
	string md5(string strPlain)
	{
		MD5_CTX mdContext;
		//int bytes;
		//unsigned char data[1024];

		MD5Init(&mdContext);
		MD5Update(&mdContext, (unsigned char*)const_cast<char*>(strPlain.c_str()), strPlain.size());
		MD5Final(&mdContext);

		string md5;
		char buf[3];
		for (int i = 0; i < 16; i++)
		{
			sprintf_s(buf, "%02x", mdContext.digest[i]);
			md5.append(buf);
		}
		return md5;
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	string strMD5 = kagula::md5("kagula");
	cout << strMD5 << endl;

	return 0;
}
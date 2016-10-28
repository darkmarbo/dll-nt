// TN_Toolkit_CPP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TN_Engine.h"
#include "BasicNSWs.h"

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *fp = fopen("ttt.txt","w");
	TN_Engine tn_engine;

	string out_str = tn_engine.ProcessText("2015-11-6");
	cout << out_str << endl;
	fprintf(fp, "%s\n", out_str.c_str()); 

	fclose(fp);
	getchar();

	return 0;
}

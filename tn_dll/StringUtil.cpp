#include "StdAfx.h"
#include "StringUtil.h"
#include <sstream>
#include "UTF8Util.h"


StringUtil::StringUtil(void)
{
}


StringUtil::~StringUtil(void)
{
}

string& StringUtil::trim(string& str)
{
	if(str.empty())
	{
		return str;
	}
	str.erase(0, str.find_first_not_of(" "));
	str.erase(str.find_last_not_of(" ")+1);

	return str;
}

// 有错误！！！！
//string& StringUtil::delSeqSpace(string& str)
//{
//	string out_str = "";
//	bool isSpace = false;
//
//	size_t length_count = 0;
//	const char* inbuff = str.c_str();
//	const char* pbuff = UTF8Util::FindNextInline(inbuff, '\t');
//	while(inbuff != pbuff)
//	{
//		string ch = UTF8Util::NextNChars(inbuff, 1);
//
//		if(ch == " ")
//		{
//			if(!isSpace)
//			{
//				out_str += ch;
//			}
//			isSpace = true;
//		}
//		else
//		{
//			isSpace = false;
//		}
//
//		inbuff = UTF8Util::NextChar(inbuff);
//	}
//
//	return StringUtil::trim(out_str);
//}

void StringUtil::split(vector<string>& res, const string& input, const char dem)
{
	size_t pos = input.find(dem);
	size_t beg = 0;
	while(pos != string::npos)
	{
		string w = input.substr(beg, pos-beg);
		if(w.size() > 0)
		{
			res.push_back(w);
		}
		beg = pos+1;
		pos = input.find(dem, beg);
	}
	string w = input.substr(beg, pos-beg);
	if(w.size() > 0)
	{
		res.push_back(w);
	}
}

void StringUtil::split(vector<string>& res, const string& input, const string& dem)
{
	size_t pos = input.find(dem);
	size_t beg = 0;
	while(pos != string::npos)
	{
		string w = input.substr(beg, pos-beg);
		if(w.size() > 0)
		{
			res.push_back(w);
		}
		beg = pos+dem.length();
		pos = input.find(dem, beg);
	}
	string w = input.substr(beg, pos-beg);
	if(w.size() > 0)
	{
		res.push_back(w);
	}
}

void StringUtil::split_or(vector<string>& res, const string& input, const string& dem_or)
{
	vector<string> dem_vec;
	split(dem_vec, dem_or, '|');

	size_t beg = 0;
	string dem = "";
	size_t pos = find_ldem(dem_vec, input, dem, 0);
	while(pos != string::npos)
	{
		string w = input.substr(beg, pos-beg);
		if(w.size() > 0)
		{
			res.push_back(w);
		}
		beg = pos+dem.length();
		if(beg >= input.length())
		{
			break;
		}
		pos = find_ldem(dem_vec, input, dem, beg);
	}
	string w = input.substr(beg, pos-beg);
	if(w.size() > 0)
	{
		res.push_back(w);
	}

}

size_t StringUtil::find_ldem(vector<string>& res, const string& input, string& dem, size_t beg)
{
	//cout << "beg=" << beg << endl;
	//cout << "input.length()=" << input.length() << endl;
	string suf_str = input.substr(beg);
	size_t ldem_pos = input.length();
	bool isMatch = false;
	for(int idx=0; idx<res.size(); ++idx)
	{
		string temp_dem = res.at(idx);
		size_t pos = suf_str.find(temp_dem);

		if(pos!=string::npos && pos<ldem_pos)
		{
			ldem_pos = pos;
			dem = temp_dem;
			isMatch = true;
		}
	}

	if(isMatch)
	{
		return beg+ldem_pos;
	}
	else
	{
		return string::npos;
	}
	
}


string StringUtil::num2str(int num)
{
	stringstream ss;
	ss << num;
	return ss.str();
}


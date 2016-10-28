#pragma once

class StringUtil
{
public:
	StringUtil(void);
	~StringUtil(void);

	static string& trim(string &);
	static string& delSeqSpace(string &);

	static void split(vector<string>& res, const string& input, const char dem);
	static void split(vector<string>& res, const string& input, const string& dem);
	static void split_or(vector<string>& res, const string& input, const string& dem_or);
	static size_t find_ldem(vector<string>& res, const string& input, string& dem, size_t beg);

	static string num2str(int num);

};


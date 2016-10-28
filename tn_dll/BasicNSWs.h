#pragma once

#include "Rule_Obj.h"
#include "Rule_Match_Obj.h"

class BasicNSWs
{

	static vector<Rule_Obj> rule_vec;
	vector<Rule_Match_Obj> rule_match_vec;

	string text;
	string rule_standard_str;
	//string rule_norm_str;

public:
	BasicNSWs(void);
	~BasicNSWs(void);

	static void Init();

	static int Load_rules();
	static int Preprocess_rules();
	string ParseText(string text);
	string LabelNSWs(string text);
	int Disambiguation();

	// 以下不属于正式的代码，最后删除
	void TestDeelx();
	int test_all_number(const char * string);
	int test_email(const char * string);
	int find_remark(const char * string, int & start, int & end);
	int match_decimal(const char* string);
	int match_exp(const char* string);
	//

	void ParseFile(string file_path, string output_path);

};


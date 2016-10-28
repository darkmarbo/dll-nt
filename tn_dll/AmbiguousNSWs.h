#pragma once
class AmbiguousNSWs
{
	string text;
	string rule_standard_str;

public:
	AmbiguousNSWs(void);
	~AmbiguousNSWs(void);

	void Init();

	int Load_Model();	//
	string ParseText(string text);
	string LabelNSWs(string text);

};


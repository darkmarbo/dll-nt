#pragma once
class Rule_Match_Obj
{
	int start;
	int end;
	int rule_id;
	string rule_transform_str;
	bool isReady;

public:
	Rule_Match_Obj(void);
	~Rule_Match_Obj(void);

	Rule_Match_Obj(int start, int end, int rule_id);

	void SetStart(int start);
	int GetStart();
	void SetEnd(int end);
	int GetEnd();
	void SetRule_id(int rule_id);
	int GetRule_id();
	void SetRule_transform_str(string rule_transform_str);
	string GetRule_transform_str();
	void SetIsReady(bool isReady);
	bool GetIsReady();

	static bool Compare(Rule_Match_Obj rule_match_obj_1, Rule_Match_Obj rule_match_obj_2);
};


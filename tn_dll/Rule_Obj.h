#pragma once
class Rule_Obj
{
	int rule_id;
	string rule_name;
	string rule_text;
	string rule_exp;
	string rule_restrictions;
	int rule_priority;
	string rule_transform_exp;
	bool isUsable;

public:
	Rule_Obj(void);
	~Rule_Obj(void);
	Rule_Obj(string name, string text, string restrictions, int priority, string transform_exp, bool isUsable);
	Rule_Obj(string name, string text, string restrictions, int priority, string transform_exp, int isUsableMark);

	void SetRule_id(int rule_id);
	int GetRule_id();
	void SetRule_name(string rule_name);
	string GetRule_name();
	void SetRule_text(string rule_text);
	string GetRule_text();
	void SetRule_exp(string rule_exp);
	string GetRule_exp();
	void SetRule_restrictions(string restrictions);
	string GetRule_restrictions();
	void SetRule_priority(int priority);
	int GetRule_priority();
	void SetRule_Transform_exp(string rule_transform_exp);
	string GetRule_transform_exp();
	void SetIsUsable(bool isUsable);
	bool GetIsUsable();
	void SetIsUsable(int isUsableMark);

	string toString();

	static bool Compare(Rule_Obj rule_obj_1, Rule_Obj rule_obj_2);
};


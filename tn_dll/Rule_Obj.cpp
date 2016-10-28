#include "StdAfx.h"
#include "Rule_Obj.h"
#include "StringUtil.h"


Rule_Obj::Rule_Obj(void)
{
}

Rule_Obj::~Rule_Obj(void)
{
}

Rule_Obj::Rule_Obj(string name, string text, string restrictions, int priority, string transform_exp, bool isUable)
{
	this->rule_name = name;
	this->rule_text = text;
	this->rule_restrictions = restrictions;
	this->rule_priority = priority;
	this->rule_transform_exp = transform_exp;
	this->isUsable = isUsable;
}

Rule_Obj::Rule_Obj(string name, string text, string restrictions, int priority, string transform_exp, int isUsableMark)
{
	bool isUsable = true;

	if(isUsableMark < 1)
	{
		isUsable = false;
	}

	Rule_Obj(name, text, restrictions, priority, transform_exp, isUsable);

}

void Rule_Obj::SetRule_id(int rule_id)
{
	this->rule_id = rule_id;
}

int Rule_Obj::GetRule_id()
{
	return this->rule_id;
}

void Rule_Obj::SetRule_name(string rule_name)
{
	this->rule_name = rule_name;
}

string Rule_Obj::GetRule_name()
{
	return this->rule_name;
}
	
void Rule_Obj::SetRule_text(string rule_text)
{
	this->rule_text = rule_text;
}

string Rule_Obj::GetRule_text()
{
	return this->rule_text;
}
	
void Rule_Obj::SetRule_exp(string rule_exp)
{
	this->rule_exp = rule_exp;
}

string Rule_Obj::GetRule_exp()
{
	return this->rule_exp;
}

void Rule_Obj::SetRule_restrictions(string restrictions)
{
	this->rule_restrictions = restrictions;
}
	
string Rule_Obj::GetRule_restrictions()
{
	return this->rule_restrictions;
}

void Rule_Obj::SetRule_priority(int priority)
{
	this->rule_priority = priority;
}

int Rule_Obj::GetRule_priority()
{
	return this->rule_priority;
}

void Rule_Obj::SetRule_Transform_exp(string rule_transform_exp)
{
	this->rule_transform_exp = rule_transform_exp;
}

string Rule_Obj::GetRule_transform_exp()
{
	return this->rule_transform_exp;
}
	
void Rule_Obj::SetIsUsable(bool isUsable)
{
	this->isUsable = isUsable;
}

bool Rule_Obj::GetIsUsable()
{
	return this->isUsable;
}

void Rule_Obj::SetIsUsable(int isUsableMark)
{
	if(isUsableMark < 1)
	{
		this->isUsable = false;
	}
	else
	{
		this->isUsable = true;
	}

}

string Rule_Obj::toString()
{
	//return this->rule_name+"|||"+this->rule_text+"|||"+this->rule_restrictions+"|||"+StringUtil::num2str(this->rule_priority)+"|||"+this->rule_transform_exp+"|||";
	return this->rule_name+"|||"+this->rule_text+"|||"+this->rule_exp+"|||"+this->rule_restrictions+"|||"+StringUtil::num2str(this->rule_priority)+"|||"+this->rule_transform_exp+"|||";
}

bool Rule_Obj::Compare(Rule_Obj rule_obj_1, Rule_Obj rule_obj_2)
{
	if(rule_obj_1.GetRule_priority() < rule_obj_2.GetRule_priority())
	{
		return true;
	}
	else if(rule_obj_1.GetRule_priority() > rule_obj_2.GetRule_priority())
	{
		return false;
	}

	return false;
}
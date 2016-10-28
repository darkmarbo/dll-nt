#include "StdAfx.h"
#include "Rule_Match_Obj.h"


Rule_Match_Obj::Rule_Match_Obj(void)
{
}


Rule_Match_Obj::~Rule_Match_Obj(void)
{
}

Rule_Match_Obj::Rule_Match_Obj(int start, int end, int rule_id)
{
	this->start = start;
	this->end = end;
	this->rule_id = rule_id;

}

void Rule_Match_Obj::SetStart(int start)
{
	this->start = start;
}

int Rule_Match_Obj::GetStart()
{
	return this->start;
}

void Rule_Match_Obj::SetEnd(int end)
{
	this->end = end;
}
	
int Rule_Match_Obj::GetEnd()
{
	return this->end;
}

void Rule_Match_Obj::SetRule_id(int rule_id)
{
	this->rule_id = rule_id;
}

int Rule_Match_Obj::GetRule_id()
{
	return this->rule_id;
}

void Rule_Match_Obj::SetRule_transform_str(string rule_transform_str)
{
	this->rule_transform_str = rule_transform_str;
}

string Rule_Match_Obj::GetRule_transform_str()
{
	return this->rule_transform_str;
}

void Rule_Match_Obj::SetIsReady(bool isReady)
{
	this->isReady = isReady;
}

bool Rule_Match_Obj::GetIsReady()
{
	return this->isReady;
}

bool Rule_Match_Obj::Compare(Rule_Match_Obj rule_match_obj_1, Rule_Match_Obj rule_match_obj_2)
{
	if(rule_match_obj_1.GetStart() <= rule_match_obj_2.GetStart())
	{
		return true;
	}
	else
	{
		return false;
	}
}

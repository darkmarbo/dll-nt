#include "stdafx.h"
#include "Tran_Obj.h"


Tran_Obj::Tran_Obj(void)
{
}


Tran_Obj::~Tran_Obj(void)
{
}

Tran_Obj::Tran_Obj(string key, string value)
{
	this->key = key;
	this->value = value;
}

void Tran_Obj::SetKey(string key)
{
	this->key = key;
}

string Tran_Obj::GetKey()
{
	return this->key;
}

void Tran_Obj::SetValue(string value)
{
	this->value = value;
}
	
string Tran_Obj::GetValue()
{
	return this->value;
}

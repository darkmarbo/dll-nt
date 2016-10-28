#pragma once
class Tran_Obj
{

	string key;
	string value;

public:
	Tran_Obj(void);
	~Tran_Obj(void);

	Tran_Obj(string key, string value);

	void SetKey(string key);
	string GetKey();
	void SetValue(string value);
	string GetValue();

};


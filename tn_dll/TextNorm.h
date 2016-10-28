#pragma once
#include "Tran_Obj.h"
#include "Norm_ZH.h"

class TextNorm
{

public:
	TextNorm(void);
	~TextNorm(void);

	static void Init();

	static int load_tran_map();

	static map<string, vector<Tran_Obj>> tran_map;

	static string NormText(string text);

};

#pragma once
#include "TextNorm.h"

class Norm_Base
{

public:

	Norm_Base(void);
	~Norm_Base(void);
	Norm_Base(TextNorm* textNorm);

	string NormText(string text);

	TextNorm* textNorm;

};


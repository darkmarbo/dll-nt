#include "StdAfx.h"
#include "Norm_Base.h"


Norm_Base::Norm_Base(void)
{
}


Norm_Base::~Norm_Base(void)
{
}

Norm_Base::Norm_Base(TextNorm* textNorm)
{
	this->textNorm = textNorm;
}

string Norm_Base::NormText(string text)
{
	return "";

}

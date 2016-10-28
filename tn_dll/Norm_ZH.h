#pragma once
#include "TextNorm.h"

class Norm_ZH
{

public:
	Norm_ZH(void);
	~Norm_ZH(void);

	static string NormText(string text);

	static string Func_Read_Digit(string text);

	static string Func_Read_Tel(string text);

	static string Func_Read_Num(string text);

	static string Func_Read_Abbr(string text);

	static string Func_Read_En_Seq(string text);

	static string Func_Read_Word(string text);

	static string Func_Read_Symbol(string text);

	static string Func_Read_Tran(string text, string tran_name);

	static string read_num_seq(string text);

	static string read_tel_seq(string text);

	//Digit  Tel  Num  Abbr  En_Seq   Word, 只设置这几个标签，可以组合如：Digit-En_Seq
	static string Func_Norm_Zh(string text);
	////Email   Url

	static string integer2Han_hundred(string text);

	static string integer2Han_thousand(string text);
	
	static string integer2Han(string text);

	static string full2half(string text);


	//
	static string read_int(string number_str);

	static string read_decimal(string number_str);

	static string read_real(string number_str);

	static string read_fraction(string number_str);

	static string read_percentage(string number_str);

	static string read_ratio(string number_str);

	static string read_interval(string number_str);

	static string read_number_exp(string number_exp);

	static string read_en_number_mixture(string sent);

	static string read_date(string date_str);

	static string read_date_format(string date_str);
	
	static string read_date_text(string date_str);

	static string read_date_seq(string date_str);

	static string read_time(string time_str);

	static string read_time_format(string time_str);

	static string read_time_text(string time_str);

	static string read_time_section(string time_str);

	static string read_email(string email_str);

	static string read_url(string url_str);

	static string read_phone(string phone_str);

	static string read_en_number_symbol_mixture(string sent, map<string, string> local_tran_map);

	static string test();

	static string TestSingle();

	static string testBasicNSWs();

};


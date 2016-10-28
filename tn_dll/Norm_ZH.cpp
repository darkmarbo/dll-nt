#include "StdAfx.h"
#include "Norm_ZH.h"
#include "UTF8Util.h"
#include "StringUtil.h"
#include "BasicNSWs.h"
#include "regexp\deelx.h"
#include "TN_Engine.h"


#if defined(_MSC_VER) && _MSC_VER > 1310
// Visual C++ 2005 and later require the source files in UTF-8, and all strings
// to be encoded as wchar_t otherwise the strings will be converted into the
// local multibyte encoding and cause errors. To use a wchar_t as UTF-8, these
// strings then need to be convert back to UTF-8. This function is just a rough
// example of how to do this.
# include <Windows.h>
# define utf8(str) ConvertToUTF8(L ## str)
string ConvertToUTF8(const wchar_t* pStr) {
  static char szBuf[1024];
  WideCharToMultiByte(CP_UTF8, 0, pStr, -1, szBuf, sizeof(szBuf), NULL, NULL);
  return szBuf;
}

#else // if defined(_MSC_VER) && _MSC_VER > 1310
// Visual C++ 2003 and gcc will use the string literals as is, so the files
// should be saved as UTF-8. gcc requires the files to not have a UTF-8 BOM.
# define utf8(str) string(str)
#endif // if defined(_MSC_VER) && _MSC_VER > 1310


Norm_ZH::Norm_ZH(void)
{}

Norm_ZH::~Norm_ZH(void)
{
}

//Norm_ZH::Norm_ZH(TextNorm textNorm)
//{
//	this->textNorm = textNorm;
//}

string Norm_ZH::NormText(string text)
{
	string temp_text = text;
	//CRegexpT <char> regexp(utf8("(?<=(<[a-zA-Z0-9_]+>))([^</>]+)(?=(</[a-zA-Z0-9_]+>))").c_str());
	CRegexpT <char> regexp(utf8("(<[a-zA-Z0-9_]+>)([^<>]+)(</[a-zA-Z0-9_]+>)").c_str());
	MatchResult result = regexp.Match(temp_text.c_str());

	string norm_str = "";
	size_t beg = 0;

	while(result.IsMatched())
	{
		norm_str += temp_text.substr(beg, result.GetStart()-beg);

		//cout << temp_text.substr(result.GetStart(), result.GetEnd()-result.GetStart()) << endl;  // yqw 20150918 debug

		string label = temp_text.substr(result.GetGroupStart(1)+1, result.GetGroupEnd(1)-result.GetGroupStart(1)-2);

		string match_text = temp_text.substr(result.GetGroupStart(2), result.GetGroupEnd(2)-result.GetGroupStart(2));

		if(label == utf8("Digit"))
		{
			norm_str += Func_Read_Digit(match_text);
		}
		else if(label == utf8("Tel"))
		{
			norm_str += Func_Read_Tel(match_text);
		}
		else if(label == utf8("Num"))
		{
			norm_str += Func_Read_Num(match_text);
		}
		else if(label == utf8("Num_Percentage"))
		{
			norm_str += read_percentage(match_text);
		}
		else if(label == utf8("Num_Fraction"))
		{
			norm_str += read_fraction(match_text);
		}
		else if(label == utf8("Num_Ratio"))
		{
			norm_str += read_ratio(match_text);
		}
		else if(label == utf8("Num_Interval"))
		{
			norm_str += read_interval(match_text);
		}
		else if(label == utf8("Num_Seq"))
		{
			norm_str += read_num_seq(match_text);
		}
		else if(label == utf8("Tel_Seq"))
		{
			norm_str += read_tel_seq(match_text);
		}
		else if(label == utf8("Num_Real"))
		{
			norm_str += read_real(match_text);
		}
		else if(label == utf8("Email"))
		{
			norm_str += read_email(match_text);
		}
		else if(label == utf8("URL"))
		{
			norm_str += read_url(match_text);
		}
		else if(label == utf8("Phone"))
		{
			norm_str += read_phone(match_text);
		}
		else if(label == utf8("En_Num_Mix"))
		{
			norm_str += read_en_number_mixture(match_text);
		}
		else if(label == utf8("Date"))
		{
			norm_str += read_date(match_text);
		}
		else if(label == utf8("Time"))
		{
			norm_str += read_time(match_text);
		}
		else if(label == utf8("En_Seq"))
		{
			norm_str += Func_Read_En_Seq(match_text);
		}
		else if(UTF8Util::FromSubstr(label.c_str(), 4) == "Tran")
		{
			norm_str += Func_Read_Tran(match_text, UTF8Util::SkipNChars(label.c_str(), 5));
		}
		else
		{
			norm_str += match_text;
		}

		beg = result.GetEnd();

		result = regexp.Match(temp_text.c_str(), result.GetEnd());
	}

	norm_str += temp_text.substr(beg, temp_text.length()-beg);

	return StringUtil::trim(norm_str);
}

string Norm_ZH::Func_Read_Digit(string text)
{
	map<string, vector<Tran_Obj>>::iterator iter = TextNorm::tran_map.find("read_digit");

	vector<Tran_Obj> tran_vec;
	map<string, string> local_tran_map;
	if(iter != TextNorm::tran_map.end())
	{
		tran_vec = iter->second;
	}

	for(int i=0; i<tran_vec.size(); ++i)
	{
		Tran_Obj tran_obj = tran_vec.at(i);
		local_tran_map.insert(make_pair(tran_obj.GetKey(), tran_obj.GetValue()));
		//cout << tran_obj.GetKey() << ", " << tran_obj.GetValue() << endl;
	}

	string out_str = "";
	string temp_text = text;
	for(int idx=0; idx<UTF8Util::StrCharLength(text.c_str()); ++idx)
	{
		string ch = UTF8Util::NextNChars(temp_text.c_str(), 1);
		//cout << "now, " << ch << endl;

		map<string, string>::iterator local_iter = local_tran_map.find(ch);
		if(local_iter != local_tran_map.end())
		{
			out_str += local_iter->second;
		}
		else
		{
			out_str += ch;
		}

		//cout << out_str << endl;
		temp_text = UTF8Util::SkipNChars(temp_text.c_str(), 1);
	}

	return out_str;

}

string Norm_ZH::Func_Read_Tel(string text)
{
	map<string, vector<Tran_Obj>>::iterator iter = TextNorm::tran_map.find("read_tel");

	vector<Tran_Obj> tran_vec;
	map<string, string> local_tran_map;
	if(iter != TextNorm::tran_map.end())
	{
		tran_vec = iter->second;
	}

	for(int i=0; i<tran_vec.size(); ++i)
	{
		Tran_Obj tran_obj = tran_vec.at(i);
		local_tran_map.insert(make_pair(tran_obj.GetKey(), tran_obj.GetValue()));
	}

	string out_str = "";
	string temp_text = text;
	for(int idx=0; idx<UTF8Util::StrCharLength(text.c_str()); ++idx)
	{
		string ch = UTF8Util::NextNChars(temp_text.c_str(), 1);

		map<string, string>::iterator local_iter = local_tran_map.find(ch);
		if(local_iter != local_tran_map.end())
		{
			out_str += local_iter->second;
		}
		else
		{
			out_str += ch;
		}

		temp_text = UTF8Util::SkipNChars(temp_text.c_str(), 1);
	}

	return out_str;
}

string Norm_ZH::Func_Read_Num(string text)
{
	return integer2Han(text);
}

string Norm_ZH::integer2Han_hundred(string hundred_text)
{
	int length = UTF8Util::StrCharLength(hundred_text.c_str());

	if(length > 3)
	{
		cout << "integer2Han_hundred, parameter length bigger than 3" << endl;
		return hundred_text;
	}

	if(1 == length)
	{
		return Func_Read_Digit(hundred_text);
	}

	string out_str = "";

	if(2 == length )
	{
		int num = atoi(hundred_text.c_str());

		if(num/10 != 1)
		{
			out_str += Func_Read_Digit(Func_Read_Digit(StringUtil::num2str(num/10)));
		}
		out_str += utf8("十");
		if(num%10 != 0)
		{
			out_str += Func_Read_Digit(StringUtil::num2str(num%10));
		}

		return StringUtil::trim(out_str);
	}

	if(3 == length)
	{
		int num = atoi(hundred_text.c_str());

		//cout << "num=\t" << num << endl;

		if(num/100 != 0)
		{
			out_str += Func_Read_Digit(StringUtil::num2str(num/100));
			//cout << "hundred, " << StringUtil::num2str(num/100) << endl;
			out_str += utf8("百");
		}

		if(num%100 == 0)
		{
			return StringUtil::trim(out_str);
		}

		if(num%100 < 10)
		{
			out_str += utf8("零");
			out_str += Func_Read_Digit(StringUtil::num2str(num%100));
			return StringUtil::trim(out_str);
		}

		out_str += Func_Read_Digit(StringUtil::num2str(num%100/10));
		out_str += utf8("十");
		if(num%10 != 0)
		{
			out_str += Func_Read_Digit(StringUtil::num2str(num%10));
		}

		//cout << "Return 3" << endl;
		return StringUtil::trim(out_str);

	}
	
	//cout << "Return:\t" << out_str << endl;

	return out_str;

}

string Norm_ZH::integer2Han_thousand(string thousand_text)
{
	int length = UTF8Util::StrCharLength(thousand_text.c_str());

	if(length > 4)
	{
		cout << "integer2Han_thousand, parameter length bigger than 4" << endl;
		return thousand_text;
	}

	if (length < 4)
	{
		return integer2Han_hundred(thousand_text);
	}

	string out_str = "";
	if(length == 4)
	{
		int num = atoi(thousand_text.c_str());
		if(num/1000 != 0)
		{
			out_str += Func_Read_Digit(StringUtil::num2str(num/1000));
			out_str += utf8("千");
		}
		if(num%1000 == 0)
		{
			return StringUtil::trim(out_str);
		}
		if(num%1000 < 10)
		{
			out_str += utf8("零");
			out_str += Func_Read_Digit(StringUtil::num2str(num%1000));
			return StringUtil::trim(out_str);
		}
		if(num%1000 < 100)
		{
			out_str += utf8("零");
			out_str += Func_Read_Digit(StringUtil::num2str(num%100/10));
			out_str += utf8("十");
			if(num%10 != 0)
			{
				out_str += Func_Read_Digit(StringUtil::num2str(num%10));
			}
			
			return StringUtil::trim(out_str);
		}

		out_str += integer2Han_hundred(UTF8Util::SkipNChars(thousand_text.c_str(), 1));
		
		return StringUtil::trim(out_str);
	}
	return out_str;
}
	
string Norm_ZH::integer2Han(string text)
{
	string temp_text = text;
	while(true)
	{
		int length = UTF8Util::StrCharLength(temp_text.c_str());
		if(length>1 && UTF8Util::NextNChars(temp_text.c_str(), 1)==utf8("0"))
		{
			temp_text = UTF8Util::SkipNChars(temp_text.c_str(), 1);
		}else
		{
			break;
		}
	}

	int length = UTF8Util::StrCharLength(temp_text.c_str());
	if(length <= 4)
	{
		return integer2Han_thousand(temp_text);
	}

	string out_str = "";

	string rev_text;
	rev_text.assign(temp_text.rbegin(), temp_text.rend());
	
	//cout << "temp_text:\t" << temp_text << endl;
	//cout << "rev_text:\t" << rev_text << endl;

	for(int i=0; i<=(length-1)/4; ++i)
	{
		int end_idx = (i+1)*4;
		if(length < end_idx)
		{
			end_idx = length;
		}
		string thousand_text = UTF8Util::SkipNChars(rev_text.c_str(), i*4);
		thousand_text = UTF8Util::NextNChars(thousand_text.c_str(), end_idx-(i*4));
		bool isAllZero = true;
		string temp_thousand_text = thousand_text;
		while(UTF8Util::StrCharLength(temp_thousand_text.c_str()) > 0)
		{
			string temp_ch = UTF8Util::NextNChars(temp_thousand_text.c_str(), 1);
			if(temp_ch != utf8("0"))
			{
				isAllZero = false;
				break;
			}
			temp_thousand_text = UTF8Util::SkipNChars(temp_thousand_text.c_str(), 1);

		}
		if(isAllZero)
		{
			continue;
		}
		else
		{
			string norm_thousand_text;
			norm_thousand_text.assign(thousand_text.rbegin(), thousand_text.rend());
			//cout << "thousand_text:\t" << thousand_text << endl;
			//cout << "norm_thousand_text:\t" << norm_thousand_text << endl;
			string units_ten_thous;
			switch(i)
			{
			case 0:
				units_ten_thous = utf8("");
				break;
			case 1:
				units_ten_thous = utf8("万");
				break;
			case 2:
				units_ten_thous = utf8("亿");
				break;
			case 3:
				units_ten_thous = utf8("万亿");
				break;
			default:
				units_ten_thous = utf8(" ");
				break;

			}
			out_str = integer2Han_thousand(norm_thousand_text)+units_ten_thous+out_str;
		}

	}

	return out_str;
}

string Norm_ZH::Func_Read_Abbr(string text)
{


	return "";
}

string Norm_ZH::Func_Read_En_Seq(string text)
{
	string out_str = "";
	string temp_text = text;

	for(int i=0; i<temp_text.length(); ++i)
	{
		//cout << i << ", " << temp_text[i] << endl;
		if(temp_text[i]>='a' && temp_text[i]<='z')
		{
			out_str += "_";
			out_str += char(temp_text[i]-32);
			//cout << "if: " << char(temp_text[i]-32) << endl;
		}
		else
		{
			out_str += "_";
			out_str += char(temp_text[i]);
		}
		//cout << "now, " << out_str << endl;
	}

	return out_str;
}

string Norm_ZH::Func_Read_Word(string text)
{
	string out_str = "";

	out_str = text;

	return utf8("_")+out_str;

}

string Norm_ZH::Func_Read_Symbol(string text)
{
	return "";
}

string Norm_ZH::Func_Read_Tran(string text, string tran_name)
{
	map<string, vector<Tran_Obj>>::iterator iter = TextNorm::tran_map.find(tran_name);

	vector<Tran_Obj> tran_vec;
	if(iter != TextNorm::tran_map.end())
	{
		tran_vec = iter->second;
	}

	for(int i=0; i<tran_vec.size(); ++i)
	{
		Tran_Obj tran_obj = tran_vec.at(i);
		if(tran_obj.GetKey() == text)
		{
			return tran_obj.GetValue();
		}
	}
	
	return text;
}

string Norm_ZH::read_num_seq(string text)
{
	string out_str = "";
	string temp_text = text;

	string::size_type lastPos = 0;

	CRegexpT <char> regexp_num("([0-9]+)");
	MatchResult result_num = regexp_num.Match(temp_text.c_str());
	while(result_num.IsMatched())
	{
		string mat_str = temp_text.substr(result_num.GetStart(), result_num.GetEnd()-result_num.GetStart());

		out_str += temp_text.substr(lastPos, result_num.GetStart()-lastPos);
		out_str += Func_Read_Num(mat_str);

		lastPos = result_num.GetEnd();

		result_num = regexp_num.Match(temp_text.c_str(), lastPos);
	}

	out_str += temp_text.substr(lastPos, temp_text.length()-lastPos);

	return StringUtil::trim(out_str);
}

string Norm_ZH::read_tel_seq(string text)
{
	string out_str = "";
	string temp_text = text;

	string::size_type lastPos = 0;

	CRegexpT <char> regexp_num("([0-9]+)");
	MatchResult result_num = regexp_num.Match(temp_text.c_str());
	while(result_num.IsMatched())
	{
		string mat_str = temp_text.substr(result_num.GetStart(), result_num.GetEnd()-result_num.GetStart());

		out_str += temp_text.substr(lastPos, result_num.GetStart()-lastPos);
		out_str += Func_Read_Tel(mat_str);

		lastPos = result_num.GetEnd();

		result_num = regexp_num.Match(temp_text.c_str(), lastPos);
	}

	out_str += temp_text.substr(lastPos, temp_text.length()-lastPos);

	return StringUtil::trim(out_str);
}

//Digit  Tel  Num  Abbr  En_Seq   Word, 只设置这几个标签，可以组合如：Digit-En_Seq
string Norm_ZH::Func_Norm_Zh(string text)
{
	return "";
}
////Email   Url

string Norm_ZH::full2half(string text)
{
	map<string, vector<Tran_Obj>>::iterator iter = TextNorm::tran_map.find("full_half_tbl");

	vector<Tran_Obj> tran_vec;
	map<string, string> local_tran_map;
	if(iter != TextNorm::tran_map.end())
	{
		tran_vec = iter->second;
	}

	for(int i=0; i<tran_vec.size(); ++i)
	{
		Tran_Obj tran_obj = tran_vec.at(i);
		local_tran_map.insert(make_pair(tran_obj.GetKey(), tran_obj.GetValue()));
	}

	string out_str = "";
	string temp_text = text;
	for(int idx=0; idx<UTF8Util::StrCharLength(text.c_str()); ++idx)
	{
		string ch = UTF8Util::NextNChars(temp_text.c_str(), 1);

		map<string, string>::iterator local_iter = local_tran_map.find(ch);
		if(local_iter != local_tran_map.end())
		{
			out_str += local_iter->second;
		}
		else
		{
			out_str += ch;
		}

		temp_text = UTF8Util::SkipNChars(temp_text.c_str(), 1);
	}
	//cout << "full2half : " << text << " ==> " << out_str << endl;
	return out_str;
}


string Norm_ZH::read_int(string number_str)
{
	string out_str = "";
	string temp_text = number_str;

	string ch = UTF8Util::NextNChars(temp_text.c_str(), 1);
	if(utf8("-")==ch)
	{
		out_str += utf8("负");
		temp_text = UTF8Util::SkipNChars(temp_text.c_str(), 1);
	}

	if(UTF8Util::StrCharLength(temp_text.c_str())>=16)
	{
		out_str += Func_Read_Digit(temp_text);
		return StringUtil::trim(temp_text);
	}
	else
	{
		out_str += integer2Han(temp_text);
		return StringUtil::trim(out_str);
	}

}

string Norm_ZH::read_decimal(string number_str)
{
	string out_str = "";
	string temp_text = number_str;

	vector<string> int_decimal;
	StringUtil::split(int_decimal, temp_text, utf8(".")); 
	
	out_str += read_int(int_decimal[0]);
	if(int_decimal.size() >= 2)
	{
		out_str += utf8("点");
		out_str += Func_Read_Digit(int_decimal[1]);
	}

	return StringUtil::trim(out_str);
}

string Norm_ZH::read_real(string number_str)
{
	return read_decimal(number_str);
}

string Norm_ZH::read_fraction(string number_str)
{
	string out_str = "";
	string temp_text = number_str;

	string ch = UTF8Util::NextNChars(temp_text.c_str(), 1);
	if(utf8("-")==ch)
	{
		out_str += utf8("负");
		temp_text = UTF8Util::SkipNChars(temp_text.c_str(), 1);
	}

	vector<string> int_fraction;
	StringUtil::split(int_fraction, temp_text, utf8("/")); 
	
	if(int_fraction.size() >= 2)
	{
		out_str += integer2Han(int_fraction[1]);
		out_str += utf8("分之");
	}
	out_str += read_int(int_fraction[0]);

	return StringUtil::trim(out_str);
}

string Norm_ZH::read_percentage(string number_str)
{
	string out_str = "";
	string temp_text = number_str;

	size_t length = UTF8Util::StrCharLength(temp_text.c_str());
	string ch = UTF8Util::Substr(temp_text.c_str(), length-1, length);
	if(utf8("‰")==ch)
	{
		out_str += utf8("千分之");
		temp_text = UTF8Util::Substr(temp_text.c_str(), 0, length-1);
	}
	else if(utf8("%")==ch || utf8("％")==ch)
	{
		out_str += utf8("百分之");
		temp_text = UTF8Util::Substr(temp_text.c_str(), 0, length-1);
	}

	out_str += read_real(temp_text);

	return StringUtil::trim(out_str);
}

string Norm_ZH::read_ratio(string number_str)
{
	string out_str = "";
	string temp_text = number_str;

	vector<string> ratio_vec;
	StringUtil::split(ratio_vec, temp_text, utf8(":"));

	if(ratio_vec.size() < 2)
	{
		ratio_vec.clear();
		StringUtil::split(ratio_vec, temp_text, utf8("："));
	}
	
	out_str += read_real(ratio_vec[0]);

	if(ratio_vec.size() >= 2)
	{
		out_str += utf8("比");
		out_str += read_real(ratio_vec[1]);
	}
	
	return StringUtil::trim(out_str);
}

string Norm_ZH::read_interval(string number_str)
{
	string out_str = "";
	string temp_text = number_str;

	vector<string> interval_vec;
	StringUtil::split_or(interval_vec, temp_text, utf8("-|~|—|～"));
	
	out_str += read_real(interval_vec[0]);

	if(interval_vec.size() >= 2)
	{
		out_str += utf8("到");
		out_str += read_real(interval_vec[1]);
	}
	
	return StringUtil::trim(out_str);
}

string Norm_ZH::read_number_exp(string number_exp)
{
	return "";
}

string Norm_ZH::read_en_number_mixture(string sent)
{
	map<string, vector<Tran_Obj>>::iterator iter = TextNorm::tran_map.find("read_digit");

	vector<Tran_Obj> tran_vec;
	map<string, string> local_tran_map;
	if(iter != TextNorm::tran_map.end())
	{
		tran_vec = iter->second;
	}

	for(int i=0; i<tran_vec.size(); ++i)
	{
		Tran_Obj tran_obj = tran_vec.at(i);
		local_tran_map.insert(make_pair(tran_obj.GetKey(), tran_obj.GetValue()));
		//cout << tran_obj.GetKey() << ", " << tran_obj.GetValue() << endl;
	}

	string out_str = "";
	string temp_text = sent;

	for(int idx=0; idx<UTF8Util::StrCharLength(sent.c_str()); ++idx)
	{
		string ch = UTF8Util::NextNChars(temp_text.c_str(), 1);
		//cout << "Each char:" << ch << endl;
		map<string, string>::iterator local_iter = local_tran_map.find(ch);
		if(local_iter != local_tran_map.end())
		{
			out_str += local_iter->second;
			//cout << local_iter->second << endl;
		}
		else if(ch==utf8("-"))
		{
			out_str += utf8("杠");
		}
		else
		{
			out_str += Func_Read_En_Seq(ch);
		}

		temp_text = UTF8Util::SkipNChars(temp_text.c_str(), 1);
	}

	return out_str;
}


string Norm_ZH::read_date(string date_str)
{
	string out_str = read_date_format(date_str);
	if(out_str != "")
	{
		return out_str;
	}

	out_str = read_date_text(date_str);
	if(out_str != "")
	{
		return out_str;
	}

	out_str = read_date_seq(date_str);
	if(out_str != "")
	{
		return out_str;
	}

	return out_str;
}

string Norm_ZH::read_date_format(string date_str)
{
	string date_sep_str = utf8("/|-|\\.");
	vector<string> date_sep_vec;
	StringUtil::split(date_sep_vec, date_sep_str, utf8("|"));

	string out_str = "";
	string temp_date_str = date_str;

	for(int idx=0; idx<date_sep_vec.size(); ++idx)
	{
		string year_month_day_exp = utf8("([1-9][\\d]{0,3})")+date_sep_vec.at(idx)+utf8("(0?[1-9]|1[0-2])")+date_sep_vec.at(idx)+utf8("([0-2]?[0-9]|3[0-1])");
		CRegexpT<char> year_month_day_regexp(year_month_day_exp.c_str());

		MatchResult year_month_day_result = year_month_day_regexp.Match(temp_date_str.c_str());
		if(year_month_day_result.IsMatched())
		{
			out_str += Func_Read_Digit(temp_date_str.substr(year_month_day_result.GetGroupStart(1), year_month_day_result.GetGroupEnd(1)-year_month_day_result.GetGroupStart(1)));
			out_str += utf8("年");
			out_str += Func_Read_Num(temp_date_str.substr(year_month_day_result.GetGroupStart(2), year_month_day_result.GetGroupEnd(2)-year_month_day_result.GetGroupStart(2)));
			out_str += utf8("月");
			out_str += Func_Read_Num(temp_date_str.substr(year_month_day_result.GetGroupStart(3), year_month_day_result.GetGroupEnd(3)-year_month_day_result.GetGroupStart(3)));
			out_str += utf8("日");
			break;
		}

		string year_month_exp = utf8("([1-9][\\d]{0,3})")+date_sep_vec.at(idx)+utf8("(0?[1-9]|1[0-2])");
		CRegexpT<char> year_month_regexp(year_month_exp.c_str());

		MatchResult year_month_result = year_month_regexp.Match(temp_date_str.c_str());
		if(year_month_result.IsMatched())
		{
			out_str += Func_Read_Digit(temp_date_str.substr(year_month_day_result.GetGroupStart(1), year_month_day_result.GetGroupEnd(1)-year_month_day_result.GetGroupStart(1)));
			out_str += utf8("年");
			out_str += Func_Read_Num(temp_date_str.substr(year_month_day_result.GetGroupStart(2), year_month_day_result.GetGroupEnd(2)-year_month_day_result.GetGroupStart(2)));
			out_str += utf8("月");
			break;
		}

		string month_day_exp = utf8("(0?[1-9]|1[0-2])")+date_sep_vec.at(idx)+utf8("([0-2]?[0-9]|3[0-1])");
		CRegexpT<char> month_day_regexp(month_day_exp.c_str());

		MatchResult month_day_result = month_day_regexp.Match(temp_date_str.c_str());
		if(month_day_result.IsMatched())
		{
			out_str += Func_Read_Num(temp_date_str.substr(year_month_day_result.GetGroupStart(1), year_month_day_result.GetGroupEnd(1)-year_month_day_result.GetGroupStart(1)));
			out_str += utf8("月");
			out_str += Func_Read_Num(temp_date_str.substr(year_month_day_result.GetGroupStart(2), year_month_day_result.GetGroupEnd(2)-year_month_day_result.GetGroupStart(2)));
			out_str += utf8("日");
			break;
		}
	}

	//cout << "Read date format " << endl;
	return StringUtil::trim(out_str);
}
	
string Norm_ZH::read_date_text(string date_str)
{
	string out_str = "";
	string temp_date_str = date_str;

	string year_exp = utf8("([0-9]{2}|[0-9]?[0-9]{3})(?=年)");
	CRegexpT <char> year_regexp(year_exp.c_str());
	MatchResult year_result = year_regexp.Match(temp_date_str.c_str());
	if(year_result.IsMatched())
	{
		out_str += Func_Read_Digit(temp_date_str.substr(year_result.GetStart(), year_result.GetEnd()-year_result.GetStart()));
		out_str += utf8("年");
	}

	string month_exp = utf8("((10)|(11)|(12)|(0?[1-9]))(?=月)");
	CRegexpT <char> month_regexp(month_exp.c_str());
	MatchResult month_result = month_regexp.Match(temp_date_str.c_str());
	if(month_result.IsMatched())
	{
		out_str += Func_Read_Num(temp_date_str.substr(month_result.GetStart(), month_result.GetEnd()-month_result.GetStart()));
		out_str += utf8("月");
	}

	string day_exp = utf8("((?<!\\d))([0-2]?[0-9]|3[0-1])(?=(日|号))");
	CRegexpT <char> day_regexp(day_exp.c_str());
	MatchResult day_result = day_regexp.Match(temp_date_str.c_str());
	if(day_result.IsMatched())
	{
		out_str += Func_Read_Num(temp_date_str.substr(day_result.GetStart(), day_result.GetEnd()-day_result.GetStart()));
		out_str += UTF8Util::NextNChars(temp_date_str.substr(day_result.GetEnd()).c_str(), 1);
	}

	//cout << "Read date text " << endl;

	return StringUtil::trim(out_str);
}

string Norm_ZH::read_date_seq(string date_str)
{
	string out_str = "";
	string temp_date_str = date_str;

	string year_month_day_seq = utf8("((19|20)[\\d]{2})(0[1-9]|1[0-2])([0-2]?[0-9]|3[0-1])");
	CRegexpT<char> year_month_day_regexp(year_month_day_seq.c_str());

	MatchResult year_month_day_result = year_month_day_regexp.Match(temp_date_str.c_str());
	if(year_month_day_result.IsMatched())
	{
		out_str += Func_Read_Digit(temp_date_str.substr(year_month_day_result.GetGroupStart(1), year_month_day_result.GetGroupEnd(1)-year_month_day_result.GetGroupStart(1)));
		out_str += utf8("年");
		out_str += Func_Read_Num(temp_date_str.substr(year_month_day_result.GetGroupStart(3), year_month_day_result.GetGroupEnd(3)-year_month_day_result.GetGroupStart(3)));
		out_str += utf8("月");
		out_str += Func_Read_Num(temp_date_str.substr(year_month_day_result.GetGroupStart(4), year_month_day_result.GetGroupEnd(4)-year_month_day_result.GetGroupStart(4)));
		out_str += utf8("日");
	}

	//cout << "Read date seq " << endl;
	
	return StringUtil::trim(out_str);
}

string Norm_ZH::read_time(string time_str)
{
	string pre_str = "";
	string temp_time_str = time_str;

	string am_exp = utf8("( |　)?[aA][mM]");
	string pm_exp = utf8("( |　)?[pP][mM]");
	CRegexpT <char> am_regexp(am_exp.c_str());
	CRegexpT <char> pm_regexp(pm_exp.c_str());
	MatchResult am_result = am_regexp.Match(temp_time_str.c_str());
	if(am_result.IsMatched())
	{
		temp_time_str = temp_time_str.substr(0, am_result.GetStart()) + temp_time_str.substr(am_result.GetEnd());
		pre_str = utf8("上午 ");
	}

	MatchResult pm_result = pm_regexp.Match(temp_time_str.c_str());
	if(pm_result.IsMatched())
	{
		temp_time_str = temp_time_str.substr(0, pm_result.GetStart()) + temp_time_str.substr(pm_result.GetEnd());
		pre_str = utf8("下午 ");
	}

	string out_str = read_time_format(temp_time_str);
	if(out_str != "")
	{
		return pre_str+out_str;
	}

	out_str = read_time_text(temp_time_str);
	if(out_str != "")
	{
		return pre_str+out_str;
	}

	return out_str;
}

string Norm_ZH::read_time_format(string time_str)
{
	string time_sep_str = utf8("：|:");
	vector<string> time_sep_vec;
	StringUtil::split(time_sep_vec, time_sep_str, utf8("|"));

	string out_str = "";
	string temp_time_str = time_str;
	for(int idx=0; idx<time_sep_vec.size(); ++idx)
	{
		string h_m_s_exp = utf8("([0-1]?[0-9]|2[0123])")+time_sep_vec.at(idx)+utf8("([0-5]?[0-9])")+time_sep_vec.at(idx)+utf8("([0-5]?[0-9])");
		CRegexpT<char> h_m_s_regexp(h_m_s_exp.c_str());

		MatchResult h_m_s_result = h_m_s_regexp.Match(temp_time_str.c_str());
		if(h_m_s_result.IsMatched())
		{
			out_str += Func_Read_Num(temp_time_str.substr(h_m_s_result.GetGroupStart(1), h_m_s_result.GetGroupEnd(1)-h_m_s_result.GetGroupStart(1)));
			out_str += utf8("时");
			out_str += Func_Read_Num(temp_time_str.substr(h_m_s_result.GetGroupStart(2), h_m_s_result.GetGroupEnd(2)-h_m_s_result.GetGroupStart(2)));
			out_str += utf8("分");
			out_str += Func_Read_Num(temp_time_str.substr(h_m_s_result.GetGroupStart(3), h_m_s_result.GetGroupEnd(3)-h_m_s_result.GetGroupStart(3)));
			out_str += utf8("秒");
			break;
		}

		string h_m_exp = utf8("([0-1]?[0-9]|2[0123])")+time_sep_vec.at(idx)+utf8("([0-5]?[0-9])");
		CRegexpT<char> h_m_regexp(h_m_exp.c_str());

		MatchResult h_m_result = h_m_regexp.Match(temp_time_str.c_str());
		if(h_m_result.IsMatched())
		{
			out_str += Func_Read_Num(temp_time_str.substr(h_m_result.GetGroupStart(1), h_m_result.GetGroupEnd(1)-h_m_result.GetGroupStart(1)));
			out_str += utf8("时");
			out_str += Func_Read_Num(temp_time_str.substr(h_m_result.GetGroupStart(2), h_m_result.GetGroupEnd(2)-h_m_result.GetGroupStart(2)));
			out_str += utf8("分");
			break;
		}

		string m_s_exp = utf8("([0-5]?[0-9])")+time_sep_vec.at(idx)+utf8("([0-5]?[0-9])");
		CRegexpT<char> m_s_regexp(m_s_exp.c_str());

		MatchResult m_s_result = m_s_regexp.Match(temp_time_str.c_str());
		if(m_s_result.IsMatched())
		{
			out_str += Func_Read_Num(temp_time_str.substr(m_s_result.GetGroupStart(1), m_s_result.GetGroupEnd(1)-m_s_result.GetGroupStart(1)));
			out_str += utf8("分");
			out_str += Func_Read_Num(temp_time_str.substr(m_s_result.GetGroupStart(2), m_s_result.GetGroupEnd(2)-m_s_result.GetGroupStart(2)));
			out_str += utf8("秒");
			break;
		}
	}

	return StringUtil::trim(out_str);
}

string Norm_ZH::read_time_text(string time_str)
{
	string out_str = "";
	string temp_time_str = time_str;

	string h_exp = utf8("(?<!(周|星期))([0-2]?[0-9])(?=(点|时|h|H))");
	CRegexpT <char> h_regexp(h_exp.c_str());
	MatchResult h_result = h_regexp.Match(temp_time_str.c_str());
	if(h_result.IsMatched())
	{
		out_str += Func_Read_Num(temp_time_str.substr(h_result.GetStart(), h_result.GetEnd()-h_result.GetStart()));
		if(temp_time_str.find(utf8("点")) != string::npos)
		{
			out_str += utf8("点");
		}
		else
		{
			out_str += utf8("时");
		}

	}

	string m_exp = utf8("([0-5]?[0-9](?=(分(?!钟)|m|M)))|((?<=((?<!小)[点时]|h|H))[0-5]?[0-9](?!刻))");
	CRegexpT <char> m_regexp(m_exp.c_str());
	MatchResult m_result = m_regexp.Match(temp_time_str.c_str());
	if(m_result.IsMatched())
	{
		out_str += Func_Read_Num(temp_time_str.substr(m_result.GetStart(), m_result.GetEnd()-m_result.GetStart()));
		out_str += utf8("分");
	}

	string s_exp = utf8("([0-5]?[0-9](?=(秒|s|S)))|((?<=(分|m|M))[0-5]?[0-9])");
	CRegexpT <char> s_regexp(s_exp.c_str());
	MatchResult s_result = s_regexp.Match(temp_time_str.c_str());
	if(s_result.IsMatched())
	{
		out_str += Func_Read_Num(temp_time_str.substr(s_result.GetStart(), s_result.GetEnd()-s_result.GetStart()));
		out_str += utf8("秒");
	}

	return StringUtil::trim(out_str);
}

string Norm_ZH::read_time_section(string time_str)
{
	return "";
}

string Norm_ZH::read_email(string email_str)
{
	map<string, string> local_tran_map;
	{
		local_tran_map.insert(make_pair(utf8("."), utf8(" 点 ")));
		local_tran_map.insert(make_pair(utf8("@"), utf8(" 花_A ")));
		local_tran_map.insert(make_pair(utf8("-"), utf8(" 杠 ")));
		local_tran_map.insert(make_pair(utf8("_"), utf8(" 下划线 ")));
	}

	return read_en_number_symbol_mixture(email_str, local_tran_map);
}

string Norm_ZH::read_url(string url_str)
{
	map<string, string> local_tran_map;
	{
		local_tran_map.insert(make_pair(utf8("."), utf8(" 点 ")));
		local_tran_map.insert(make_pair(utf8("－"), utf8(" 杠 ")));
		local_tran_map.insert(make_pair(utf8("-"), utf8(" 杠 ")));
		local_tran_map.insert(make_pair(utf8("_"), utf8(" 下划线 ")));
		local_tran_map.insert(make_pair(utf8("%"), utf8(" 百分号 ")));
		local_tran_map.insert(make_pair(utf8("/"), utf8(" 斜杠 ")));
		local_tran_map.insert(make_pair(utf8("="), utf8(" 等号 ")));
		local_tran_map.insert(make_pair(utf8("&"), utf8(" 和号 ")));
		local_tran_map.insert(make_pair(utf8(":"), utf8(" 冒号 ")));
		local_tran_map.insert(make_pair(utf8("："), utf8(" 冒号 ")));
		local_tran_map.insert(make_pair(utf8("？"), utf8(" 问号 ")));
		local_tran_map.insert(make_pair(utf8("?"), utf8(" 问号 ")));
		local_tran_map.insert(make_pair(utf8("#"), utf8(" 井号 ")));
		local_tran_map.insert(make_pair(utf8("$"), utf8(" 美元 ")));
	}

	return read_en_number_symbol_mixture(url_str, local_tran_map);
}

string Norm_ZH::read_en_number_symbol_mixture(string sent, map<string, string> local_tran_map)
{
	map<string, vector<Tran_Obj>>::iterator iter = TextNorm::tran_map.find("read_tel");
	vector<Tran_Obj> tran_vec;
	map<string, string> digit_tran_map;
	if(iter != TextNorm::tran_map.end())
	{
		tran_vec = iter->second;
	}

	for(int i=0; i<tran_vec.size(); ++i)
	{
		Tran_Obj tran_obj = tran_vec.at(i);
		digit_tran_map.insert(make_pair(tran_obj.GetKey(), tran_obj.GetValue()));
	}

	string out_str = "";
	string temp_text = sent;

	for(int idx=0; idx<UTF8Util::StrCharLength(sent.c_str()); ++idx)
	{
		string ch = UTF8Util::NextNChars(temp_text.c_str(), 1);
		map<string, string>::iterator local_iter = local_tran_map.find(ch);
		if(local_iter != local_tran_map.end())
		{
			out_str += local_iter->second;
		}
		else
		{
			map<string, string>::iterator digit_iter = digit_tran_map.find(ch);
			if(digit_iter != digit_tran_map.end())
			{
				out_str += digit_iter->second;
			}
			else
			{
				out_str += Func_Read_En_Seq(ch);
			}
		}

		temp_text = UTF8Util::SkipNChars(temp_text.c_str(), 1);
	}

	return StringUtil::trim(out_str);
}

string Norm_ZH::read_phone(string phone_str)
{
	string temp_phone_str = phone_str;
	CRegexpT <char> regexp_Ext(utf8("((\\([+＋]?86\\)0?(10|2[\\d]|[3-9][\\d]{2})[\\-－][\\dxX]{6,8}(([Ee][Xx][Tt]|转|/|-|－)[\\d]{1,6})?)|([+＋]?86[\\-－]0?(10|2[\\d]|[3-9][\\d]{2})[\\-－][\\dxX]{6,8}(([Ee][Xx][Tt]|转|/|-|－)[\\d]{1,6})?)|(\\(0(10|2[\\d]|[3-9][\\d]{2})\\)[\\dxX]{6,8}(([Ee][Xx][Tt]|转|/|-|－)[\\d]{1,6})?)|(0(10|2[\\d]|[3-9][\\d]{2})[\\-－][\\dxX]{6,8}(([Ee][Xx][Tt]|转|/|-|－)[\\d]{1,6})?)|([\\dxX]{6,8}(([Ee][Xx][Tt]|转|/|-|－)[\\d]{1,6})?))").c_str());
	MatchResult result_Ext = regexp_Ext.Match(temp_phone_str.c_str());
	if(result_Ext.IsMatched())
	{
		CRegexpT <char> regexp_Ext_suf(utf8("[\\-－](?=[\\d]{1,6}$)").c_str());
		temp_phone_str = regexp_Ext_suf.Replace(temp_phone_str.c_str(), utf8("转").c_str());
	}

	CRegexpT <char> regexp_Ext_str(utf8("[Ee][Xx][Tt]").c_str());
	temp_phone_str = regexp_Ext_str.Replace(temp_phone_str.c_str(), utf8("转").c_str());

	map<string, string> local_tran_map;
	{
		local_tran_map.insert(make_pair(utf8("x"), utf8("某")));
		local_tran_map.insert(make_pair(utf8("X"), utf8("某")));
		local_tran_map.insert(make_pair(utf8("("), utf8("")));
		local_tran_map.insert(make_pair(utf8(")"), utf8("")));
		local_tran_map.insert(make_pair(utf8("-"), utf8(" ")));
		local_tran_map.insert(make_pair(utf8("－"), utf8(" ")));
		local_tran_map.insert(make_pair(utf8("+"), utf8("")));
		local_tran_map.insert(make_pair(utf8("＋"), utf8("")));
		local_tran_map.insert(make_pair(utf8("/"), utf8("转")));
		local_tran_map.insert(make_pair(utf8("转"), utf8("转")));
	}

	return read_en_number_symbol_mixture(temp_phone_str, local_tran_map);
}

string Norm_ZH::test()
{

	cout << full2half(utf8("１２３４５６０７０ａｂｃＡＲＱ")) << endl;
	cout << Func_Read_Digit(utf8("0123456789")) << endl;
	cout << Func_Read_Tel(utf8("0123456789")) << endl;
	cout << Func_Read_Num(utf8("1234567890")) << endl;

	cout << Func_Read_En_Seq(utf8("abcHello World")) << endl;

	cout << UTF8Util::Substr(utf8("我是一个中国人，我深深的爱着我的祖国和人民。").c_str(), 3, 6) << endl;

	cout << read_en_number_mixture(utf8("MH-370")) << endl;
	cout << read_int(utf8("-123456789012340")) << endl;
	cout << read_decimal(utf8("-123.5770")) << ", " << read_real(utf8("-0.013")) << endl;
	cout << read_fraction(utf8("87/2400")) << endl;
	cout << read_percentage(utf8("15‰")) << "," << read_percentage(utf8("7%")) << endl;
	cout << read_ratio(utf8("49:51")) << endl;
	cout << read_interval(utf8("1000～2000")) << endl;
	cout << read_email(utf8("yxx@speechocean.com")) << endl;
	cout << read_url(utf8("http://202.108.174.83/datacollector")) << endl;
	cout << read_phone(utf8("(+86)010-8065542-12")) << endl;

	cout << read_date_format(utf8("2014/12/10")) << endl;
	cout << read_date_text(utf8("2015年09月18日")) << endl;
	cout << read_date_seq(utf8("20150214")) << endl;

	cout << read_time_format(utf8("13:40:00")) << endl;
	cout << read_time_text(utf8("13点40分30秒")) << endl;

	string test_str = "8065532Ext12";
	CRegexpT <char> regexp_("[Ee][Xx][Tt]");
	string res_str = regexp_.Replace(test_str.c_str(), "Zhuan");
	cout << "test_str=" << test_str << ", res_str="<< res_str << endl;

	test_str = utf8("期待您的来电：13961841986张开元，顺颂商褀！如果您有任何疑问，请您拨打新浪网唯一客服热线：95105670进行咨询和举报。");
	CRegexpT <char> regexp_test(utf8("((?<TMat_1>((电话|传真|电话号码|传真号码|手机|手机号|手机号码|热线|联系方式|来电)(是)?(:|：)?))(?<TMat_2>([ 　]*[\\+＋]?[\\dxX]([\\dxX]|([\\dxX]{5,7}(([Ee][Xx][Tt])|转|/|[\\-－]))|(\\([\\+＋]?[\\dxX]+\\))|[\\-－])*[\\dxX])))").c_str());
	MatchResult result_test = regexp_test.Match(test_str.c_str());
	if(result_test.IsMatched())
	{
		cout << "Matched, " << test_str.substr(result_test.GetStart(), result_test.GetEnd()-result_test.GetStart()) << endl;
	}
	else
	{
		cout << "Not Matched!" << endl;
	}



	return "";
}

string Norm_ZH::testBasicNSWs()
{
	BasicNSWs basicNSWs;
	basicNSWs.Init();

	//basicNSWs.TestDeelx();
	//basicNSWs.LabelNSWs(utf8("2015/09/06 2015-09-07 2013.11.06 今年2015年09月03日是抗日战争胜利70周年！阅兵仪式上午10:02:30准时开始。新中国成立于1949年10月1日，下午3点15分30秒开始国庆大典。现在时间是16:43:00。我的邮箱是beifeng300@126.com, yxx@speechocean.com。公司网站是http://219.131.222.62/，点击链接试一下。010-87798915，0773-8815198/9，13825557299，(010)63263377，(+86)010-8065542-12，(010)8065543-1234，8065543转21，135-1130-2459，-1234567890, 地球2/3是海洋。99%准确率"));
	//basicNSWs.ParseText(utf8("2015/09/06 2015-09-07 2013.11.06 今年2015年09月03日是抗日战争胜利70周年！阅兵仪式上午10:02:30准时开始。新中国成立于1949年10月1日，下午3点15分30秒开始国庆大典。现在时间是16:43:00。我的邮箱是beifeng300@126.com, yxx@speechocean.com。公司网站是http://219.131.222.62/，点击链接试一下。010-87798915，0773-8815198/9，13825557299，(010)63263377，(+86)010-8065542-12，(010)8065543-1234，8065543转21，135-1130-2459，-1234567890, 地球2/3是海洋。99%准确率"));
	
	test();

	//basicNSWs.ParseText(utf8("《陈独秀文章燕》第346—353页，生活·读书·新知三联书店，1984年出版。"));

	basicNSWs.ParseFile("data/Testdata.txt", "data/Testdata.txt.tn");	//yqw 20151106

	//basicNSWs.ParseFile("D:/testdata/email.txt", "D:/testdata/email.txt.tn");
	//basicNSWs.match_exp(utf8("2015Hello/month1209/08").c_str());

	return "";

}


string Norm_ZH::TestSingle()
{

	TN_Engine tn_engine;

	cout << "Here" << endl;
	cout << tn_engine.ProcessText(utf8("123")) << endl;

	return "";
}
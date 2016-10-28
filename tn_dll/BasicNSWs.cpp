#include "StdAfx.h"
#include <algorithm>
#include "BasicNSWs.h"
#include "TN_Engine.h"
#include "UTF8Util.h"
#include "StringUtil.h"
#include "regexp\deelx.h"
#include "Rule_Match_Obj.h"



vector<Rule_Obj> BasicNSWs::rule_vec;


BasicNSWs::BasicNSWs(void)
{
}


BasicNSWs::~BasicNSWs(void)
{
}

void BasicNSWs::Init()
{
	Load_rules();
	Preprocess_rules();

}

//读取规则库 xx.rules, 读入rule_vec 内，
int BasicNSWs::Load_rules()
{
	string lang = TN_Engine::getLang();
	if(lang=="")
	{
		cout<<"Error, language has not been initialized"<<endl;
		lang = "zh";
		//return -1;
	}

	string rule_file_path = "rules/"+lang+".rules";
	if(TN_Engine::getModel_Dir() != "")
	{
		rule_file_path = TN_Engine::getModel_Dir()+"/"+rule_file_path;
	}

	const int ENTRY_BUFF_SIZE = 4096;
	char buff[ENTRY_BUFF_SIZE];

	//cout << rule_file_path << endl;
	FILE* fp = fopen(rule_file_path.c_str(), "r");
	UTF8Util::SkipUtf8Bom(fp);

	string rule_name;
	string rule_text;
	string rule_restrictions;
	int rule_priority;
	string rule_transform_exp;
	int isUsableMark;

	Rule_Obj rule_obj;
	size_t lineNum = 0;
	int rule_id = 0;
	while(fgets(buff, ENTRY_BUFF_SIZE, fp))
	{
		const char* inbuff = buff;
		size_t length;

		const char* pbuff = UTF8Util::FindNextInline(inbuff, '\n');
		if(UTF8Util::IsLineEndingOrFileEnding(*pbuff))
		{}

		length = static_cast<size_t>(pbuff-inbuff);
		string key = UTF8Util::FromSubstr(inbuff, length);

		if(key.length() == 0)
		{
			continue;
		}

		if(UTF8Util::FromSubstr(key.c_str(), 4) == "####")
		{
			continue;
		}
		
		vector<string> contents;
		StringUtil::split(contents, key, '\t');

		//cout << "len of contents: " << contents.size() << endl;
		//cout << "key: " << key << endl;
		rule_id += 1;
		rule_name = contents.at(0);
		rule_text = contents.at(1);
		rule_restrictions = contents.at(2);
		rule_priority = atoi(contents.at(3).c_str());
		rule_transform_exp = contents.at(4);
		isUsableMark = atoi(contents.at(5).c_str());

		rule_obj.SetRule_name(rule_name);
		rule_obj.SetRule_text(rule_text);
		rule_obj.SetRule_restrictions(rule_restrictions);
		rule_obj.SetRule_priority(rule_priority);
		rule_obj.SetRule_Transform_exp(rule_transform_exp);
		rule_obj.SetIsUsable(isUsableMark);
		rule_obj.SetRule_id(rule_id);

		BasicNSWs::rule_vec.push_back(rule_obj);
		//cout << "rule:\t" << rule_obj.toString() << endl;
	}

	return 0;
}

// 在全部加载 规则库xx.rules后， 对于表达式内的嵌套结构，进行预处理 rule_text --> rule_exp
int BasicNSWs::Preprocess_rules()
{
	vector<Rule_Obj> rule_vec_local = BasicNSWs::rule_vec;

	for(int i=0; i<rule_vec_local.size(); ++i)
	{
		Rule_Obj rule_obj = rule_vec_local.at(i);

		string rule_text = rule_obj.GetRule_text();
		string rule_transform_exp = rule_obj.GetRule_transform_exp();

		string rule_exp = "";
		size_t lastPos = 0;
		CRegexpT <char> regexp_rule_text("\\(\\$((Rule|Conf|Tran)_[a-zA-Z0-9_]+)\\)");
		// loop
		MatchResult result_rule_text = regexp_rule_text.Match(rule_text.c_str());
		while(result_rule_text.IsMatched())
		{
			string replace_str = "";
			string mat_str = rule_text.substr(result_rule_text.GetGroupStart(1), result_rule_text.GetGroupEnd(1)-result_rule_text.GetGroupStart(1));
			if(UTF8Util::FromSubstr(mat_str.c_str(), 4) == "Rule")
			{
				for(int inner_idx=0; inner_idx<i; ++inner_idx)
				{
					Rule_Obj inner_rule_obj = rule_vec_local.at(inner_idx);
					if(inner_rule_obj.GetRule_name() == mat_str)
					{
						replace_str = "("+inner_rule_obj.GetRule_exp()+")";
					}
				}
			}
			else if(UTF8Util::FromSubstr(mat_str.c_str(), 4) == "Conf")
			{
				map<string, vector<string>>::iterator iter = TN_Engine::conf_map.find(UTF8Util::SkipNChars(mat_str.c_str(), 5));

				vector<string> conf_vec;
				if(iter != TN_Engine::conf_map.end())
				{
					conf_vec = iter->second;
					for(int conf_vec_idx=0; conf_vec_idx<conf_vec.size(); ++conf_vec_idx)
					{
						if(conf_vec_idx != 0)
						{
							replace_str += "|";
						}
						else
						{
							replace_str += "(";
						}
						replace_str += conf_vec.at(conf_vec_idx);
					}

					if(conf_vec.size() > 0)
					{
						replace_str += ")";
					}
				}
			}
			else if(UTF8Util::FromSubstr(mat_str.c_str(), 4) == "Tran")
			{
				map<string, vector<Tran_Obj>>::iterator iter = TextNorm::tran_map.find(UTF8Util::SkipNChars(mat_str.c_str(), 5));

				vector<Tran_Obj> tran_vec;
				if(iter != TextNorm::tran_map.end())
				{
					tran_vec = iter->second;

					for(int tran_vec_idx=0; tran_vec_idx<tran_vec.size(); ++tran_vec_idx)
					{
						Tran_Obj tran_obj = tran_vec.at(tran_vec_idx);

						if(tran_vec_idx != 0)
						{
							replace_str += "|";
						}
						else
						{
							replace_str += "(";
						}
						replace_str += tran_obj.GetKey();
					}

					if(tran_vec.size() > 0)
					{
						replace_str += ")";

						UTF8Util::ReplaceAll(replace_str, ".", "\\.");
						UTF8Util::ReplaceAll(replace_str, "+", "\\+");
						UTF8Util::ReplaceAll(replace_str, "-", "\\-");
						UTF8Util::ReplaceAll(replace_str, "?", "\\?");
						UTF8Util::ReplaceAll(replace_str, "*", "\\*");
						UTF8Util::ReplaceAll(replace_str, "^", "\\^");
						UTF8Util::ReplaceAll(replace_str, "$", "\\$");
					}
				}
			}

			if(replace_str == "")
			{
				replace_str = rule_text.substr(result_rule_text.GetStart(), result_rule_text.GetEnd()-result_rule_text.GetStart());
			}

			rule_exp += rule_text.substr(lastPos, result_rule_text.GetStart()-lastPos);
			rule_exp += replace_str;
			
			lastPos = result_rule_text.GetEnd();

			result_rule_text = regexp_rule_text.Match(rule_text.c_str(), result_rule_text.GetEnd());
		}

		rule_exp += rule_text.substr(lastPos, rule_text.length()-lastPos);
		////UTF8Util::ReplaceAll(rule_exp, "\\", "\\\\");
		//cout << "Before: " << rule_obj.toString() << endl;
		rule_obj.SetRule_exp(rule_exp);

		string rule_transform_exp_new = "";
		lastPos = 0;
		CRegexpT <char> regexp_rule(rule_obj.GetRule_exp().c_str());
		CRegexpT <char> regexp_rule_transform("\\(\\$((TMat)_[a-zA-Z0-9_]+)\\)");
		MatchResult result_rule_transform = regexp_rule_transform.Match(rule_transform_exp.c_str());
		while(result_rule_transform.IsMatched())
		{
			string replace_str = "";
			string mat_str = rule_transform_exp.substr(result_rule_transform.GetGroupStart(1), result_rule_transform.GetGroupEnd(1)-result_rule_transform.GetGroupStart(1));
			int trans_mat = regexp_rule.GetNamedGroupNumber(mat_str.c_str());
			//cout << "mat_str=" << mat_str << ", trans_mat=" << trans_mat << endl;
			if(trans_mat < 0)
			{
				replace_str = rule_transform_exp.substr(result_rule_transform.GetStart(), result_rule_transform.GetEnd()-result_rule_transform.GetStart());
			}
			else
			{
				replace_str = "$"+StringUtil::num2str(trans_mat);				
			}

			rule_transform_exp_new += rule_transform_exp.substr(lastPos, result_rule_transform.GetStart()-lastPos);
			rule_transform_exp_new += replace_str;
			
			lastPos = result_rule_transform.GetEnd();

			result_rule_transform = regexp_rule_transform.Match(rule_transform_exp.c_str(), result_rule_transform.GetEnd());
		}

		rule_transform_exp_new += rule_transform_exp.substr(lastPos, rule_transform_exp_new.length()-lastPos);

		if(rule_transform_exp_new != "")
		{
			rule_obj.SetRule_Transform_exp(StringUtil::trim(rule_transform_exp_new));
		}

		// 201509245 19:00 yqw 扩展限制条件 restrictions
		if(rule_obj.GetRule_restrictions() != "\"\"")
		{
			string rule_restrictions = rule_obj.GetRule_restrictions();
			string rule_restrictions_new = "";
			lastPos = 0;
			//CRegexpT <char> regexp_rule(rule_obj.GetRule_exp().c_str());
			CRegexpT <char> regexp_rule_restrictions("\\(\\$((TMat)_[a-zA-Z0-9_]+)\\)");
			MatchResult result_rule_restrictions = regexp_rule_restrictions.Match(rule_restrictions.c_str());
			while(result_rule_restrictions.IsMatched())
			{
				string replace_str = "";
				string mat_str = rule_restrictions.substr(result_rule_restrictions.GetGroupStart(1), result_rule_restrictions.GetGroupEnd(1)-result_rule_restrictions.GetGroupStart(1));
				int trans_mat = regexp_rule.GetNamedGroupNumber(mat_str.c_str());
				//cout << "mat_str=" << mat_str << ", trans_mat=" << trans_mat << endl;
				if(trans_mat < 0)
				{
					replace_str = rule_restrictions.substr(result_rule_restrictions.GetStart(), result_rule_restrictions.GetEnd()-result_rule_restrictions.GetStart());
				}
				else
				{
					replace_str = "$"+StringUtil::num2str(trans_mat);				
				}

				rule_restrictions_new += rule_restrictions.substr(lastPos, result_rule_restrictions.GetStart()-lastPos);
				rule_restrictions_new += replace_str;
			
				lastPos = result_rule_restrictions.GetEnd();

				result_rule_restrictions = regexp_rule_restrictions.Match(rule_restrictions.c_str(), result_rule_restrictions.GetEnd());
			}

			rule_restrictions_new += rule_restrictions.substr(lastPos, rule_restrictions.length()-lastPos);

			if(rule_restrictions_new != "")
			{
				rule_obj.SetRule_restrictions(StringUtil::trim(rule_restrictions_new));
			}
		}
		// 201509245 19:00

		rule_vec_local[i] = rule_obj;
		//cout << "After: " << rule_vec_local[i].toString() << endl;
	}

	sort(rule_vec_local.begin(), rule_vec_local.end(), Rule_Obj::Compare);

	BasicNSWs::rule_vec = rule_vec_local;

	/*for(int rule_idx=0; rule_idx<rule_vec_local.size(); ++rule_idx)
	{
		cout << rule_vec_local.at(rule_idx).toString() << endl;
	}*/

	return 0;
}

// 按照rule_vec, 对于传入的text进行解析，填入rule_match_vec
string BasicNSWs::ParseText(string text)
{
	this->text = Norm_ZH::full2half(text);
	this->rule_match_vec.clear();
	this->rule_standard_str = "";

	LabelNSWs(this->text);



	Disambiguation();

	return this->rule_standard_str;
}

//string BasicNSWs::LabelNSWs(string text)
//{
//	//匹配规则， 标记标签
//
//	//逐条 vector<Rule_Obj> rule_vec; 
//	//匹配的规则存储在 vector<Rule_Match_Obj> rule_match_vec;
//
//	string temp_str = text;
//
//	vector<Rule_Obj> rule_vec = BasicNSWs::rule_vec;
//
//	Rule_Match_Obj rule_match_obj;
//
//	for(int rule_idx=0; rule_idx<rule_vec.size(); ++rule_idx)
//	{
//		Rule_Obj rule_obj = rule_vec.at(rule_idx);
//		
//		string rule_exp = rule_obj.GetRule_exp();
//		//cout << "rule_obj = " << rule_obj.toString() << endl;   // yqw 20150918 debug
//		//cout << "rule_exp = " << rule_exp << endl;    // yqw 20150918 debug
//		string rule_transform_exp = rule_obj.GetRule_transform_exp();
//
//		CRegexpT <char> regexp(rule_exp.c_str());
//		MatchResult result = regexp.Match(text.c_str());
//
//		while(result.IsMatched())
//		{
//			string transform_exp = "";
//			string::size_type lastPos = 0;
//			string::size_type pos = 0;
//			while((pos=rule_transform_exp.find("$", pos)) != string::npos)
//			{
//				//cout << "Now, pos = " << pos << endl;
//
//				if(rule_transform_exp[pos+1] >= '0' && rule_transform_exp[pos+1] <= '9')
//				{
//					int trans_mat = rule_transform_exp[pos+1]-'0';
//
//					//cout << "trans_mat = " << trans_mat << endl;
//
//					//cout << "(start, end) => (" << result.GetGroupStart(trans_mat) << "," << result.GetGroupEnd(trans_mat) << ")" << endl; 
//					
//					string mat_str = text.substr(result.GetGroupStart(trans_mat), result.GetGroupEnd(trans_mat)-result.GetGroupStart(trans_mat));
//					//cout << "text = " << text << ", mat_str = " << mat_str << endl;
//
//					transform_exp += rule_transform_exp.substr(lastPos, pos-lastPos);
//
//					transform_exp += mat_str;
//
//					lastPos = pos+2;
//
//					pos += 2;
//				}
//
//			}
//
//			transform_exp += rule_transform_exp.substr(lastPos, rule_transform_exp.length()-lastPos);
//
//			//cout << "transform_exp: " << transform_exp << endl;    // yqw 20150918 debug
//
//
//			rule_match_obj.SetStart(result.GetStart());
//			rule_match_obj.SetEnd(result.GetEnd());
//			rule_match_obj.SetRule_id(rule_idx);
//			rule_match_obj.SetRule_transform_str(transform_exp);
//			rule_match_obj.SetIsReady(true);
//
//			//cout << "(start, end, rule_id) ==> (" << rule_match_obj.GetStart() << ", " << rule_match_obj.GetEnd() << ", " << rule_match_obj.GetRule_id() << ")" << endl;    // yqw 20150918 debug
//
//			rule_match_vec.push_back(rule_match_obj);
//			
//			result = regexp.Match(text.c_str(), result.GetEnd());
//
//		}
//		//else
//		//{
//		//	cout << "Not matched!" << endl;
//		//}
//
//	}
//
//	return text;
//
//}


string BasicNSWs::LabelNSWs(string text)
{
	//匹配规则， 标记标签

	//逐条 vector<Rule_Obj> rule_vec; 
	//匹配的规则存储在 vector<Rule_Match_Obj> rule_match_vec;

	string temp_str = text;

	vector<Rule_Obj> rule_vec = BasicNSWs::rule_vec;

	Rule_Match_Obj rule_match_obj;

	for(int rule_idx=0; rule_idx<rule_vec.size(); ++rule_idx)
	{
		Rule_Obj rule_obj = rule_vec.at(rule_idx);

		if(!rule_obj.GetIsUsable())
		{
			continue;
		}
		
		string rule_exp = rule_obj.GetRule_exp();
		//cout << "rule_obj = " << rule_obj.toString() << endl;   // yqw 20150918 debug
		//cout << "rule_exp = " << rule_exp << endl;    // yqw 20150918 debug
		string rule_transform_exp = rule_obj.GetRule_transform_exp();

		CRegexpT <char> regexp(rule_exp.c_str());
		MatchResult result = regexp.Match(text.c_str());

		while(result.IsMatched())
		{
			//20150924 19:11 restrictions verification
			bool isValid = true;
			string rule_restrictions = rule_obj.GetRule_restrictions();
			if(rule_restrictions != "\"\"")
			{
				vector<string> restrict_vec;
				StringUtil::split(restrict_vec, rule_restrictions.substr(1, rule_restrictions.length()-2), "|");
				for(int restrict_vec_idx=0; restrict_vec_idx<restrict_vec.size(); ++restrict_vec_idx)
				{
					string restrict_item_str = restrict_vec.at(restrict_vec_idx);
					vector<string> restrict_item_vec;
					StringUtil::split(restrict_item_vec, restrict_item_str.substr(1, restrict_item_str.length()-2), ",");
					if(restrict_item_vec.size()<4)
					{
						continue;
					}
					if(restrict_item_vec.at(0) == "text")
					{
						//
					}
					else if(restrict_item_vec.at(0) == "num")
					{
						int rest_num_value = 0;
						int rest_num_para = atoi(restrict_item_vec.at(1).c_str());
						string rest_num_para_text = text.substr(result.GetGroupStart(rest_num_para), result.GetGroupEnd(rest_num_para)-result.GetGroupStart(rest_num_para));
						if(restrict_item_vec.at(2) == "value")
						{
							rest_num_value = atoi(rest_num_para_text.c_str());
						}
						else if(restrict_item_vec.at(2) == "length")
						{
							rest_num_value = UTF8Util::StrCharLength(rest_num_para_text.c_str());
						}
						// 

					}
				}
			}

			if(!isValid)
			{
				continue;
			}
			//20150924 19:11 


			string transform_exp = "";
			string::size_type lastPos = 0;

			CRegexpT <char> regexp_transform("\\$([0-9]+)");
			MatchResult result_transform = regexp_transform.Match(rule_transform_exp.c_str());
			while(result_transform.IsMatched())
			{

				int trans_mat = atoi(rule_transform_exp.substr(result_transform.GetGroupStart(1), result_transform.GetGroupEnd(1)-result_transform.GetGroupStart(1)).c_str());

				//cout << "trans_mat = " << trans_mat << endl;
				//cout << "(start, end) => (" << result.GetGroupStart(trans_mat) << "," << result.GetGroupEnd(trans_mat) << ")" << endl; 

				string mat_str = text.substr(result.GetGroupStart(trans_mat), result.GetGroupEnd(trans_mat)-result.GetGroupStart(trans_mat));

				//cout << "text = " << text << ", mat_str = " << mat_str << endl;

				transform_exp += rule_transform_exp.substr(lastPos, result_transform.GetStart()-lastPos);
				transform_exp += mat_str;
				lastPos = result_transform.GetEnd();

				result_transform = regexp_transform.Match(rule_transform_exp.c_str(), lastPos);
			}

			transform_exp += rule_transform_exp.substr(lastPos, rule_transform_exp.length()-lastPos);

			//cout << "transform_exp: " << transform_exp << endl;    // yqw 20150918 debug

			rule_match_obj.SetStart(result.GetStart());
			rule_match_obj.SetEnd(result.GetEnd());
			rule_match_obj.SetRule_id(rule_obj.GetRule_id());	// yqw 20150924 change, 原来为rule_idx
			rule_match_obj.SetRule_transform_str(transform_exp);
			rule_match_obj.SetIsReady(true);

			//cout << "(start, end, rule_id) ==> (" << rule_match_obj.GetStart() << ", " << rule_match_obj.GetEnd() << ", " << rule_match_obj.GetRule_id() << ")" << endl;    // yqw 20150918 debug

			rule_match_vec.push_back(rule_match_obj);
			
			result = regexp.Match(text.c_str(), result.GetEnd());

		}
		//else
		//{
		//	cout << "Not matched!" << endl;
		//}

	}

	return text;

}

int BasicNSWs::Disambiguation()
{
	//int rule_match_idx = 0;
	if(rule_match_vec.size() == 0)
	{
		this->rule_standard_str = text;
		return 0;
	}

	vector<Rule_Match_Obj>::iterator post_iter = rule_match_vec.begin();
	post_iter += 1;
	//while(rule_match_idx < rule_match_vec.size())
	while(post_iter != rule_match_vec.end())
	{
		//Rule_Match_Obj rule_match_obj = rule_match_vec.at(rule_match_idx);
		Rule_Match_Obj rule_match_obj = *post_iter;
		//int rule_match_idx_pre = 0;
		vector<Rule_Match_Obj>::iterator pre_iter = rule_match_vec.begin();
		//while(rule_match_idx_pre < rule_match_idx)
		bool isDel = false;
		while(pre_iter!=post_iter && pre_iter!=rule_match_vec.end())
		{
			//Rule_Match_Obj rule_match_obj_pre = rule_match_vec.at(rule_match_idx_pre);
			Rule_Match_Obj rule_match_obj_pre = *pre_iter;
			if(!rule_match_obj_pre.GetIsReady())	// 无效的匹配
			{
				pre_iter += 1;
				continue;
			}
			// 无交叉
			if(rule_match_obj.GetEnd()<=rule_match_obj_pre.GetStart() || rule_match_obj.GetStart()>=rule_match_obj_pre.GetEnd())
			{
				//rule_match_idx_pre += 1;
				pre_iter += 1;
				continue;
			}

			if(rule_match_obj.GetStart()<=rule_match_obj_pre.GetStart() && rule_match_obj.GetEnd()>=rule_match_obj_pre.GetEnd())
			{
				// 包含前面的， 前面的设置为无效
				if(rule_match_obj.GetStart()!=rule_match_obj_pre.GetStart() || rule_match_obj.GetEnd()!=rule_match_obj_pre.GetEnd())
				{
					//pre_iter = rule_match_vec.erase(pre_iter);
					(*pre_iter).SetIsReady(false);
					pre_iter += 1;
					continue;
				}

			}
			// 存在交叉，后面的为无效
			post_iter = rule_match_vec.erase(post_iter);
			isDel = true;
			break;

			//if(rule_match_obj.GetStart()<=rule_match_obj_pre.GetStart())
			//{
			//	if(rule_match_obj.GetEnd()<rule_match_obj_pre.GetEnd())
			//	{
			//		post_iter = rule_match_vec.erase(post_iter);
			//		break;
			//	}
			//	else
			//	{
			//		//rule_match_vec.erase(rule_match_idx_pre, rule_match_idx_pre+1);
			//		pre_iter = rule_match_vec.erase(pre_iter);
			//		continue;
			//	}
			//}
			//else
			//{
			//	break;
			//}

		}

		if(!isDel)
		{
			post_iter += 1;
		}
		
	}

	//cout << "Ready(start, end, rule_id) => " << endl;
	//for(int idx=0; idx<rule_match_vec.size(); ++idx)
	//{
	//	Rule_Match_Obj rule_match_obj = rule_match_vec.at(idx);
	//	cout << "(" << rule_match_obj.GetStart() << ", " << rule_match_obj.GetEnd() << ", " << rule_match_obj.GetRule_id() << ")" << endl;
	//}

	//cout << "Ready(start, end, rule_id) => " << endl;    // yqw 20150918 debug
	vector<Rule_Match_Obj>::iterator idx_iter = rule_match_vec.begin();
	while(idx_iter != rule_match_vec.end())
	{
		if(!(*idx_iter).GetIsReady())
		{
			//cout << "DEL (" << (*idx_iter).GetStart() << ", " << (*idx_iter).GetEnd() << ", " << (*idx_iter).GetRule_id() << ")" << endl;    // yqw 20150918 debug
			idx_iter = rule_match_vec.erase(idx_iter);
			continue;
		}

		Rule_Match_Obj rule_match_obj = *idx_iter;
		//cout << "(" << rule_match_obj.GetStart() << ", " << rule_match_obj.GetEnd() << ", " << rule_match_obj.GetRule_id() << ")" << endl;    // yqw 20150918 debug
		idx_iter += 1;
	}

	//cout << "After sorted: " << endl;   // yqw 20150918 debug
	sort(rule_match_vec.begin(), rule_match_vec.end(), Rule_Match_Obj::Compare);
	string rule_standard_str_temp = "";
	int temp_idx = 0;
	for(int idx=0; idx<rule_match_vec.size(); ++idx)
	{
		Rule_Match_Obj rule_match_obj = rule_match_vec.at(idx);
		rule_standard_str_temp += this->text.substr(temp_idx, rule_match_obj.GetStart()-temp_idx);
		//cout << "(" << rule_match_obj.GetStart() << ", " << rule_match_obj.GetEnd() << ", " << rule_match_obj.GetRule_id() << ")" << endl;    // yqw 20150918 debug
		rule_standard_str_temp += rule_match_obj.GetRule_transform_str();
		temp_idx = rule_match_obj.GetEnd();

	}

	if(temp_idx < this->text.length())
	{
		rule_standard_str_temp += this->text.substr(temp_idx, this->text.length()-temp_idx);
	}

	//cout << "Now rule_standard_str=" << rule_standard_str_temp << endl;    // yqw 20150918 debug

	this->rule_standard_str = rule_standard_str_temp;

	return 0;

}

void BasicNSWs::ParseFile(string file_path, string output_path)
{

	const int ENTRY_BUFF_SIZE = 8192;
	char buff[ENTRY_BUFF_SIZE];

	cout << file_path << endl;  // yqw 20150918 debug
	FILE* fp = fopen(file_path.c_str(), "r");
	UTF8Util::SkipUtf8Bom(fp);

	FILE* fpw = fopen(output_path.c_str(), "w");

	size_t lineNum = 0;
	while(fgets(buff, ENTRY_BUFF_SIZE, fp))
	{
		const char* inbuff = buff;
		size_t length;

		const char* pbuff = UTF8Util::FindNextInline(inbuff, '\n');
		if(UTF8Util::IsLineEndingOrFileEnding(*pbuff))
		{}

		length = static_cast<size_t>(pbuff-inbuff);
		string key = UTF8Util::FromSubstr(inbuff, length);

		if(key.length() == 0)
		{
			continue;
		}

		//cout << "Normalizing text: " << key << endl;  // yqw 20150918 debug

		string standard_str = this->ParseText(key);

		string norm_str = Norm_ZH::NormText(standard_str);
//		string norm_str_ready = StringUtil::delSeqSpace(norm_str);

		cout << standard_str << endl;
		cout << norm_str << endl;
		
		//fwrite(this->text.c_str(), this->text.length(), 1, fpw);
		//fwrite("\n", 1, 1, fpw);
		//fwrite(standard_str.c_str(), standard_str.length(), 1, fpw);
		//fwrite("\n", 1, 1, fpw);
		fwrite(norm_str.c_str(), norm_str.length(), 1, fpw);
		fwrite("\n", 1, 1, fpw);
	}

	fclose(fp);
	fclose(fpw);

}

void BasicNSWs::TestDeelx()
{
	//char * str1 = "12345";
	//char * str2 = "12345 abcde";

	//printf("'%s' => %s\n", str1, (test_all_number(str1) ? "yes" : "no"));
	//printf("'%s' => %s\n", str2, (test_all_number(str2) ? "yes" : "no"));


	//char * str3 = "bob@smith.com";
	//char * str4 = "bob@.com";

	//printf("'%s' => %s\n", str3, (test_email(str3) ? "yes" : "no"));
	//printf("'%s' => %s\n", str4, (test_email(str4) ? "yes" : "no"));


	//char * code1 = "int a;/* a */";
	//char * code2 = "int a;";

	//int start, end;
	//if(find_remark(code1, start, end))
	//{
	//	printf("In code1, found: %.*s\n", end-start, code1+start);
	//}
	//else
	//{
	//	printf("In code1, not found.\n");
	//}

	//if(find_remark(code2, start, end))
	//{
	//	printf("In code2, found: %s.*s\n", end-start, code2+start);
	//}
	//else
	//{
	//	printf("In code2, not found.\n");
	//}


	//char * text = "12.5, a1.1, 0.123, 178";

	//match_decimal(text);

	char * text = "2015/09/06";

	match_exp(text);

}

int BasicNSWs::test_all_number(const char * str)
{
	// declare, you can use 'static' if your regex does not change
	CRegexpT <char> regexp("\\d+");

	// test
	MatchResult result = regexp.MatchExact(str);

	// matched or not
	return result.IsMatched();
}

int BasicNSWs::test_email(const char * str)
{
	// declare
	CRegexpT <char> regexp("^([0-9a-zA-Z]([-.\\w]*[0-9a-zA-Z])*@(([0-9a-zA-Z])+([-\\w]*[0-9a-zA-Z])*\\.)+[a-zA-Z]{2,9})$");

	// test
	MatchResult result = regexp.MatchExact(str);

	// 
	return result.IsMatched();
}

int BasicNSWs::find_remark(const char * str, int & start, int & end)
{
	// 
	CRegexpT <char> regexp("/\\*((?!\\*/).)*(\\*/)?|//([^\\x0A-\\x0D\\\\]|\\\\.)*");

	//
	MatchResult result = regexp.Match(str);

	//
	if(result.IsMatched())
	{
		start = result.GetStart();
		end = result.GetEnd();
		return 1;
	}
	else
	{
		return 0;
	}

}

int BasicNSWs::match_decimal(const char* str)
{
	//
	CRegexpT <char> regexp("\\b(\\d+)\\.(\\d+)");

	// loop
	MatchResult result = regexp.Match(str);

	while(result.IsMatched())
	{
		printf("%.*s\n", result.GetEnd()-result.GetStart(), str+result.GetStart());

		printf("Matched: %.*s and %.*s\n",result.GetGroupEnd(1)-result.GetGroupStart(1), str+result.GetGroupStart(1), result.GetGroupEnd(2)-result.GetGroupStart(2), str+result.GetGroupStart(2));

		// get next
		result = regexp.Match(str, result.GetEnd());

	}

	// prepare
	CContext * pContext = regexp.PrepareMatch(str);

	// loop
	result = regexp.Match(pContext);

	while(result.IsMatched())
	{
		printf("%.*s\n", result.GetEnd()-result.GetStart(), str+result.GetStart());

		// get next
		result = regexp.Match(pContext);
	}

	// release
	regexp.ReleaseContext(pContext);


	return 0;

}

int BasicNSWs::match_exp(const char* str)
{
	//
	CRegexpT <char> regexp("(?<TMat_year>[\\d]{4})(Hello)/(month)([\\d]+)(?<TMat_month>[\\d]{2})/(?<TMat_day>[\\d]{2})");

	// loop
	MatchResult result = regexp.Match(str);

	int year_num = regexp.GetNamedGroupNumber("TMat_year");
	int month_num = regexp.GetNamedGroupNumber("TMat_month");
	int day_num = regexp.GetNamedGroupNumber("TMat_day");

	cout << "(year, month, day) ==> (" << year_num << ", " << month_num << ", " << day_num << ")" << endl; 

	while(result.IsMatched())
	{
		printf("%.*s\n", result.GetGroupEnd(1)-result.GetGroupStart(1), str+result.GetGroupStart(1));
		printf("%.*s\n", result.GetGroupEnd(2)-result.GetGroupStart(2), str+result.GetGroupStart(2));
		printf("%.*s\n", result.GetGroupEnd(3)-result.GetGroupStart(3), str+result.GetGroupStart(3));

		printf("%.*s\n", result.GetGroupEnd(5)-result.GetGroupStart(5), str+result.GetGroupStart(5));

		result = regexp.Match(str, result.GetEnd());
	}

	return 0;
}


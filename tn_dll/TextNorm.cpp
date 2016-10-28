#include "StdAfx.h"
#include "TextNorm.h"
#include "StringUtil.h"
#include "UTF8Util.h"
#include "Norm_ZH.h"
#include "TN_Engine.h"

map<string, vector<Tran_Obj>> TextNorm::tran_map;


TextNorm::TextNorm(void)
{
	//Init();
}


TextNorm::~TextNorm(void)
{
}

void TextNorm::Init()
{
	//cout<<"TextNorm Init"<<endl;
	load_tran_map();

}

int TextNorm::load_tran_map()
{
	string lang = TN_Engine::getLang();
	//string lang = "zh";
	if(lang=="")
	{
		cout<<"Error, language has not been initialized"<<endl;
		return -1;
	}

	string tran_file_path = "rules/"+lang+".tran";
	if(TN_Engine::getModel_Dir() != "")
	{
		tran_file_path = TN_Engine::getModel_Dir()+"/"+tran_file_path;
	}
	

	const int ENTRY_BUFF_SIZE = 4096;
	char buff[ENTRY_BUFF_SIZE];

	FILE* fp = fopen(tran_file_path.c_str(), "r");
	UTF8Util::SkipUtf8Bom(fp);

	string tran_name;
	vector<Tran_Obj> tran_vec;
	string obj_key;
	string obj_value;
	Tran_Obj tran_obj;
	size_t lineNum = 1;
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
			if(tran_name.length()!=0)
			{
				TextNorm::tran_map.insert(make_pair(tran_name, tran_vec));
				tran_name = "";
				tran_vec.clear();
			}
			continue;
		}

		if(UTF8Util::FromSubstr(key.c_str(), 4) == "####")
		{
			if(tran_name.length()!=0)
			{
				TextNorm::tran_map.insert(make_pair(tran_name, tran_vec));
				tran_name = "";
				tran_vec.clear();
			}
			continue;
		}

		if(UTF8Util::FromSubstr(key.c_str(), 7) == "#define")
		{
			if(tran_name.length()!=0)
			{
				TextNorm::tran_map.insert(make_pair(tran_name, tran_vec));
				tran_name = "";
				tran_vec.clear();
			}

			vector<string> contents;
			StringUtil::split(contents, key, ' ');

			tran_name = contents.at(1);

			//cout<< "tran_name :\t" << tran_name <<endl;

			for(int idx=2; idx<contents.size(); ++idx)
			{
				string inc_name = contents.at(idx);

				//cout << "idx:\t" << idx << endl;

				map<string, vector<Tran_Obj>>::iterator iter = TextNorm::tran_map.find(inc_name);
				if(iter != TextNorm::tran_map.end())
				{
					//cout<<iter->first<<" Begin:"<<endl;
					vector<Tran_Obj> tran_vec_temp = iter->second;
					for(int i=0; i<tran_vec_temp.size(); ++i)
					{
						tran_obj = tran_vec_temp.at(i);
						tran_vec.push_back(tran_obj);
						//cout<<tran_obj.GetKey()<<"\t:\t"<<tran_obj.GetValue()<<endl;
					}
				}
			}

			continue;
		}
		
		vector<string> contents;
		StringUtil::split(contents, key, '\t');
		//cout << "key:\t" << key << endl;
		
		obj_key = contents.at(0);
		obj_value = contents.at(1);
		tran_obj.SetKey(obj_key);
		tran_obj.SetValue(obj_value);
		tran_vec.push_back(tran_obj);
	}


	map<string, vector<Tran_Obj>>::iterator iter = TextNorm::tran_map.begin();
	while(iter != TextNorm::tran_map.end())
	{
		//cout<<iter->first<<" Begin:"<<endl;	// yqw 20150918 debug
		vector<Tran_Obj> tran_vec = iter->second;
		for(int i=0; i<tran_vec.size(); ++i)
		{
			Tran_Obj tran_obj = tran_vec.at(i);
			//cout<<tran_obj.GetKey()<<"\t:\t"<<tran_obj.GetValue()<<endl; // yqw 20150918 debug
		}

		iter ++;
	}

	return 0;

}

string TextNorm::NormText(string text)
{
	//Norm_ZH norm_ZH;
	//return norm_ZH.NormText(text);
	//return norm_ZH.Func_Read_Digit(text, this);
	return "";
}


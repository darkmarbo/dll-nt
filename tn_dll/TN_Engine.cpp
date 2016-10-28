#include "StdAfx.h"
#include "TN_Engine.h"
#include "UTF8Util.h"
#include "StringUtil.h"

string TN_Engine::lang = "zh";	//默认值， zh 中文； 放到配置文件中

string TN_Engine::model_dir = "";

map<string, vector<string>> TN_Engine::conf_map;


TN_Engine::TN_Engine(void)
{
	//Init();
}


TN_Engine::~TN_Engine(void)
{
}

void TN_Engine::setLang(string lang)
{
	TN_Engine::lang = lang;
}



string TN_Engine::getLang()
{
	return TN_Engine::lang;
}

void TN_Engine::setModel_Dir(string model_dir)
{
	TN_Engine::model_dir = model_dir;
}

string TN_Engine::getModel_Dir()
{
	return TN_Engine::model_dir;
}

//初始化，
//设置全局语言，初始化BasicNSWs, AmbiguousNSWs, TextNorm等
int TN_Engine::Init(string model_dir)
{


	
	//cout<<"TN_Engine Init"<<endl;
	TN_Engine::setModel_Dir(model_dir);
	string conf_path = "config/TN_Engine.conf";
	if(model_dir != "")
	{
		conf_path = model_dir+"/"+conf_path;
	}
	FILE* fp = fopen(conf_path.c_str(), "r");
	if (fp == NULL)
	{
		return -1;
	}

	UTF8Util::SkipUtf8Bom(fp);

	const int ENTRY_BUFF_SIZE = 8192;
	char buff[ENTRY_BUFF_SIZE];

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

		if(UTF8Util::FromSubstr(key.c_str(), 4) == "####")
		{
			continue;
		}

		vector<string> contents;
		StringUtil::split(contents, key, '=');
		
		if(contents.size() >= 2)
		{
			if(StringUtil::trim(contents[0])=="lang")
			{
				TN_Engine::setLang(StringUtil::trim(contents[1]));
			}
			if(StringUtil::trim(contents[0])=="model_dir")
			{
				TN_Engine::setModel_Dir(StringUtil::trim(contents[1]));
			}

		}

	}

	fclose(fp);

	//setLang("zh");

	int ret = 0;
	ret = load_conf_map();
	if(ret < 0)
	{
		printf("load_conf_map failed!\n");
		return -2;
	}


	//TextNorm::Init();
	TextNorm textNorm;
	textNorm.Init();

	BasicNSWs basicNSWs;
	basicNSWs.Init();

	

	return 0;

}

//
string TN_Engine::ParseText(string text)
{
	return "";
}

string TN_Engine::NormText(string text)
{
	return Norm_ZH::NormText(text);
}

int TN_Engine::load_conf_map()
{
	string lang = TN_Engine::getLang();
	if(lang=="")
	{
		cout<<"Error, language has not been initialized"<<endl;
		return -1;
	}

	string conf_file_path = "rules/"+lang+".conf";
	if(TN_Engine::getModel_Dir() != "")
	{
		conf_file_path = TN_Engine::getModel_Dir()+"/"+conf_file_path;
	}
		
	
	const int ENTRY_BUFF_SIZE = 4096;
	char buff[ENTRY_BUFF_SIZE];

	FILE* fp = fopen(conf_file_path.c_str(), "r");
	if(fp == NULL)
	{
		printf("open file %s failed!\n",conf_file_path.c_str());
		return -1;
	}
	UTF8Util::SkipUtf8Bom(fp);

	string conf_name;
	vector<string> conf_vec;
	string obj_key;
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
			if(conf_name.length()!=0)
			{
				TN_Engine::conf_map.insert(make_pair(conf_name, conf_vec));
				conf_name = "";
				conf_vec.clear();
			}
			continue;
		}

		if(UTF8Util::FromSubstr(key.c_str(), 4) == "####")
		{
			if(conf_name.length()!=0)
			{
				TN_Engine::conf_map.insert(make_pair(conf_name, conf_vec));
				conf_name = "";
				conf_vec.clear();
			}
			continue;
		}

		if(UTF8Util::FromSubstr(key.c_str(), 7) == "#define")
		{
			if(conf_name.length()!=0)
			{
				TN_Engine::conf_map.insert(make_pair(conf_name, conf_vec));
				conf_name = "";
				conf_vec.clear();
			}

			vector<string> contents;
			StringUtil::split(contents, key, ' ');

			conf_name = contents.at(1);

			//cout<< "conf_name :\t" << conf_name <<endl;

			for(int idx=2; idx<contents.size(); ++idx)
			{
				string inc_name = contents.at(idx);

				//cout << "idx:\t" << idx << endl;

				map<string, vector<string>>::iterator iter = TN_Engine::conf_map.find(inc_name);
				if(iter != TN_Engine::conf_map.end())
				{
					//cout<<iter->first<<" Begin:"<<endl;
					vector<string> conf_vec_temp = iter->second;
					for(int i=0; i<conf_vec_temp.size(); ++i)
					{
						string conf_obj = conf_vec_temp.at(i);
						conf_vec.push_back(conf_obj);
					}
				}
			}

			continue;
		}
		
		vector<string> contents;
		StringUtil::split(contents, key, '\t');
		//cout << "key:\t" << key << endl;
		
		obj_key = contents.at(0);
		conf_vec.push_back(obj_key);
	}


	map<string, vector<string>>::iterator iter = TN_Engine::conf_map.begin();
	while(iter != TN_Engine::conf_map.end())
	{
		//cout << "\n" <<iter->first<<" Begin:"<<endl;	// yqw 20150920 debug
		vector<string> conf_vec = iter->second;
		for(int i=0; i<conf_vec.size(); ++i)
		{
			string conf_obj = conf_vec.at(i);
			//cout<< conf_obj <<endl; // yqw 20150920 debug
		}

		iter ++;
	}

	return 0;

}


void TN_Engine::ProcessFile(string file_path, string output_path)
{}

string TN_Engine::ProcessText(string text)
{
	
	BasicNSWs basicNSWs;
	//basicNSWs.Init();
	printf("start basicNSWs.ParseText(text)\n");
	string standard_str = basicNSWs.ParseText(text);
	printf("basicNSWs.ParseText(text)\n");

	string norm_str = Norm_ZH::NormText(standard_str);
	printf("Norm_ZH::NormText(standard_str)\n");

	//cout << norm_str << endl;
	printf("start return\n");
	return norm_str;
	printf("return ok\n");
}
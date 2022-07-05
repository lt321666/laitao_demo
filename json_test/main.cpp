#include <string>  
#include "./JsonStorage/json/json.h"
#include <iostream>  
#include <fstream>  
using namespace std;

void readStrJson(); //从字符串中读取JSON  
void readStrProJson(); //从字符串中读取JSON（内容复杂些）  
void readFileJson(); //从文件中读取JSON 
void writeFileJson();  //将信息保存为JSON格式  

int main(int argc, char* argv[])
{
	writeFileJson(); //写入json

	readFileJson(); //从文件中读取JSON 

	cout << "\n\n";

	readStrJson();  //从字符串中读json

	cout << "\n\n";

	readStrProJson();//从字符串中读取JSON（内容复杂些）  

	system("pause");
	return 0;
}
void readStrJson()
{
	//字符串  
	const char* str =
		"{\"name\":\"shuiyixin\",\"age\":\"21\",\"sex\":\"man\"}";


	Json::Reader reader;
	Json::Value root;

	//从字符串中读取数据  
	if (reader.parse(str, root))
	{
		string name = root["name"].asString();
		int age = root["nomen"].asInt();
		string sex = root["sex"].asString();
		cout << name + "," << age << "," << sex << endl;
	}

}

void readStrProJson()
{
	string strValue = "{\"name\":\"shuiyixin\",\"major\":[{\"AI\":\"MachineLearning\"},{\"AI\":\"DeepLearning\"},{\"AI\":\"ComputerVision\"}]}";
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(strValue, value))
	{
		string out = value["name"].asString();
		cout << out << endl;
		const Json::Value arrayObj = value["major"];
		for (unsigned int i = 0; i < arrayObj.size(); i++)
		{
			out = arrayObj[i]["AI"].asString();
			cout << out << endl;
		}
	}
}

void readFileJson()
{
	Json::Reader reader;
	Json::Value root;

	//从文件中读取，保证当前文件有demo.json文件  
	ifstream in("demo.json", ios::binary);

	if (!in.is_open())
	{
		cout << "Error opening file\n";
		return;
	}

	if (reader.parse(in, root))
	{
		//读取根节点信息  
		string name = root["name"].asString();
		int age = root["age"].asInt();
		string sex = root["sex"].asString();

		cout << "My name is " << name << endl;
		cout << "I'm " << age << " years old" << endl;
		cout << "I'm a " << sex << endl;

		//读取子节点信息  
		string friend_name = root["friends"]["friend_name"].asString();
		int friend_age = root["friends"]["friend_age"].asInt();
		string friend_sex = root["friends"]["friend_sex"].asString();

		cout << "My friend's name is " << friend_name << endl;
		cout << "My friend's sex is " << friend_sex << endl;
		cout << "My friend is " << friend_age << " years old" << endl;

		//读取数组信息  
		cout << "Here's my hobby:" << endl;
		for (unsigned int i = 0; i < root["hobby"].size(); i++)
		{
			string ach = root["hobby"][i].asString();
			cout << ach << '\t';
		}
		cout << endl;

		cout << "Reading Complete!" << endl;
	}
	else
	{
		cout << "parse error\n" << endl;
	}

	in.close();
}

void writeFileJson()
{
	//根节点  
	Json::Value root;

	//根节点属性  
	root["name"] = Json::Value("shuiyixin");
	root["age"] = Json::Value(21);
	root["sex"] = Json::Value("man");

	//子节点  
	Json::Value friends;

	//子节点属性  
	friends["friend_name"] = Json::Value("ZhaoWuxian");
	friends["friend_age"] = Json::Value(21);
	friends["friend_sex"] = Json::Value("man");

	//子节点挂到根节点上  
	root["friends"] = Json::Value(friends);

	//数组形式  
	root["hobby"].append("sing");
	root["hobby"].append("run");
	root["hobby"].append("Tai Chi");

	//直接输出  
	//cout << "FastWriter:" << endl;
	//Json::FastWriter fw;
	//cout << fw.write(root) << endl << endl;

	//缩进输出  
	cout << "StyledWriter:" << endl;
	Json::StyledWriter sw;
	cout << sw.write(root) << endl << endl;

	//输出到文件  
	ofstream os;
	os.open("demo.json", std::ios::out | std::ios::app);
	if (!os.is_open())
		cout << "error：can not find or create the file which named \" demo.json\"." << endl;
	os << sw.write(root);
	os.close();

}
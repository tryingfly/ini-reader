/*=========== ***doc description @ yyp*** ===========
Copyright (C) 2024 YYP, Shanghai Jiao Tong University, under MIT License.
<https://github.com/tryingfly/ini-reader>
<https://zhuanlan.zhihu.com/p/685164920>

<map>查找不要用value=map[key]，这样会插入不存在的key
=====================================================*/
#include"ini.h"
#include<map>
#include<vector>
#include<iostream>
#include<sstream>
#include<fstream>

namespace Ini{
	//按token分割
	inline void splitString(const string &str, vector<string> &substrings, char token){
		string tmp;
		stringstream ss(str);
		while(getline(ss, tmp, token)){
			substrings.push_back(tmp);
		}
	};
	//去除空格、tab
	inline void wipeWhite(string &str){
		size_t id=0;
		while((id=str.find(' ',id)) != string::npos){
			str.erase(id,1);
		}
		id=0;
		while((id=str.find('\t',id)) != string::npos){
			str.erase(id,1);
		}
	};
//==========================
class iniClass::impClass{
public:
	impClass(){};
	impClass(const string &fileName);
	bool open(const string &fileName,bool append);

	map<string,string> defaultSec;//defaultSec是为了加快访问速度，在store中也存在
	map<string,map<string,string>> store;
	string file;

	const map<string,string>&getSection(const string &section);

	double getVal(const string &secName, const map<string,string> &sec, const string &key);
	const string &getStr(const string &secName, const map<string,string> &sec, const string &key);
	template<typename T,int n>
	void getArray(const string &secName, const map<string,string> &sec, const string &key,T (&value)[n]){getArray(key,value,n);};
	template<typename T>
	void getArray(const string &secName, const map<string,string> &sec, const string &key,T *value,int n);
};
	iniClass::impClass::impClass(const string &fileName){
		open(fileName,0);
	}
	bool iniClass::impClass::open(const string &fileName,bool append){
		ifstream f;
		f.open(fileName);
		if(!f.good()){
			stringstream ss;
			ss<<"=== 未找到"<<fileName<<" !===";
			throw runtime_error(ss.str());
			return 0;
		}
		file=fileName;
		if(!append){
			defaultSec.clear();
			store.clear();
		}
		auto *sec=&defaultSec;
		string secName;
		string line;
		string preKey;
		while(getline(f,line)){
			size_t id=0;
			id=line.find(";");//分号是ini的注释标志
			if(id!=string::npos){
				line=line.substr(0,id);
			}
			wipeWhite(line);
			if(line.size()==0){
				continue;
			}
			if(line.front()=='[' && line.back()==']'){
				secName=line.substr(1,line.length()-2);
				sec=&store[secName];//不存在会自动添加
				preKey.clear();
				// cout<<line.substr(1,line.length()-2)<<endl;
				continue;
			}
			
			id=line.find("=");
			if(id==string::npos){//无“=”号，视为上一行的换行
				if(preKey.size()>0){
					auto &value=(*sec)[preKey];
					if(value[value.size()-1]!=',' && line[0]!=','){
						stringstream ss;
						ss<<"ini：跨行数组拼接，未发现逗号分割符！file="<<fileName<<"，section="<<secName<<"，key="<<preKey;
						cout<<ss.str()<<endl;
						throw runtime_error(ss.str());
					}else{
						value+=line;
					}
				}
			}else{//有“=”号，新键值对
				string key,value;
				key.assign(line,0,id);
				if(sec->find(key) != sec->end()){
					stringstream ss;
					ss<<"ini：重复key！file="<<fileName<<"，section="<<secName<<"，key="<<preKey;
					cout<<ss.str()<<endl;
					throw runtime_error(ss.str());
				}
				value.assign(line,id+1,line.size()-id);
				sec->insert(pair<string,string>(key,value));
				preKey=key;
			}
		}
		return 1;
	}
	const map<string,string>& iniClass::impClass::getSection(const string &section){
		auto it=store.find(section);
		if(it!=store.end()){
			return it->second;
		}else{
			stringstream ss;
			ss<<"ini：未找到section！file="<<file<<"，section="<<section;
			cout<<ss.str()<<endl;
			throw runtime_error(ss.str());
		}
		return store[section];
	}
	double iniClass::impClass::getVal(const string &secName, const map<string,string> &sec,const string &key){
		auto it=sec.find(key);
		if(it!=sec.end()){
			try{
				return stod(it->second);
			}catch(...){
				stringstream ss;
				ss<<"ini:值类型不匹配！file="<<file<<"，section="<<secName<<"，key="<<key;
				cout<<ss.str()<<endl;
				throw runtime_error(ss.str());
			}
		}else{
			stringstream ss;
			ss<<"ini:找不到key！file="<<file<<"，section="<<secName<<"，key="<<key;
			cout<<ss.str()<<endl;
			throw runtime_error(ss.str());
		}
	};
	const string &iniClass::impClass::getStr(const string &secName, const map<string,string> &sec,const string &key){
		auto it=sec.find(key);
		if(it!=sec.end()){
			return it->second;
		}else{
			stringstream ss;
			ss<<"ini:getStr找不到key！file="<<file<<"，section="<<secName<<"，key="<<key;
			cout<<ss.str()<<endl;
			throw runtime_error(ss.str());
		}
	};
	template<typename T>
	void iniClass::impClass::getArray(const string &secName, const map<string,string> &sec,const string &key,T *value,int n){
		auto it=sec.find(key);
		if(it!=sec.end()){
			std::vector<std::string> vs;
			splitString(it->second,vs,',');
			if(vs.size()<n){
				stringstream ss;
				ss<<"ini:getArray数目不匹配！file="<<file<<"，section="<<secName<<"，key="<<key;
				cout<<ss.str()<<endl;
				throw runtime_error(ss.str());
			}
			try{
				for(int i=0;i<n;i++){
					value[i]=(T)stod(vs[i]);
				}
			}catch(...){
				stringstream ss;
				ss<<"ini:getArray值类型不匹配！file="<<file<<"，section="<<secName<<"，key="<<key;
				cout<<ss.str()<<endl;
				throw runtime_error(ss.str());
			}
		}else{
			stringstream ss;
			ss<<"ini:getArray找不到key！file="<<file<<"，section="<<secName<<"，key="<<key;
			cout<<ss.str()<<endl;
			throw runtime_error(ss.str());
		}
	}
	//==无section参数(或section为defaut)==
	iniClass::iniClass():imp(*new impClass()){}
	iniClass::iniClass(const string &fileName):imp(*new impClass(fileName)){}
	bool iniClass::open(const string &fileName,bool append){return imp.open(fileName,append);}
	double iniClass::getVal(const string &key){
		return imp.getVal("默认", imp.defaultSec, key);
	};
	double iniClass::operator[](const string &key){
		return imp.getVal("默认", imp.defaultSec,key);
	};
	const string &iniClass::getStr(const string &key){
		return imp.getStr("默认", imp.defaultSec,key);
	};
	template<typename T>
	void iniClass::getArray(const string &key,T *value,int n){
		imp.getArray("默认", imp.defaultSec, key,value,n);
	}
	template void iniClass::getArray(const string &key,int *value,int n);
	template void iniClass::getArray(const string &key,float *value,int n);
	template void iniClass::getArray(const string &key,double *value,int n);
	//==有section参数========
	double iniClass::getVal(const string &section,const string &key){
		return imp.getVal(section, imp.getSection(section), key);
	};
	const string &iniClass::getStr(const string &section,const string &key){
		return imp.getStr(section, imp.getSection(section), key);
	};
	template<typename T>
	void iniClass::getArray(const string &section,const string &key,T *value,int n){
		imp.getArray(section, imp.getSection(section), key, value, n);
	}
	template void iniClass::getArray(const string &section,const string &key,int *value,int n);
	template void iniClass::getArray(const string &section,const string &key,float *value,int n);
	template void iniClass::getArray(const string &section,const string &key,double *value,int n);
}//namespace


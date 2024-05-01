/*=========== ***doc description @ yyp*** ===========
Copyright (C) 2024 YYP, Shanghai Jiao Tong University, under MIT License.
<https://github.com/tryingfly/ini-reader>
<https://zhuanlan.zhihu.com/p/685164920>

=====================================================*/
#include"ini.h"
#include<map>
#include<vector>
#include<iostream>
#include<sstream>
#include<fstream>

namespace Ini{
	//按token分割
	inline void splitString(string &str, vector<string> &substrings, char token){
		string tmp;
		stringstream ss(str);
		while(getline(ss, tmp, token)){
			substrings.push_back(tmp);
		}
	}
	//去除空格、tab
	void wipeWhite(string &str){
		size_t id=0;
		while((id=str.find(' ',id)) != string::npos){
			str.erase(id,1);
		}
		id=0;
		while((id=str.find('\t',id)) != string::npos){
			str.erase(id,1);
		}
	}
//==========================
class iniClass::impClass{
public:
	impClass();
	impClass(const string &fileName);
	ifstream f;
	map<string,string>mp;
	
	void init();
	double operator[](const string &key);
	const string &getStr(const string &key);

	template<typename T,int n>
	void getArray(const string &key,T (&value)[n]){getArray(key,value,n);};
	template<typename T>
	void getArray(const string &key,T *value,int n);
};
	iniClass::impClass::impClass(){
		#ifdef _WIN32
		f.open("../../000.ini");
		if(f.is_open()){fout.open("../../zzz.txt", ios::trunc);}
		#else
		f.open("../000.ini");
		if(f.is_open()){fout.open("../zzz.txt", ios::trunc);}
		#endif
		if(!f.is_open()){
			f.open("000.ini");
			fout.open("zzz.txt", ios::trunc);
		}
		init();
	}
	iniClass::impClass::impClass(const string &fileName){
		f.open(fileName);
		init();
	}
	void iniClass::impClass::init(){
		string line;
		string preKey;
		while(getline(f,line)){
			size_t id=0;
			//分号是ini的注释标志
			id=line.find(";");
			if(id!=string::npos){
				line=line.substr(0,id);
			}
			wipeWhite(line);
			if(line.size()==0){
				continue;
			}
			id=line.find("=");
			if(id==string::npos){//无“=”号，视为上一行的换行
				if(preKey.size()>0){
					auto &value=mp[preKey];
					if(value[value.size()-1]!=',' && line[0]!=','){
						stringstream ss;
						ss<<"ini：跨行数组拼接，未发现逗号分割符！key="<<preKey;
						cout<<ss.str()<<endl;
						throw runtime_error(ss.str());
					}else{
						value+=line;
					}
				}
			}else{//有“=”号，新键值对
				string key,value;
				key.assign(line,0,id);
				value.assign(line,id+1,line.size()-id);
				mp.insert(pair<string,string>(key,value));
				preKey=key;
			}
		}
	}
	double iniClass::impClass::operator[](const string &key){
		auto it=mp.find(key);
		if(it!=mp.end()){
			try{
				return stod(it->second);
			}catch(...){
				stringstream ss;
				ss<<"\" ini[\""<<key<<"\"] \" 值类型不匹配";
				cout<<ss.str()<<endl;
				throw runtime_error(ss.str());
			}
		}else{
			stringstream ss;
			ss<<"\" ini[\""<<key<<"\"] \" 找不到key";
			cout<<ss.str()<<endl;
			throw runtime_error(ss.str());
		}


	};
	const string &iniClass::impClass::getStr(const string &key){
		auto it=mp.find(key);
		if(it!=mp.end()){
			return it->second;
		}else{
			stringstream ss;
			ss<<"\" ini[\""<<key<<"\"] \" 找不到key";
			cout<<ss.str()<<endl;
			throw runtime_error(ss.str());
		}
	};
	template<typename T>
	void iniClass::impClass::getArray(const string &key,T *value,int n){
		auto it=mp.find(key);
		if(it!=mp.end()){
			std::vector<std::string> vs;
			splitString(it->second,vs,',');
			if(vs.size()<n){
				stringstream ss;
				ss<<"\"ini.getArray\" 数目不匹配，key="<<key;
				cout<<ss.str()<<endl;
				throw runtime_error(ss.str());
			}
			try{
				for(int i=0;i<n;i++){
					value[i]=(T)stod(vs[i]);
				}
			}catch(...){
				stringstream ss;
				ss<<"\"ini.getArray\" 值类型不匹配，key="<<key;
				cout<<ss.str()<<endl;
				throw runtime_error(ss.str());
			}
		}else{
			stringstream ss;
			ss<<"\"ini.getArray\" 找不到key="<<key;
			cout<<ss.str()<<endl;
			throw runtime_error(ss.str());
		}
	}
//==========================
	iniClass::iniClass():imp(*new impClass()){}
	iniClass::iniClass(const string &fileName):imp(*new impClass(fileName)){}
	double iniClass::operator[](const string &key){
		return imp[key];
	};
	const string &iniClass::getStr(const string &key){
		return imp.getStr(key);
	};
	template<typename T>
	void iniClass::getArray(const string &key,T *value,int n){
		imp.getArray(key,value,n);
	}
	template void iniClass::getArray(const string &key,int *value,int n);
	template void iniClass::getArray(const string &key,float *value,int n);
	template void iniClass::getArray(const string &key,double *value,int n);
}//namespace

Ini::iniClass ini;//默认打开“000.ini”

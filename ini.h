/*=========== ***doc description @ yyp*** ===========
Copyright (C) 2024 YYP, Shanghai Jiao Tong University, under MIT License.
<https://github.com/tryingfly/ini-reader>
<https://zhuanlan.zhihu.com/p/685164920>

读取解析配置文件*.ini
不要在实时循环使用，可以在进实时循环前先找变量赋值

#include"ini.h"
=====================================================*/
#pragma once
#include<string>
using namespace std;

namespace Ini{
//不要在实时循环使用
class iniClass{
public:
	iniClass();
	iniClass(const string &fileName);//等价于无参构造+open，
	bool open(const string &fileName, bool append=0);//append=1用于打开文件并在原有基础上键值对

	//--不存在section的独立数据获取--
	double getVal(const string &key);       //不要在实时循环使用
	double operator[](const string &key);   //不要在实时循环使用
	const string &getStr(const string &key);//不要在实时循环使用
	template<typename T>
	void getArray(const string &key, T*value, int n); //不要在实时循环使用
	template<typename T,int n>
	void getArray(const string &key, T(&value)[n]){getArray(key,value,n);};//不要在实时循环使用

	//--有section参数--
	double getVal(const string &section, const string &key);       //不要在实时循环使用
	const string &getStr(const string &section, const string &key);//不要在实时循环使用
	template<typename T>
	void getArray(const string &section, const string &key, T*value, int n);//不要在实时循环使用
	template<typename T,int n>
	void getArray(const string &section, const string &key, T(&value)[n]){getArray(key,value,n);};//不要在实时循环使用
protected:
	class impClass;
	impClass&imp;
};

}//namespace

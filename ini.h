/*=========== ***doc description @ yyp*** ===========
Copyright (C) 2024 YYP, Shanghai Jiao Tong University, under MIT License.
<https://github.com/tryingfly/ini-reader>
<https://zhuanlan.zhihu.com/p/685164920>

读取解析配置文件*.ini
不要在实时循环使用，可以在进实时循环前先找变量赋值
不能在全局变量使用
可以在类构造函数中使用(main之前执行)，但该类不能实例化全局变量
#include"ini.h"
=====================================================*/
#pragma once
#include<string>
using namespace std;

namespace Ini{
//不要在实时循环使用
class iniClass{
public:
	iniClass();//不要在实时循环使用
	iniClass(const string &fileName);//不要在实时循环使用
	double operator[](const string &key);//不要在实时循环使用
	const string &getStr(const string &key);//不要在实时循环使用
	template<typename T,int n>
	void getArray(const string &key,T (&value)[n]){getArray(key,value,n);};//不要在实时循环使用
	template<typename T>
	void getArray(const string &key,T *value,int n);//不要在实时循环使用
private:
	class impClass;
	impClass&imp;
};
}//namespace

extern Ini::iniClass ini;//已实例化，默认打开“000.ini”

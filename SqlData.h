#ifndef SQL_H
#define SQL_H
#include "list"
#include <variant>
#include <string>
#include <unordered_map>
#include <iostream>
// aliases for  simplicity
typedef std::string TEXT;
typedef int INTEGER;
typedef float REAL;
template <typename F,typename G>
using dict = std::unordered_map<F,G>;
using SQL_TYPE = std::variant<int, float, std::string, bool>;
// func to get the string version of sqldata type
std::string ToString(SQL_TYPE data);

class Dict{
	private:
	dict<std::string,SQL_TYPE> values;
	public:
	Dict(initializer_list<std::string> parameters,initializer_list<SQL_TYPE> values);
	//Dict(initializer_list<std::string> parameters,initializer_list<std::string> types);
	void set(dict<std::string,SQL_TYPE>);
	SQL_TYPE get(std::string);
	template <typename T>
	const T get(std::string) const; //getter
	SQL_TYPE& operator[](std::string); //setter
};
struct dataClass{
	list<std::string> parameters;
	dict<std::string,std::string> types;
	Dict values;};
class SQLdata{
	private : 
		dict<std::string,dataClass> Data;
		list<std::string> classes;
	public:
		SQLdata(std::string);
		SQL_TYPE& Get(std::string,std::string);
#include "SqlData.cpp"
#endif	
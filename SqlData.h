#ifndef SQL_H
#define SQL_H
#include "list"
#include <variant>
#include <string>
#include <unordered_map>
#include <iostream>
#include <sqlite3.h>
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
	Dict() = default;
	Dict(list<std::string> parameters,list<SQL_TYPE> values);
	void set(dict<std::string,SQL_TYPE>);
	SQL_TYPE get(std::string);
	template <typename T>
	const T get(std::string parameter) const; //getter , gives error for non-existing keys
	SQL_TYPE& operator[](std::string parameter); //setter for existing key
	void set(std::string parameter,SQL_TYPE value);//setter for non-existing key
};
struct dataClass{
	list<std::string> parameters;
	dict<std::string,std::string> types;
	Dict values;
};
class SQLdata{
	private : 
		sqlite3* DB;
		dict<std::string,dataClass> Data;
		list<std::string> classes;
		string query, sql;
		string lastClass,currentClass;
		dict<string,SQL_TYPE> lastGet;
		static int callback(void*,int, char**, char**);
		void execute();
		void createQuery(string,string);
	public:
		SQLdata(std::string);//make a .db file in folder
		void Create(std::string className,initializer_list<std::string> parameters,initializer_list<std::string> types);
		//makes a table of that name
		void Add(std::string className,initializer_list<SQL_TYPE> values);// Inserts a row
		Dict Get_Row(std::string className,std::string parameter,SQL_TYPE value);//Gets back a row on the condition
		list<SQL_TYPE> Get_Column(std::string className,std::string parameter);
		SQL_TYPE Get_Cell(std::string className,std::string column,std::string parameter,SQL_TYPE value);
		void Update(string className,
		initializer_list<string> parametersTochange,initializer_list<SQL_TYPE> valuesToput,
		string parameter,SQL_TYPE value); //keep parameter and value "" and 0 to update all the rows
		void Delete(string className,string parameter,SQL_TYPE value);//deletes a row on condition
		void Destroy(std::string className);//deletes a whole table
};
#include "SqlData.cpp"
#endif	
#include "list"
#include <variant>
#include <sqlite3.h>
#include <stdexcept>
// Overload + for two SQL_TYPEs

std::string toLower(std::string& str){
	std::string s = str;
	for(char& c : s){
		c = tolower(c);
	}
	return s;
}

std::string toUpper(std::string& str){
	std::string s = str;
	for(char& c : s){
		c = toupper(c);
	}
	return s;
}


inline SQL_TYPE operator+(const SQL_TYPE& a, const SQL_TYPE& b) {
    return std::visit([](auto&& arg1, auto&& arg2) -> SQL_TYPE {
        using T1 = std::decay_t<decltype(arg1)>;
        using T2 = std::decay_t<decltype(arg2)>;

        // Only allow math if both are numeric (int or float)
        if constexpr ((std::is_arithmetic_v<T1>) && (std::is_arithmetic_v<T2>)) {
            return arg1 + arg2;
        }
        // If they are strings, maybe concatenate them?
        else if constexpr (std::is_same_v<T1, std::string> && std::is_same_v<T2, std::string>) {
            return arg1 + arg2;
        }
        return 0; // Default or error
    }, a, b);
}
inline SQL_TYPE operator+(const SQL_TYPE& a, int b) {
    if (std::holds_alternative<int>(a)) {
        return std::get<int>(a) + b;
    } else if (std::holds_alternative<float>(a)) {
        return std::get<float>(a) + (float)b;
    }
    return a; // Return unchanged if not numeric
}
inline SQL_TYPE operator+(const SQL_TYPE& a, float b) {
    if (std::holds_alternative<int>(a)) {
        return std::get<int>(a) + b;
    } else if (std::holds_alternative<float>(a)) {
        return std::get<float>(a) + (float)b;
    }
    return a; // Return unchanged if not numeric
}

std::string ToString(SQL_TYPE data) {
    return std::visit([](auto&& arg) -> std::string {
        // This 'auto' handles whatever type is inside
        // We use std::to_string for numbers, or return the string directly
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            return "'" + arg + "'"; 
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else {
            return std::to_string(arg);
        }
    }, data);
}
void Dict::set(dict<std::string,SQL_TYPE> v){
	values = v;
}
SQL_TYPE Dict::get(std::string parameter){
	try{
		return values.at(parameter);}
	catch (const std::out_of_range& e) {
        std::cerr << "SQL Error: Parameter '" << parameter << "' not found in table." << std::endl;
        exit(1); // Stops the program immediately
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        exit(1);
    }
 }
 // getter
template <typename T>
const T Dict::get(std::string parameter) const{
	try{
		return std::get<T>(values.at(parameter));
		}
	catch (const std::out_of_range& e) {
        std::cerr << "SQL Error: Parameter '" << parameter << "' not found in table." << std::endl;
        exit(1); 
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        exit(1);
    }
}
// setter
SQL_TYPE& Dict::operator[](std::string parameter){
	try{
		return values.at(parameter);
		}
	catch (const std::out_of_range& e) {
        std::cerr << "SQL Error: Parameter '" << parameter << "' not found in table." << std::endl;
        exit(1); 
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        exit(1);
    }
}
void Dict::set(string parameter, SQL_TYPE value){
	values[parameter] = value;
}
Dict::Dict(list<std::string> parameters,list<SQL_TYPE> valuesList){
			for(int i=0;i<parameters.len();i++){
				std::string par = parameters[i];
				values[par] = valuesList[i];
			}
	}
	
int SQLdata::callback(void* dataPtr, int argc, char** argv, char** azColName) {
    SQLdata* instance = static_cast<SQLdata*>(dataPtr);
    for (int i = 0; i < argc; i++) {
        instance->Data[instance->currentClass].values[azColName[i]] = (argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

void SQLdata::execute() {
    char* zErrMsg = 0;
    // We pass 'callback' as the 3rd argument so SQL can talk to C++
    int rc = sqlite3_exec(DB, sql.c_str(), callback, this, &zErrMsg);
    query = "";
    if (rc != SQLITE_OK) {
        cerr << "SQL Error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
}
void SQLdata::createQuery(std::string func,std::string columns="*"){
	if(func=="create"){
		sql = "CREATE TABLE IF NOT EXISTS "+currentClass+" ( "+query +" );";}
	else if(func=="insert"){
		sql = "INSERT INTO "+currentClass+" VALUES( "+query+" );";}
	else if(func=="get"){
		sql = "SELECT " + columns + " FROM "+currentClass+" "+query+";";}
	else if(func=="update"){
		sql = "UPDATE " + currentClass + " SET " + query + ";";}
	else if(func=="delete"){
		sql = "DELETE " + currentClass + " WHERE " + query +";";}
	else{
		sql = "DROP TABLE IF EXISTS "+currentClass+";";}
	execute();
}
SQLdata::SQLdata(string name){
	string s = name + ".db";
	int exit = sqlite3_open(s.c_str(), &DB); }
void SQLdata::Create(std::string className,
initializer_list<std::string> parameters,
initializer_list<std::string> types){
		currentClass = className;
		classes.append(className);
		list p=parameters;list t=types;
		list<SQL_TYPE> v;
		dict<string,string> ty;
		for(int i=0;i<t.len();i++){
			string type = toUpper(t[i]);
			query += (i==0? "" : ", ") + p[i] + " "+type;
			ty[p[i]] = type;
			if(type=="INT" or type=="INTEGER")  v.append(0);
			else if(type=="REAL" or type=="FLOAT")  v.append(0.0f);
			else if(type=="BOOL")  v.append(false);
			else v.append("");
		}
		Data.emplace(className, dataClass{p,ty,Dict(p,v)});
		p.clear();t.clear();v.clear();
		createQuery("create");
}
void SQLdata::Add(std::string className,initializer_list<SQL_TYPE> values){
	currentClass = className;
	list v = values;
	for(int i=0;i<v.len();i++){
		query += (i==0? "" : ", ") + ToString(v[i]);
	}
	v.clear();
	createQuery("insert");
}
Dict SQLdata::Get_Row(std::string className,std::string parameter,SQL_TYPE value){
	currentClass = className;
	dict<string,SQL_TYPE> d={{parameter,value}};
	if(currentClass != lastClass and lastGet != d){
		query =" WHERE " + parameter + " = " + ToString(value);
	}
	lastClass = currentClass;
	lastGet = d;
	createQuery("get");
	return Data[currentClass].values;
}
Dict SQLdata::Get_Column(std::string className,std::string parameter){
	currentClass = className;
	query = "";
	createQuery("get",parameter);
	return Data[currentClass].values;
}
SQL_TYPE SQLdata::Get_Cell(std::string className,std::string column,std::string parameter,SQL_TYPE value){
	currentClass = className;
	query = " WHERE " + parameter " = " + ToString(value);
	createQuery("get",column);
	return Data[currentClass].values.get(parameter);
}

void SQLdata::Update(string className,
		initializer_list<string> parametersTochange,initializer_list<SQL_TYPE> valuesToput,
		string parameter,SQL_TYPE value){
		currentClass = className;
		list p=parametersTochange;list v = valuesToput;
		for(int i=0;i<v.len();i++){
			query +=  (i==0? "" : ", ") + p[i] + " = " + ToString(v[i]);
		}
		if(parameter !=""){
			query += " WHERE " + parameter +" = " + ToString(value);}
		p.clear();v.clear();
		createQuery("update");
}
void SQLdata::Delete(string className,string parameter,SQL_TYPE value){
	currentClass = className;
	query = parameter + " = " + ToString(value);
	createQuery("delete");
}
void SQLdata::Destroy(std::string className){
	currentClass = className;
	createQuery("destroy");
}
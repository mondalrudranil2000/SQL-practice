#include "list"
#include <variant>
#include <stdexcept>
// Overload + for two SQL_TYPEs
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
            return arg; 
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
Dict::Dict(
		initializer_list<std::string> parameters,initializer_list<SQL_TYPE> valuesList){
			list p=parameters;list v=valuesList;
			for(int i=0;i<p.len();i++){
				std::string par = p[i];
				values[par] = v[i];
			}
		p.clear();v.clear();
	}
/*
Dict::Dict(initializer_list<std::string> parameters,initializer_list<std::string> typesList){
		list p=parameters;
		list t = typesList;
		for(int i=0;i<p.len();i++){
			std::string par = p[i];
			std::string type = t[i];
			if(type=="string" or type=="text") values[par]="";
			else if(type=="int" or type=="integer") values[par]=0;
			else if(type=="float" or type=="real") values[par]=0.0f;
			else if(type=="bool") values[par]=false;
			else values[par] = "NULL";
		}
		p.clear();t.clear();
	}
*/
#ifndef MYJSON_H_
#define MYJSON_H_
#include<vector>
#include<map>
#include<cstdio>
#include<cstring>
#include<string>
#include<exception>


#define NUM_DIGITS 10 //JSON_NUM类型，数字的有效数字位数

typedef double NUM;


typedef enum json_e {
    JSON_NONE,
    JSON_BOL,
    JSON_STR,
    JSON_ARR,
    JSON_OBJ,
    JSON_NUM,
} json_e;

typedef struct ptrCmp
{
	bool operator()( const char * s1, const char * s2 ) const
	{
		return strcmp( s1, s2 ) < 0;
	}
}ptrCmp;


class JSON{
private:
	json_e type;
	union{
		NUM num;
		bool bol;
		char* str;
		std::vector<JSON*> arr;
	};
	std::map<const char*, JSON*, ptrCmp> obj;


	char* err_info;						//错误信息

	/* 
	* 记录错误信息, 保存在err_info中。
	*/
	void note_err_info(const char*);

	/*
	* to_str函数，将各种类型的JSON对象转换成char*返回
	 */
	char* none_to_str();
	char* num_to_str();
	char* bol_to_str();
	char* str_to_str();
	char* arr_to_str();
	char* obj_to_str();


public:
	JSON();
	/*
	* 使用类型构建
	 */
	JSON(json_e type_name);
	/*
	* 使用 num、bool、cahr*或string构建，会设置其对应类型值。
	 */			
	JSON(NUM value);
	JSON(const char* value);
	JSON(std::string value);
	JSON(bool value);
	/*
	* 拷贝构造
	 */
	JSON(const JSON & json);
	/*
	* 析构
	 */
	~JSON();
	/*
	* 返回JSON类型
	 */
	json_e get_type();

	template<typename T>
	T get_value();						// NUM、BOL、STR类型，获取值
	JSON* get_from_key(const char* key);// OBJ类型，获取键key对应的值
	JSON* get_from_arr(int index);		// ARR类型，获取index位置的值

	// void set_type(json_e type_e);			// 设置类型值 

	/* 
	* INT、BOL、STR类型，设置值, 成功返回0
	*/
	int set_value(bool value);				
	int set_value(NUM value);
	int set_value(const char* value);
	int set_value(std::string value);
	/*
	* OBJ类型，新增一个键值对, 成功返回0
	 */
	int obj_add(const char* key, JSON* value);
	/*
	* ARR类型，在index位置新增一个JSON对象，index为-1表示在最后插入, 成功返回0
	 */
	int arr_add(int index, JSON* value);
	
	/*
	* 格式化为JSON字符串
	*/
	char* to_str();						

	void obj_rm(const char* key); 		// OBJ类型，删除一个键值对
	void arr_rm(int index);				// ARR类型，删除下标为index的JSON对象

	void free();

	/* 
	* 获取错误信息, 返回err_info的指针。
	*/
	char* get_err_info_ptr();
};


#endif

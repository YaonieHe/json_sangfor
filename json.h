#ifndef MYJSON_H_
#define MYJSON_H_
#include<vector>
#include<map>
#include<cstdio>
#include<cstring>
#include<string>
#include<exception>
#include<cassert>

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
	};
	/*
	* ARR类型，记录数据。
	* 空间有浪费，可以考虑自行实现一个类，实现vector部分功能，放入union中
	*/
	std::vector<JSON*> arr;
	/*
	* OBJ类型保存键值对
	*/
	std::map<const char* const, JSON*, ptrCmp> obj;

	/*
	* 保存错误信息
	*/
	char* err_info;						

	/* 
	* 记录错误信息, 保存在err_info中。
	*/
	void note_err_info(const char*);

	/*
	* to_str函数，将各种类型的JSON对象转换成char*返回
	* need为-1时不加tab，否则加tab，need表示tab个数
	* @param mode: 缩进模式
	* 对于ARR和OBJ，对象之间换行，末尾不换行
	 */
	std::string none_to_str(int need, int mode = 1);
	std::string num_to_str(int need, int mode = 1);
	std::string bol_to_str(int need, int mode = 1);
	std::string str_to_str(int need, int mode = 1);
	std::string arr_to_str(int need, int mode = 1);
	std::string obj_to_str(int need, int mode =1);
	/*
	* 选择是否需要换行和加tab。
	* need为-1时不换行且无tab，否则换行加tab，need表示tab个数
	 */
	std::string to_str_select_tab(int need = -1);

	/*
	* @param mode: 一个整数，表示缩进模式：
    *   Mode = 1， 填充tab
    *   Mode = 2，填充2空格
    *   Mode = 3，填充1空格，最后一个2空格的第1位置用短线取代
    *   Mode = 4, 填充4空格
    *   Mode = 5，填充4空格，最后一个四空格的第三位置用短线取代
	* @param num: 一个整数，表示填充个数
	* @return: 一个char*，表示返回的字符串，错误则返回NULL
	 */
	std::string pre_place(int num, int mode);
	/*
	* 转为yaml格式的字符串
	* @param need: 表示需要的缩进层数 
	* @param mode: 表示缩进模式 
	 */
	std::string none_to_yaml(int need, int mode = 4);
	std::string obj_to_yaml(int need, int mode = 4);
	std::string arr_to_yaml(int need, int mode = 4);

	std::string to_yaml_select_tab(int need = -1);

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
	JSON(const std::string& value);
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

	/*
	* ARR类型和OBJ类型返回包含对象数量
	* NONE类型返回0
	* 其他类型返回1
	 */
	int size();
	/*
	* NUM、BOL、STR类型，获取值
	* 类型不匹配扔出异常
	*/
	NUM get_from_num();
	bool get_from_bol();
	std::string get_from_str();	

	/* 
	* OBJ类型，获取键key对应的值，不成功返回NULL	
	*/
	JSON* get_from_key(const char* key);
	/* 
	* ARR类型，获取index位置的值，index从0开始
	* 为-1表示查询最后一个
	* 超出范围或其他原因不成功返回NULL
	*/
	JSON* get_from_arr(int index);		

	/* 
	* INT、BOL、STR类型，设置值, 成功返回0
	*/
	int set_value(bool value);				
	int set_value(NUM value);
	int set_value(const char* value);
	int set_value(const std::string& value);
	/*
	* OBJ类型，新增一个键值对, 成功返回0
	 */
	int obj_add(const char* key, JSON* value);
	/*
	* ARR类型，在index位置新增一个JSON对象，index为-1表示在最后插入, 成功返回0
	 */
	int arr_add(int index, JSON* value);
	
	/*
	* 格式化为JSON字符串，不换行
	*/
	std::string to_json_str_without_tab();
	/*
	* 格式化为JSON字符串，换行，有缩进
	*/
	std::string to_json_str_with_tab();		
	/*
	* 格式化为YAML字符串
	*/
	std::string to_yaml_str();			

	/*
	* OBJ类型，删除一个键值对,成功返回0
	 */
	int obj_rm(const char* key); 		
	/* 
	* ARR类型，删除下标为index的JSON对象，成功返回0
	*/
	int arr_rm(int index);				

	/*
	* 清空 
	*/
	void free();

	/* 
	* 获取错误信息, 返回err_info的指针。
	*/
	std::string get_err_info();
};


#endif

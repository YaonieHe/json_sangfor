#include"../include/json.h"

JSON::JSON(){
	type = json_e::JSON_NONE;
	err_info = NULL;
}

JSON::JSON(json_e type_name){
	this->type = type_name;
	err_info = NULL;
	switch(type_name){
		case json_e::JSON_STR:{
			this->str = NULL;
			break;
		}
		case json_e::JSON_NUM:{
			this->num = 0;
			break;
		}
		case json_e::JSON_BOL:{
			this->bol = true;
			break;
		}
	}
}

JSON::JSON(NUM value){
	err_info = NULL;
	this->num = value;
	this->type = json_e::JSON_NUM;
}

JSON::JSON(bool value){
	err_info = NULL;
	this->bol = value;
	this->type = json_e::JSON_BOL;
}

JSON::JSON(const std::string& value){
	err_info = NULL;
	this->type = json_e::JSON_STR;
	this->str = new char[value.size() + 1];
	if(this->str == NULL){
		throw "error";
	}
	strcpy(this->str, value.c_str());
}

JSON::JSON(const char* value){
	err_info = NULL;
	this->type = json_e::JSON_STR;
	this->str = new char[strlen(value) + 1];
	strcpy(this->str, value);
}

JSON::JSON(const JSON & json){
	this->type = json.type;
	this->err_info = NULL;
	switch(json.type){
		case json_e::JSON_NUM:{ 
			this->num = json.num;
			break;
		}
		case json_e::JSON_BOL:{
			this->bol = json.bol;
			break;
		}
		case json_e::JSON_STR:{
			this->str = new char[strlen(json.str) + 1];
			strcpy(this->str, json.str);
			break;
		}
		case json_e::JSON_ARR:{
			int arr_len = json.arr.size();
			for(int i = 0; i < arr_len; i++){
				JSON* tmp = new JSON(*(json.arr[i]));
				this->arr.push_back(tmp);
			}
			break;
		}
		case json_e::JSON_OBJ:{
			for(auto it = json.obj.begin(); it != json.obj.end(); it++){
				char* name = new char[strlen(it->first) + 1];
				strcpy(name, it->first);
				JSON* value = new JSON(*(it->second));
				this->obj[name] = value;
			}
			break;
		}
	}
}

JSON::~JSON(){
	this->free();
}

json_e JSON::get_type(){
	return this->type;
}

NUM JSON::get_from_num(){
	if(this->type == json_e::JSON_NUM){
		return this->num;
	}
	note_err_info("该类型JSON对象不能调用获取NUM值的方法！\n");
	throw "error";
}

bool JSON::get_from_bol(){
	if(this->type == json_e::JSON_BOL){
		return this->bol;
	}
	note_err_info("该类型JSON对象不能调用获取BOL值的方法！\n");
	throw "error";
}

std::string JSON::get_from_str(){
	if(this->type == json_e::JSON_STR){
		// char* tmp = new char[strlen(this->str) + 1];
		// strcpy(tmp, this->str);
		return this->str;
	}
	note_err_info("该类型JSON对象不能调用获取STR值的方法！\n");
	throw "error";
}

JSON* JSON::get_from_key(const char* key){
	if(this->type == json_e::JSON_OBJ){
		if(this->obj.find(key) != this->obj.end()){
			JSON* tmp = new JSON(*(this->obj[key]));
			return tmp;
		}
		note_err_info("该OBJ对象中所查询的键不存在！\n");
		return nullptr;
	}
	note_err_info("该类型JSON对象不能调用通过键取值的方法！\n");
	return nullptr;
}

JSON* JSON::get_from_arr(int index){
	if(this->type == json_e::JSON_ARR){
		int arr_len = this->arr.size();
		int idx = -1;
		if(index == -1 && arr_len > 0){
			idx = arr_len - 1;
		}
		else if(index >= 0 && index < arr_len){
			idx = index;
		}
		if(idx != -1){
			JSON* tmp = new JSON(*(this->arr[idx]));
			return tmp;
		}
		note_err_info("输入下标超过该ARR对象最大元素数量！\n");
		return nullptr;
	}
	note_err_info("该类型JSON对象不能调用通过下标取值的方法！\n");
	return nullptr;
}

// INT、BOL、STR类型，设置值, 返回0表示执行正确。
int JSON::set_value(bool value){
	if(this->type != json_e::JSON_NONE && this->type != json_e::JSON_BOL){
		note_err_info("该JSON对象不允许设置为bool值！\n");
		return -1;
	}
	this->type = json_e::JSON_BOL;
	this->bol = value;
	return 0;
}

int JSON::set_value(NUM value){
	if(this->type != json_e::JSON_NONE && this->type != json_e::JSON_NUM){
		note_err_info("该JSON对象不允许设置为NUM值！\n");
		return -1;
	}
	this->type = json_e::JSON_NUM;
	this->num = value;
	return 0;
}

int JSON::set_value(const char* value){
	if(this->type != json_e::JSON_NONE && this->type != json_e::JSON_STR){
		note_err_info("该JSON对象不允许设置为STRING值！\n");
		return -1;
	}
	char* tmp = new char[strlen(value) + 1];
	if(tmp == NULL){
		note_err_info("[set_value(char*)] 内存申请失败！\n");
		return -1;
	}
	strcpy(tmp, value);
	free();
	this->type = json_e::JSON_STR;
	this->str = tmp;
	return 0;
}

int JSON::set_value(const std::string& value){
	if(this->type != json_e::JSON_NONE && this->type != json_e::JSON_STR){
		note_err_info("该JSON对象不允许设置为NUM值！\n");
		return -1;
	}
	char* tmp = new char[value.size() + 1];
	if(tmp == NULL){
		note_err_info("[set_value(string)] 内存申请失败！\n");
		return -1;
	}
	strcpy(tmp, value.c_str());
	free();
	this->type = json_e::JSON_STR;
	this->str = tmp;
	return 0;
}

int JSON::obj_add(const char* key, JSON* value){
	assert(key != NULL && value != NULL);
	if(this->type != json_e::JSON_OBJ && this->type != json_e::JSON_NONE){
		note_err_info("该JSON对象不支持键值对添加操作");
		return -1;
	}
	// 类型为空时, 先初始化。
	if(this->type == json_e::JSON_NONE){
		this->free();
		this->type = json_e::JSON_OBJ;
	}
	// 键为空时，不成功。
	if(key == NULL || strlen(key) == 0) {
		note_err_info("键不能为空");
		return -1;
	}

	// 值为空, 不成功
	if(value == nullptr){
		note_err_info("值不能为空");
		return -1;
	}
	char* new_key = new char[strlen(key) + 1];
	if(new_key == NULL){
		throw "error";
	}
	strcpy(new_key, key);
	//键已存在时，不成功。
	if(this->obj.find(new_key) != this->obj.end()){
		note_err_info("键已存在");
		delete [] new_key;
		return -1;
	}

	// 没有问题，拷贝，添加
	this->obj[new_key] = new JSON(*value);
	
	return 0;
}

int JSON::arr_add(int index, JSON* value){
	if(this->type != json_e::JSON_ARR && this->type != json_e::JSON_NONE){
		note_err_info("该JSON对象不支持数组添加操作");
		return -1;
	}
	if(this->type == json_e::JSON_NONE){
		this->free();
		this->type = json_e::JSON_ARR;
	}
	// index超出范围，不成功
	int arr_len = this->arr.size();
	if(index < -1 || index > arr_len){
		//printf("index : %d, size of arr: %d\n", index, arr_len);
		note_err_info("[arr_add] 参数(index)超出范围");
		return -1;
	}
	// 插入数据为空指针
	if(value == nullptr){
		note_err_info("插入数据不能为空指针"); 
	}

	// index = -1, 从末尾插入
	if(index == -1){
		JSON* tmp = new JSON(*value);
		this->arr.push_back(tmp);
	}
	// 正常插入
	else{
		JSON* tmp = new JSON(*value);
		this->arr.insert(this->arr.begin() + index, tmp);
	}
	return 0;
}

int JSON::size(){
	switch(this->type){
		case json_e::JSON_NONE:
			return 0;
		case json_e::JSON_ARR:
			return this->arr.size();
		case json_e::JSON_OBJ:
			return this->obj.size();
		default:
			return 1;
	}
}

int JSON::obj_rm(const char* key){
	assert(key != NULL);
	if(key == NULL){
		this->note_err_info("[obj_rm] 参数错误，key不能为空！");
		return -1;
	}
	auto it = this->obj.find(key);
	if(this->obj.find(key) == this->obj.end()){
		this->note_err_info("[obj_rm] 键不存在");
		return -1;
	}
	delete [] it->first;
	delete it->second;
	this->obj.erase(it);
	return 0;
}

int JSON::arr_rm(int index){
	assert(index >= -1);
	int len = this->arr.size();
	if(index < -1 || index >= len || (index == -1 && len == 0)){
		this->note_err_info("[obj_rm] 参数错误，index超出范围！");
		return -1;	
	}
	if(index == -1){
		delete this->arr[len -1];
		this->arr.pop_back();
	}
	else{
		delete this->arr[index];
		this->arr.erase(this->arr.begin() + index);
	}
	return 0;
}

std::string JSON::to_json_str_without_tab(){
	return this->to_str_select_tab();
}

std::string JSON::to_json_str_with_tab(){
	return this->to_str_select_tab(0);
}

std::string JSON::to_yaml_str(){
	return this->to_yaml_select_tab(0);
}

std::string JSON::get_err_info(){
	return this->err_info;
}

void JSON::free(){
	if(this->err_info != NULL) delete [] this->err_info;
	switch(this->type){
		case json_e::JSON_STR:{
			delete [] this->str;
			break;
		}
		case json_e::JSON_ARR:{
			int arr_len = this->arr.size();
			for(int i = 0; i < arr_len; i++){
				delete this->arr[i];
			}
			this->arr.clear();
			break;
		}
		case json_e::JSON_OBJ:{
			for(auto it = this->obj.begin(); it != this->obj.end(); it++){
				delete [] it->first;
				delete it->second;
			}
			this->obj.clear();
			break;
		}
	}
}

void JSON::note_err_info(const char* info){
	if(err_info != NULL) delete [] err_info;
	this->err_info = new char[strlen(info) + 1];
	strcpy(this->err_info, info);
	fprintf(stderr, "err info: %s\n", info);
}

std::string JSON::pre_place(int indent_len, mode_e mode){
	assert(indent_len >= 0);
	if(indent_len < 0){
		this->note_err_info("[pre_place] 参数错误，indent_len不能小于0！\n");
		throw "error";
	}
	std::string pre = "";
	switch(mode){
		case mode_e::INDENT_TAB:{
			for(int i = 0; i < indent_len; i++){
				pre += "\t";
			}
			return pre;
		}
		case mode_e::INDENT_TWO:{
			for(int i = 0; i < indent_len; i++){
				pre += "  ";
			}
			return pre;
		}
		case mode_e::INDENT_TWO_ARR:{
			for(int i = 0; i < indent_len; i++){
				pre += "  ";
			}
			return pre += "- ";
		}
		case mode_e::INDENT_FOUR:{
			for(int i = 0; i < indent_len; i++){
				pre += "    ";
			}
			return pre;
		}
		case mode_e::INDENT_FOUR_ARR:{
			for(int i = 0; i < indent_len; i++){
				pre += "    ";
			}
			return pre += "  - ";
		}
		default:{
			this->note_err_info("[pre_place] 参数错误，mode不匹配！\n");
			throw "error";
		}
	}
}

std::string JSON::to_yaml_select_tab(int indent_len){
	switch(this->type){
			case json_e::JSON_NONE:{
				return this->none_to_yaml(0);
			}
			case json_e::JSON_NUM:{
				return this->num_to_str(0);
			}
			case json_e::JSON_BOL:{
				return this->bol_to_str(0);
			}
			case json_e::JSON_STR:{
				return this->str_to_str(0);
			}
			case json_e::JSON_ARR:{
				return this->arr_to_yaml(indent_len, mode_e::INDENT_TWO_ARR);
			}
			case json_e::JSON_OBJ:{
				return this->obj_to_yaml(indent_len, mode_e::INDENT_TWO);
			}
		}
}

std::string JSON::to_str_select_tab(int indent_len){
	mode_e mode = mode_e::INDENT_FOUR;
	switch(this->type){
		case json_e::JSON_NONE:{
			return this->none_to_str(indent_len, mode);
		}
		case json_e::JSON_NUM:{
			return this->num_to_str(indent_len, mode);
		}
		case json_e::JSON_BOL:{
			return this->bol_to_str(indent_len, mode);
		}
		case json_e::JSON_STR:{
			return this->str_to_str(indent_len, mode);
		}
		case json_e::JSON_ARR:{
			return this->arr_to_str(indent_len, mode);
		}
		case json_e::JSON_OBJ:{
			return this->obj_to_str(indent_len, mode);
		}
	}
}

std::string JSON::none_to_str(int indent_len, mode_e mode){
	assert(indent_len >= -1);
	if(indent_len < -1){
		note_err_info("[none_to_str] 参数范围错误");
		throw "error";
	}
	// indent_len =-1, 不加tab
	if(indent_len == -1) return "null";
	// 加tab
	return pre_place(indent_len, mode) + "null";
}

std::string JSON::num_to_str(int indent_len, mode_e mode){
	assert(indent_len >= -1);
	if(indent_len < -1){
		note_err_info("[none_to_str] 参数范围错误");
		throw "error";
	}
	char num_to_char[NUM_DIGITS + 2];
	gcvt(this->num, NUM_DIGITS, num_to_char);
	// 不加tab
	if(indent_len == -1){
		return num_to_char;
	}
	// 加tab
	else{
		return pre_place(indent_len, mode) + num_to_char;
	}
}

std::string JSON::bol_to_str(int indent_len, mode_e mode){
	assert(indent_len >= -1);
	if(indent_len < -1){
		note_err_info("[none_to_str] 参数范围错误");
		throw "error";
	}
	std::string value = "";
	// 加tab
	if(indent_len != -1){
		value += pre_place(indent_len, mode);
	}
	// bol to str
	if(this->bol){
		return value + "true";
	}
	else{
		return value + "false";
	}
}

std::string JSON::str_to_str(int indent_len, mode_e mode){
	assert(indent_len >= -1);
	if(indent_len < -1){
		note_err_info("[none_to_str] 参数范围错误");
		throw "error";
	}
	std::string value = "";
	// 加 tab
	if(indent_len != -1){
		value += pre_place(indent_len, mode);
	}
	// str to str
	return value + "\"" + this->str + "\"";
}

std::string JSON::arr_to_str(int indent_len, mode_e mode){
	assert(indent_len >= -1);
	if(indent_len < -1){
		note_err_info("[none_to_str] 参数范围错误");
		throw "error";
	}
	std::string value = "";
	// 加tab，加 [
	if(indent_len != -1){
		value += pre_place(indent_len, mode);
	}
	value += "[";
	
	//换行
	if(indent_len != -1){
		value += "\n";
	}

	// 放入数组中所有对象。
	int arr_len = this->arr.size();
	for(int i = 0; i < arr_len; i++){
		// 不加tab
		if(indent_len == -1){
			value += this->arr[i]->to_str_select_tab(-1);
		}
		// 加tab，tab数要加1
		else{
			value += this->arr[i]->to_str_select_tab(indent_len + 1);
		}
		// 加逗号分隔
		if(i < this->arr.size() - 1){
			value += ",";
		}
		//换行
		if(indent_len != -1){
			value += "\n";
		}
		else if(i < this->arr.size() - 1){
			value += " ";
		}
	}
	// 加tab，加 ]
	if(indent_len != -1){
		value += pre_place(indent_len, mode);
	}
	return value + "]";
}

std::string JSON::obj_to_str(int indent_len, mode_e mode){
	assert(indent_len >= -1);
	if(indent_len < -1){
		note_err_info("[none_to_str] 参数范围错误");
		throw "error";
	}
	std::string value = "", pre = "", pre_next = "";
	// 初始化pre和pre_next
	// 加缩进
	if(indent_len != -1){
		pre = pre_place(indent_len, mode);
		pre_next = pre_place(indent_len + 1, mode);
		value += pre;
	}
	// 加‘{’
	value += "{";
	// 换行
	if(indent_len != -1){
		value += "\n";
	}
	for(auto it = this->obj.begin(); it != this->obj.end(); ){					
		// 加缩进
		if(indent_len != -1){
			value += pre_next;
		}
		// 加name和:
		value += "\"";
		value += it->first;
		value += "\": ";
		// 加值 去缩进
		if(indent_len == -1){
			value += it->second->to_str_select_tab(-1);
		}
		else{
			std::string tmp = it->second->to_str_select_tab(indent_len + 1);
			value += tmp.substr(pre_next.size());
		}
		// 判断是否末尾， 不是则加','
		it++;
		if(it != this->obj.end()){
			value += ",";
		}
		//换行
		if(indent_len != -1){
			value += "\n";
		}
		else if(it != this->obj.end()){
			value += " ";
		}
	}
	// 加缩进，加'}'
	if(indent_len != -1){
		value += pre;
	}
	return value + "}";
}

std::string JSON::none_to_yaml(int indent_len, mode_e mode){
	assert(indent_len >= 0);
	return pre_place(indent_len, mode) + "~";  
}

std::string JSON::obj_to_yaml(int indent_len, mode_e mode){
	assert(indent_len >= 0);
	std::string pre = pre_place(indent_len, mode);
	std::string pre_next = pre_place(indent_len + 1, mode);

	// value开始为空
	std::string value = "";
	for(auto it = this->obj.begin(); it != this->obj.end(); it++){
		//加缩进
		value += pre;
		// 加name和：
		value += it->first;
		value += ":";
		// 根据type来设置缩进和换行
		json_e tmp_type = it->second->type;
		if(tmp_type == json_e::JSON_OBJ || tmp_type == json_e::JSON_ARR){
			value += "\n";
			value += it->second->to_yaml_select_tab(indent_len + 1);
		}
		else{
			value += " ";
			value += it->second->to_yaml_select_tab(0);
			// 换行
			value += "\n";
		}
	}
	return value;
}

std::string JSON::arr_to_yaml(int indent_len, mode_e mode){
	assert(indent_len >= 0);
	std::string pre = pre_place(indent_len, mode);
	std::string pre_next = pre_place(indent_len + 1, mode);

	// value开始为空
	std::string value = "";
	int arr_len = this->arr.size();
	for(int i = 0; i < arr_len; i++){
		value += pre;
		// 根据type来设置缩进和换行
		json_e tmp_type = this->arr[i]->type;
		if(tmp_type == json_e::JSON_OBJ || tmp_type == json_e::JSON_ARR){
			value += "\n";
			value += this->arr[i]->to_yaml_select_tab(indent_len + 1);
		}
		else{
			value += this->arr[i]->to_yaml_select_tab(0);
			//换行
			value += "\n";
		}
	}
	return value;
}

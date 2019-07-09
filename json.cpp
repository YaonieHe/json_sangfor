#include"json.h"

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
JSON::JSON(std::string value){
	err_info = NULL;
	this->type = json_e::JSON_STR;
	this->str = new char[value.size() + 1];
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
			printf("22\n");
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


// void JSON::set_type(json_e type_e){
// 	if(this->type == json_e::JSON_NONE) 
// 		this->type = type_e;
// 	else{
// 		throw "非空类型不允许重新设置类型";
// 	}
// }

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
	free();
	this->type = json_e::JSON_STR;
	this->str = new char[strlen(value) + 1];
	strcpy(this->str, value);
	return 0;
}
int JSON::set_value(std::string value){
	if(this->type != json_e::JSON_NONE && this->type != json_e::JSON_NUM){
		note_err_info("该JSON对象不允许设置为NUM值！\n");
		return -1;
	}
	free();
	this->type = json_e::JSON_STR;
	this->str = new char[value.size() + 1];
	strcpy(this->str, value.c_str());
	return 0;
}


int JSON::obj_add(const char* key, JSON* value){
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
	if(strlen(key) == 0) {
		note_err_info("键不能为空");
		return -1;
	}
	char* new_key = new char[strlen(key) + 1];
	strcpy(new_key, key);
	//键已存在时，不成功。
	if(this->obj.find(new_key) != this->obj.end()){
		note_err_info("键已存在");
		return -1;
	}

	// 值为空, 不成功
	if(value == nullptr){
		note_err_info("值不能为空");
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
		this->arr.push_back(new JSON(*value));
	}
	// 正常插入
	else{
		this->arr.insert(this->arr.begin() + index, new JSON(*value));
	}
	return 0;
}


char* JSON::to_str(){
	switch(this->type){
		case json_e::JSON_NONE:{
			return this->none_to_str();
		}
		case json_e::JSON_NUM:{
			return this->num_to_str();
		}
		case json_e::JSON_BOL:{
			return this->bol_to_str();
		}
		case json_e::JSON_STR:{
			return this->str_to_str();
		}
		case json_e::JSON_ARR:{
			return this->arr_to_str();
		}
		case json_e::JSON_OBJ:{
			return this->obj_to_str();
		}
	}
}

void JSON::free(){

}


void JSON::note_err_info(const char* info){
	if(err_info != NULL) delete [] err_info;
	this->err_info = new char[strlen(info) + 1];
	strcpy(this->err_info, info);
	printf("err info: %s\n", info);
}


char* JSON::get_err_info_ptr(){
	return this->err_info;
}

char* JSON::none_to_str(){
	return strdup("NULL");
}
char* JSON::num_to_str(){
	char* value = new char[NUM_DIGITS + 5];
	gcvt(this->num, NUM_DIGITS, value);
	return value;
}
char* JSON::bol_to_str(){
	char* value = new char[6];
	if(this->bol){
		strcpy(value, "true\0");
	}
	else{
		strcpy(value, "float\0");
	}
	return value;
}
char* JSON::str_to_str(){
	char* value = new char[strlen(this->str) + 3];
	strcpy(value + 1, this->str);
	value[0] = '\"';
	value[strlen(this->str) + 1] = '\"';
	value[strlen(this->str) + 2] = '\0';
	return value;
}
char* JSON::arr_to_str(){
	char *value = NULL, *tmp = NULL;
	value = new char[3];
	value[0] = '[';
	int len = 1;
	for(int i = 0; i < this->arr.size(); i++){
		char* next = this->arr[i]->to_str();
		tmp = new char[len + strlen(next) + 4];
		strcpy(tmp, value);
		strcpy(tmp + len, next);
		len+=strlen(next);
		if(i < this->arr.size() - 1){
			tmp[len++] = ',';
			tmp[len++] = ' ';
			tmp[len] = '\0';
		}
		delete [] next;
		delete [] value;
		value = tmp;
	}
	value[len++] = ']';
	value[len] = '\0';
	return value;
}
char* JSON::obj_to_str(){
	char *value = NULL, *tmp = NULL;
	value = new char[3];
	value[0] = '{';
	int len = 1;			
	for(auto it = this->obj.begin(); it != this->obj.end(); ){
		char* next = it->second->to_str();
		tmp = new char[len + strlen(it->first) + strlen(next) + 7];
		strcpy(tmp, value);
		tmp[len++] = '\"';
		strcpy(tmp + len, it->first);
		len += strlen(it->first);
		tmp[len++] = '\"';
		tmp[len++] = ':';
		strcpy(tmp + len, next);
		len += strlen(next);
		it++;
		if(it != this->obj.end()){
			tmp[len++] = ',';
			tmp[len++] = ' ';
			tmp[len] = '\0';
		}
		delete [] value;
		delete [] next;
		value = tmp;
	}
	value[len++] = '}';
	value[len] = '\0';
	return value;
}
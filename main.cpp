#include"json.h"
#include<gtest/gtest.h>


TEST(TestCreatJSON, InputType){
	JSON* bol_json = new JSON(json_e::JSON_BOL);
    EXPECT_EQ(bol_json->get_type(), json_e::JSON_BOL);
    delete bol_json;
}

TEST(TestCreatJSON, InputValue){
	JSON str_json("char*");
	JSON num_json(1.0);
	JSON bol_json(true);
	EXPECT_EQ(str_json.get_type(), json_e::JSON_STR);
	EXPECT_EQ(num_json.get_type(), json_e::JSON_NUM);
	EXPECT_EQ(bol_json.get_type(), json_e::JSON_BOL);
}

// 测试JSON::set_value
TEST(TestSetJson, SetConstValue){
	// 测试空JSON设置值
	JSON none_json;
	none_json.set_value(1.0);
	EXPECT_EQ(none_json.get_type(), json_e::JSON_NUM);

	// 测试非空JSON设置同类型值
	JSON str_json("2.0");
	EXPECT_EQ(str_json.set_value("3.1"), 0);
	
	// 测试非空JSON设置不同类型值 
	EXPECT_EQ(str_json.set_value(3.1), -1);
}

// 测试JSON::obj_add
TEST(TestAddJSON, AddKeyValue){
	// 空对象添加， 成功
	JSON* obj_json = new JSON();
	ASSERT_TRUE(obj_json != NULL);
	JSON str_json("111");
	EXPECT_EQ(obj_json->obj_add("name1", &str_json), 0);
	EXPECT_EQ(obj_json->get_type(), json_e::JSON_OBJ);

	// OBJ类型对象添加，成功
	delete obj_json;
	obj_json = new JSON(json_e::JSON_OBJ);
	ASSERT_TRUE(obj_json != NULL);
	JSON num_json(2.3);
	EXPECT_EQ(obj_json->obj_add("name2", &num_json), 0);

	// OBJ类型，键已存在时添加， 失败
	EXPECT_EQ(obj_json->get_type(), json_e::JSON_OBJ);
	EXPECT_NE(obj_json->obj_add("name2", &num_json), 0);

	// 非OBJ类型非空对象添加, 失败
	EXPECT_NE(str_json.obj_add("name2", &num_json), 0);

	delete obj_json;
}

// 测试JSON::arr_add
TEST(TestAddJSON, AddArray){
	// 空对象添加，成功
	JSON* none_json = new JSON();
	ASSERT_TRUE(none_json != NULL);
	JSON str_json("insert to arr");
	EXPECT_EQ(none_json->arr_add(-1, &str_json), 0);
	EXPECT_EQ(none_json->get_type(), json_e::JSON_ARR);
	delete none_json;

	// ARR对象末尾添加，成功
	JSON* arr_json = new JSON(json_e::JSON_ARR);
	JSON num_json((NUM)2);
	EXPECT_EQ(arr_json->arr_add(-1, &num_json), 0);
	
	// ARR对象开头添加，成功
	JSON bol_json(true);
	EXPECT_EQ(arr_json->arr_add(0, &bol_json), 0);
	 
	// ARR对象中间添加，成功
	EXPECT_EQ(arr_json->arr_add(1, &bol_json), 0);

	// ARR对象超出最大长度位置添加，失败
	EXPECT_NE(arr_json->arr_add(5, &num_json), 0);
	
	// 非ARR类型非空对象添加，失败
	EXPECT_NE(bol_json.arr_add(0, &num_json), 0);

	delete arr_json;
}

TEST(TestGetJson, GetType){
	JSON str_json("sad");
	EXPECT_EQ(str_json.get_type(), json_e::JSON_STR);
}

TEST(TestGetJson, GetValue){
	JSON str_json("value_str");
	JSON num_json(3.5);
	JSON bol_json(true);

	EXPECT_EQ(num_json.get_from_num(), 3.5);
	EXPECT_EQ(bol_json.get_from_bol(), true);

	std::string str = str_json.get_from_str();
	EXPECT_EQ(str, "value_str");

}
TEST(TestGetJson, GetFromKey){
	JSON str_json("value_str");
	JSON num_json(3.5);
	JSON bol_json(true);

	JSON* obj_json = new JSON(json_e::JSON_OBJ);
	obj_json->obj_add("name1", &num_json);
	obj_json->obj_add("name2", &bol_json);
	JSON* get_name2 = obj_json->get_from_key("name2");
	EXPECT_EQ(get_name2->get_from_bol(), true);
	delete get_name2;
	delete obj_json;
}
TEST(TestGetJson, GetFromArr){
	JSON str_json("value_str");
	JSON num_json(3.5);
	JSON bol_json(true);

	JSON* arr_json = new JSON(json_e::JSON_ARR);
	arr_json->arr_add(-1, &num_json);
	arr_json->arr_add(-1, &bol_json);
	JSON* get_index = arr_json->get_from_arr(-1);
	EXPECT_EQ(get_index->get_from_bol(), true);
	delete get_index;
	delete arr_json;
}


TEST(TestFormatJson, NONEToJsonStr){
 	JSON* none_json = new JSON();
	// NONE to str without tab
	std::string str = none_json->to_json_str_without_tab();
	EXPECT_EQ("null", str);
	//printf("\033[1;31;40mbol_json to str without tab:\n%s\033[0m\n", str);

	// NONE to str with tab
	str = none_json->to_json_str_with_tab();
	EXPECT_EQ("null", str);
	//printf("\033[1;31;40mbol_json to str with tab:\n%s\033[0m\n", str);

	delete none_json;
}

TEST(TestFormatJson, BOLToJsonStr){
 	JSON* bol_json = new JSON(true);
	// BOL to str without tab
	std::string str = bol_json->to_json_str_without_tab();
	ASSERT_TRUE(str.size() != 0);
	EXPECT_EQ("true", str);
	//printf("\033[1;31;40mbol_json to str without tab:\n%s\033[0m\n", str);

	// BOL to str with tab
	str = bol_json->to_json_str_with_tab();
	ASSERT_TRUE(str.size() != 0);
	EXPECT_EQ("true", str);
	//printf("\033[1;31;40mbol_json to str with tab:\n%s\033[0m\n", str);

	delete bol_json;
}

TEST(TestFormatJson, STRToJsonStr){
	JSON* str_json = new JSON("value_str");

	// STR to str without tab
	std::string str = str_json->to_json_str_without_tab();

	EXPECT_EQ("\"value_str\"", str);
	//printf("\033[1;31;40mstr_json to str without tab:\n%s\033[0m\n", str.c_str());

	// STR to str with tab
	str = str_json->to_json_str_with_tab();

	EXPECT_EQ("\"value_str\"", str);
	//printf("\033[1;31;40mstr_json to str with tab:\n%s\033[0m\n", str.c_str());

	delete str_json;
}

TEST(TestFormatJson, NUMToJsonStr){
	JSON* num_json = new JSON((NUM)3.5);

	// NUM to str with tab
	std::string str = num_json->to_json_str_with_tab();
	ASSERT_TRUE(str.size() != 0);
	EXPECT_EQ("3.5", str);
	//printf("\033[1;31;40mnum_json to str with tab:\n%s\033[0m\n", str.c_str());

	// NUM to str without tab
	str = num_json->to_json_str_without_tab();
	ASSERT_TRUE(str.size() != 0);
	EXPECT_EQ("3.5", str);
	//printf("\033[1;31;40mnum_json to str without tab:\n%s\033[0m\n", str.c_str());

	delete num_json;
}

TEST(TestFormatJson, ARRandOBJToJsonStr){
 	JSON* bol_json = new JSON(true);
	JSON* str_json = new JSON("value_str");
	JSON* num_json = new JSON((NUM)3.5);
	JSON* arr_json = new JSON(json_e::JSON_ARR);	// 
	JSON* obj_json = new JSON(json_e::JSON_OBJ);	// {}

	arr_json->arr_add(-1, num_json);
	arr_json->arr_add(0, str_json);
	// ARR to str without tab
	std::string str = arr_json->to_json_str_without_tab();

	EXPECT_EQ("[\"value_str\", 3.5]", str);
	//printf("\033[1;31;40marr_json to str:\n%s\033[0m\n", str.c_str());

	// ARR to str with tab
	str = arr_json->to_json_str_with_tab();

	//EXPECT_EQ("[\n\t\"value_str\",\n\t3.5\n]", str);
	//printf("\033[1;31;40marr_json to str:\n%s\033[0m\n", str.c_str());

	obj_json->obj_add("id1", arr_json);				// {‘id1’ ： }
	obj_json->obj_add("id2", bol_json);				// {‘id1’ ： ， ‘id2’ : }
	// OBJ to str without tab
	str = obj_json->to_json_str_without_tab();

	EXPECT_EQ("{\"id1\": [\"value_str\", 3.5], \"id2\": true}", str);
	//printf("\033[1;31;40mobj_json to str without tab:\n%s\033[0m\n", str.c_str());

	// OBJ to str with tab
	str = obj_json->to_json_str_with_tab();

	//EXPECT_EQ("{\n\t\"id1\": [\n\t\t\"value_str\",\n\t\t3.5\n\t],\n\t\"id2\": true\n}", str);
	//printf("\033[1;31;40mobj_json to str with tab:\n%s\033[0m\n", str.c_str());

	delete bol_json;
	delete num_json;
	delete str_json;
	delete arr_json;
	delete obj_json;
}


TEST(TestFormatYaml, NONEToYamlStr){
 	JSON* none_json = new JSON();
	// none to yaml
	std::string str = none_json->to_yaml_str();
	EXPECT_EQ("~", str);
	//printf("\033[1;31;40mbol_json to str without tab:\n%s\033[0m\n", str);

	delete none_json;
}

TEST(TestFormatYaml, BOLToYamlStr){
 	JSON* bol_json = new JSON(true);
	// BOL to str without tab
	std::string str = bol_json->to_yaml_str();
	ASSERT_TRUE(str.size() != 0);
	EXPECT_EQ("true", str);
	//printf("\033[1;31;40mbol_json to str without tab:\n%s\033[0m\n", str);

	delete bol_json;
}

TEST(TestFormatYaml, STRToYamlStr){
	JSON* str_json = new JSON("value_str");

	// STR to str without tab
	std::string str = str_json->to_yaml_str();

	EXPECT_EQ("\"value_str\"", str);
	//printf("\033[1;31;40mstr_json to str without tab:\n%s\033[0m\n", str.c_str());

	delete str_json;
}

TEST(TestFormatYaml, NUMToYamlStr){
	JSON* num_json = new JSON((NUM)3.5);

	// NUM to str with tab
	std::string str = num_json->to_yaml_str();
	EXPECT_EQ("3.5", str);
	//printf("\033[1;31;40mnum_json to str with tab:\n%s\033[0m\n", str.c_str());

	delete num_json;
}

TEST(TestFormatYaml, ARRandOBJToYamlStr){
 	JSON* bol_json = new JSON(true);
	JSON* str_json = new JSON("value_str");
	JSON* num_json = new JSON((NUM)3.5);
	JSON* arr_json = new JSON(json_e::JSON_ARR);	// 
	JSON* obj_json = new JSON(json_e::JSON_OBJ);	// {}

	arr_json->arr_add(-1, num_json);
	arr_json->arr_add(0, str_json);
	// ARR to yaml
	std::string str = arr_json->to_yaml_str();

	//EXPECT_EQ("\"value_str\"\n3.5\n", str);
	//printf("\033[1;31;40marr_json to yaml:\n%s\033[0m\n", str.c_str());

	obj_json->obj_add("id1", arr_json);				// {‘id1’ ： }
	obj_json->obj_add("id2", bol_json);				// {‘id1’ ： ， ‘id2’ : }
	// OBJ to yaml
	str = obj_json->to_yaml_str();

	//EXPECT_EQ("id1:\n  - \"value_str\"\n  - 3.5\nid2: true\n", str);
	//printf("\033[1;31;40mobj_json to yaml:\n%s\033[0m\n", str.c_str());

	delete bol_json;
	delete num_json;
	delete str_json;
	delete arr_json;
	delete obj_json;
}

TEST(TestFormatComplex, ARRandOBJToStr){
 	JSON* bol_json = new JSON(true);
	JSON* str_json = new JSON("value_str");
	JSON* num_json = new JSON((NUM)3.5);
	JSON* arr_json = new JSON(json_e::JSON_ARR);	// 
	JSON* obj_json = new JSON(json_e::JSON_OBJ);	// {}

	arr_json->arr_add(-1, num_json);
	arr_json->arr_add(0, str_json);
	obj_json->obj_add("id1", arr_json);				// {‘id1’ ： }
	obj_json->obj_add("id2", bol_json);				// {‘id1’ ： ， ‘id2’ : }
	arr_json->arr_add(-1, arr_json);
	arr_json->arr_add(-1, obj_json);
	obj_json->obj_add("cpx1", arr_json);

	// ARR to yaml
	std::string str = arr_json->to_yaml_str();
	printf("\033[1;36;40mcomplex arr_json to yaml:\n%s\033[0m\n", str.c_str());

	// ARR to json
	str = arr_json->to_json_str_with_tab();
	printf("\033[1;36;40mcomplex arr_json to json with tab:\n%s\033[0m\n", str.c_str());

	// OBJ to yaml
	str = obj_json->to_yaml_str();
	printf("\033[1;36;40mcomplex obj_json to yaml:\n%s\033[0m\n", str.c_str());

	// OBJ to json
	str = obj_json->to_json_str_with_tab();
	printf("\033[1;36;40mcomplex obj_json to json with tab:\n%s\033[0m\n", str.c_str());

	delete bol_json;
	delete num_json;
	delete str_json;
	delete arr_json;
	delete obj_json;
}


int main(int argc,char **argv){
  testing::InitGoogleTest(&argc,argv);
  return RUN_ALL_TESTS();
}



// int main(){

// 	try{
// 		// 创建测试
// 		JSON* bol_json = new JSON(json_e::JSON_BOL);
// 		JSON* str_json = new JSON(json_e::JSON_STR);
// 		JSON* arr_json = new JSON(json_e::JSON_ARR);
// 		JSON* obj_json = new JSON(json_e::JSON_OBJ);	// {}
// 		JSON* int_json = new JSON(json_e::JSON_NUM);

// 		// 插入测试
// //		bol_json->set_value<bool>(true);
// //		int_json->set_value<NUM>(1);
// //		str_json->set_value<char*>("hello world");
// 		arr_json->arr_add(-1, int_json);
// 		arr_json->arr_add(0, str_json);
// 		obj_json->obj_add("id1", arr_json);				// {‘id1’ ： }
// 		obj_json->obj_add("id2", bol_json);				// {‘id1’ ： ， ‘id2’ : }

// 		// 查询测试
// //		NUM get_num_value = int_json->get_value<NUM>();
// //		JSON* get_from_key = obj_json->get_from_key("id1");
// //		JSON* get_from_arr = arr_json->get_from_arr(1);

// 		// 格式化测试
// 		char* to_str = obj_json->to_str();
// 	}catch(const char* str){
// 		printf("%s\n", str);
// 	}

// 	return 0;
// }
/*
 * Copyright 2022 Young Mei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>

#include <array>
#include <cfloat>
#include <locale>
#include <stdexcept>

#include <sys/time.h>

#include "context.hpp"

using namespace apache::thrift;

using namespace std;

static void init_Xtruct(Xtruct &s);

ZTEST(thrift, test_void)
{
	PRE();

	context.client->testVoid();

	POST();
}

ZTEST(thrift, test_string)
{
	PRE();

	string s;
	context.client->testString(s, "Test");
	zassert_equal(s, "Test", "");

	POST();
}

ZTEST(thrift, test_bool)
{
	PRE();

	zassert_equal(false, context.client->testBool(false), "");
	zassert_equal(true, context.client->testBool(true), "");

	POST();
}

ZTEST(thrift, test_byte)
{
	PRE();

	zassert_equal(0, context.client->testByte(0), "");
	zassert_equal(-1, context.client->testByte(-1), "");
	zassert_equal(42, context.client->testByte(42), "");
	zassert_equal(-42, context.client->testByte(-42), "");
	zassert_equal(127, context.client->testByte(127), "");
	zassert_equal(-128, context.client->testByte(-128), "");

	POST();
}

ZTEST(thrift, test_i32)
{
	PRE();

	zassert_equal(0, context.client->testI32(0), "");
	zassert_equal(-1, context.client->testI32(-1), "");
	zassert_equal(190000013, context.client->testI32(190000013), "");
	zassert_equal(-190000013, context.client->testI32(-190000013), "");
	zassert_equal(INT32_MAX, context.client->testI32(INT32_MAX), "");
	zassert_equal(INT32_MIN, context.client->testI32(INT32_MIN), "");

	POST();
}

ZTEST(thrift, test_i64)
{
	PRE();

	zassert_equal(0, context.client->testI64(0), "");
	zassert_equal(-1, context.client->testI64(-1), "");
	zassert_equal(7000000000000000123LL, context.client->testI64(7000000000000000123LL), "");
	zassert_equal(-7000000000000000123LL, context.client->testI64(-7000000000000000123LL), "");
	zassert_equal(INT64_MAX, context.client->testI64(INT64_MAX), "");
	zassert_equal(INT64_MIN, context.client->testI64(INT64_MIN), "");

	POST();
}

ZTEST(thrift, test_double)
{
	PRE();

	zassert_equal(0.0, context.client->testDouble(0.0), "");
	zassert_equal(-1.0, context.client->testDouble(-1.0), "");
	zassert_equal(-5.2098523, context.client->testDouble(-5.2098523), "");
	zassert_equal(-0.000341012439638598279,
		      context.client->testDouble(-0.000341012439638598279), "");
	zassert_equal(DBL_MAX, context.client->testDouble(DBL_MAX), "");
	zassert_equal(-DBL_MAX, context.client->testDouble(-DBL_MAX), "");

	POST();
}

ZTEST(thrift, test_binary)
{
	try {
	string rsp;

	//PRE();

	std::thread th([]{context.server->serve();});
	k_msleep(100);
	context.client = setup_client();


	context.client->testBinary(rsp, "");
	zassert_equal("", rsp, "");
	context.client->testBinary(rsp, "Hello");
	zassert_equal("Hello", rsp, "");
	context.client->testBinary(rsp, "H\x03\x01\x01\x00");
	zassert_equal("H\x03\x01\x01\x00", rsp, "");

	context.server->stop();
	th.join();

	} catch( std::exception& e ) {
		printf("failed: %s\n", e.what());
	}
}

ZTEST(thrift, test_struct)
{
	PRE();

	Xtruct request_struct;
	init_Xtruct(request_struct);
	Xtruct response_struct;
	context.client->testStruct(response_struct, request_struct);

	zassert_equal(response_struct, request_struct, NULL);

	POST();
}

ZTEST(thrift, test_nested_struct)
{
	PRE();

	Xtruct2 request_struct;
	request_struct.byte_thing = 1;
	init_Xtruct(request_struct.struct_thing);
	request_struct.i32_thing = 5;
	Xtruct2 response_struct;
	context.client->testNest(response_struct, request_struct);

	zassert_equal(response_struct, request_struct, NULL);

	POST();
}

ZTEST(thrift, test_map)
{
	PRE();

	static const map<int32_t, int32_t> request_map = {
		{0, -10}, {1, -9}, {2, -8}, {3, -7}, {4, -6}};

	map<int32_t, int32_t> response_map;
	context.client->testMap(response_map, request_map);

	zassert_equal(request_map, response_map, "");

	POST();
}

ZTEST(thrift, test_string_map)
{
	PRE();

	static const map<string, string> request_smap = {
		{"a", "2"}, {"b", "blah"}, {"some", "thing"}
	};
	map<string, string> response_smap;

	context.client->testStringMap(response_smap, request_smap);
	zassert_equal(response_smap, request_smap, "");

	POST();
}

ZTEST(thrift, test_set)
{
	PRE();

	static const set<int32_t> request_set = {-2, -1, 0, 1, 2};

	set<int32_t> response_set;
	context.client->testSet(response_set, request_set);

	zassert_equal(request_set, response_set, "");

	POST();
}

ZTEST(thrift, test_list)
{
	PRE();

	vector<int32_t> response_list;
	context.client->testList(response_list, vector<int32_t>());
	zassert_true(response_list.empty(), "Unexpected list size: %llu", response_list.size());

	static const vector<int32_t> request_list = {-2, -1, 0, 1, 2};

	response_list.clear();
	context.client->testList(response_list, request_list);
	zassert_equal(request_list, response_list, "");

	POST();
}

ZTEST(thrift, test_enum)
{
	PRE();

	Numberz::type response = context.client->testEnum(Numberz::ONE);
	zassert_equal(response, Numberz::ONE, NULL);

	response = context.client->testEnum(Numberz::TWO);
	zassert_equal(response, Numberz::TWO, NULL);

	response = context.client->testEnum(Numberz::EIGHT);
	zassert_equal(response, Numberz::EIGHT, NULL);

	POST();
}

ZTEST(thrift, test_typedef)
{
	PRE();

	UserId uid = context.client->testTypedef(309858235082523LL);
	zassert_equal(uid, 309858235082523LL, "Unexpected uid: %llu", uid);

	POST();
}

ZTEST(thrift, test_nested_map)
{
	PRE();

	map<int32_t, map<int32_t, int32_t>> mm;
	context.client->testMapMap(mm, 1);

	zassert_equal(mm.size(), 2, NULL);
	zassert_equal(mm[-4][-4], -4, NULL);
	zassert_equal(mm[-4][-3], -3, NULL);
	zassert_equal(mm[-4][-2], -2, NULL);
	zassert_equal(mm[-4][-1], -1, NULL);
	zassert_equal(mm[4][4], 4, NULL);
	zassert_equal(mm[4][3], 3, NULL);
	zassert_equal(mm[4][2], 2, NULL);
	zassert_equal(mm[4][1], 1, NULL);

	POST();
}

ZTEST(thrift, test_exception)
{
	PRE();

	std::exception_ptr eptr = nullptr;

	try {
		context.client->testException("Xception");
	} catch (...) {
		eptr = std::current_exception();
	}
	zassert_not_equal(nullptr, eptr, "an exception was not thrown");

	eptr = nullptr;
	try {
		context.client->testException("TException");
	} catch (...) {
		eptr = std::current_exception();
	}
	zassert_not_equal(nullptr, eptr, "an exception was not thrown");

	context.client->testException("success");

	POST();
}

ZTEST(thrift, test_multi_exception)
{
	PRE();

	std::exception_ptr eptr = nullptr;

	try {
		Xtruct result;
		context.client->testMultiException(result, "Xception", "test 1");
	} catch (...) {
		eptr = std::current_exception();
	}
	zassert_not_equal(nullptr, eptr, "an exception was not thrown");

	eptr = nullptr;
	try {
		Xtruct result;
		context.client->testMultiException(result, "Xception2", "test 2");
	} catch (...) {
		eptr = std::current_exception();
	}
	zassert_not_equal(nullptr, eptr, "an exception was not thrown");

	POST();
}

static void init_Xtruct(Xtruct &s)
{
	PRE();

	s.string_thing = "Zero";
	s.byte_thing = 1;
	s.i32_thing = -3;
	s.i64_thing = -5;

	POST();
}

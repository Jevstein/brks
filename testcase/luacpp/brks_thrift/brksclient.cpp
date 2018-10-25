/*
 *  brksthriftapdater.cpp 
 *  brksthriftapdater
 *
 *  Created by Jevstein on 2018/10/25 9:40.
 *  Copyright @ 2018year Jevstein. All rights reserved.
 *
 */

#include <iostream>
extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}
#include "brksclient.h"
#include "gen-cpp/BRKService.h"
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <boost/shared_ptr.hpp>
using namespace std;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace brkservice;

#define MY_EXPORTS
#define BC_OK 0
#define BC_NO -1

#define SINGLETON(type)\
	static inline type* instance()\
{\
	static type _instance; \
	return &_instance; \
}

/************************************************************************/
/* 业务实现                                                             */
/************************************************************************/
class BRKSClient
{
public:
	BRKSClient(const char* host, int port)
		: socket_(new TSocket(host, port))
		, transport_(new TFramedTransport(socket_))//TBufferedTransport
		, protocol_(new TCompactProtocol(transport_))//TBinaryProtocol
		, service_(protocol_){}
	~BRKSClient(){}

public:
	boost::shared_ptr<TSocket> socket_;
	boost::shared_ptr<TTransport> transport_;
	boost::shared_ptr<TProtocol> protocol_;
	BRKServiceClient service_;
};

class BRKSManager
{
public:
	BRKSManager() { BRKSManager::brks_client__ == NULL; }
	SINGLETON(BRKSManager);
	~BRKSManager(){ destroy(); }

public:
	BRKServiceClient* serve() { return BRKSManager::brks_client__ ? &BRKSManager::brks_client__->service_ : NULL; }

	bool create(const char* host, int port)
	{
		if (NULL == BRKSManager::brks_client__)
		{
			BRKSManager::brks_client__ = new BRKSClient(host, port);
			BRKSManager::brks_client__->transport_->open();
		}

		return true;
	}

	void destroy()
	{
		if (NULL != BRKSManager::brks_client__)
		{
			BRKSManager::brks_client__->transport_->close();
			delete BRKSManager::brks_client__;
			BRKSManager::brks_client__ == NULL;
		}
	}

private:
	static BRKSClient *brks_client__;
};

BRKSClient* BRKSManager::brks_client__ = NULL;

/************************************************************************/
/* 1.函数实现                                                           */
/************************************************************************/
int brk_init(lua_State* L)
{
	std::string host(lua_tostring(L, 1));
	int port = lua_tonumber(L, 2);
	printf("brk_init::host=%s, port=%d\n", host.c_str(), port);

	if (!BRKSManager::instance()->create(host.c_str(), port))
	{
		printf("error: failed to open brks\n");
		lua_pushnumber(L, BC_NO);
		return 1;
	}

	lua_pushnumber(L, BC_OK);
	return 1;
}

int brk_uninit(lua_State* L)
{
	printf("brk_uninit\n");

	BRKSManager::instance()->destroy();

	lua_pushnumber(L, BC_OK);
	return 1;
}

int brk_get_mobile_code(lua_State* L)
{
	std::string mobile(lua_tostring(L, 1));
	printf("brk_get_mobile_code::mobile=%s\n", mobile.c_str());

	::brkservice::ApiResult rst;
	rst.resultCode == 200;

	if (BRKSManager::instance()->serve())
		BRKSManager::instance()->serve()->brk_get_mobile_code(rst, mobile);
	else
		printf("error: failed to get service!\n");

	lua_pushnumber(L, rst.resultCode);
	lua_pushstring(L, rst.resultMsg.c_str());
	lua_pushstring(L, rst.data.c_str());
	return 3;
}

int brk_login(lua_State* L)
{
	std::string mobile(lua_tostring(L, 1));
	std::string code(lua_tostring(L, 2));
	printf("brk_login::mobile=%s, code=%s\n", mobile.c_str(), code.c_str());

	lua_pushnumber(L, BC_OK);
	return 1;
}

int brk_recharge(lua_State* L)
{
	std::string mobile(lua_tostring(L, 1));
	int amount = lua_tonumber(L, 2);
	printf("brk_recharge::mobile=%s, amount=%d\n", mobile.c_str(), amount);

	lua_pushnumber(L, BC_OK);
	return 1;
}

int brk_get_account_balance(lua_State* L)
{
	std::string mobile(lua_tostring(L, 1));
	printf("brk_get_account_balance::mobile=%s\n", mobile.c_str());

	lua_pushnumber(L, BC_OK);
	return 1;
}

int brk_list_account_records(lua_State* L)
{
	std::string mobile(lua_tostring(L, 1));
	printf("brk_list_account_records::mobile=%s\n", mobile.c_str());

	lua_pushnumber(L, BC_OK);
	return 1;
}

int brk_unlock(lua_State* L)
{
	std::string mobile(lua_tostring(L, 1));
	std::string bike_code(lua_tostring(L, 2));
	printf("brk_unlock::mobile=%s, bike_code=%s\n", mobile.c_str(), bike_code.c_str());

	lua_pushnumber(L, BC_OK);
	return 1;
}

int brk_list_travel(lua_State* L)
{
	std::string mobile(lua_tostring(L, 1));
	printf("brk_list_travel::mobile=%s\n", mobile.c_str());

	lua_pushnumber(L, BC_OK);
	return 1;
}

int brk_lock(lua_State* L)
{
	std::string mobile(lua_tostring(L, 1));
	std::string bike_code(lua_tostring(L, 2));
	printf("brk_lock::mobile=%s, bike_code=%s\n", mobile.c_str(), bike_code.c_str());

	lua_pushnumber(L, BC_OK);
	return 1;
}

/************************************************************************/
/* 2.函数数组                                                           */
/************************************************************************/
static const struct luaL_Reg funcs__[] =
{
	{ "brk_init", brk_init },
	{ "brk_uninit", brk_uninit },
	{ "brk_get_mobile_code", brk_get_mobile_code },
	{ "brk_login", brk_login },
	{ "brk_recharge", brk_recharge },
	{ "brk_get_account_balance", brk_get_account_balance },
	{ "brk_list_account_records", brk_list_account_records },
	{ "brk_unlock", brk_unlock },
	{ "brk_list_travel", brk_list_travel },
	{ "brk_lock", brk_lock },
	{ NULL, NULL }
};

/************************************************************************/
/* 3.导出接口                                                           */
/*   [注]函数名luaopen_libbrksclient中的'libbrksclient'必须为库函数名，如:*/
/*       libbrksclient.so                                                */
/************************************************************************/
MY_REPORT_API int CALLMODE luaopen_libbrksclient(lua_State* L)
{
	//lua5.1之前版本使用如下:
	//luaL_openlib(L, "MyMath", funcs__, 0); //或luaL_register(L, "mymath", funcs__);

	//lua5.2以上版本使用如下:
	lua_getglobal(L, "brks_client");
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
	}
	luaL_setfuncs(L, funcs__, 0);
	lua_setglobal(L, "brks_client");

	return 0;
}

//complile:
// $ g++ -fPIC -shared -o libbrksclient.so *.cpp gen-cpp/*cpp -llua -lthrift -ldl
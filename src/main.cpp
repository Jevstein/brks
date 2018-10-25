
#include "DispatchMsgService.h"
#include "interface.h"
#include "Logger.h"
#include "sqlconnection.h"
#include "BusProcessor.h"

#include <functional>

extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}


/************************************************************************/
/* global value                                                         */
/************************************************************************/
//pop interge
#define POP_VALUE_INT(L, key)\
({ \
	int ret = 0; \
	lua_getglobal(L, key); \
	if (lua_isnumber(L, -1)) { ret = (int)lua_tointeger(L, -1); lua_pop(L, 1); }\
	(ret);\
})
//pop string
#define POP_VALUE_STR(L, key)\
({ \
	std::string ret = ""; \
	lua_getglobal(L, key); \
	if (lua_isstring(L, -1)) { ret = (std::string)lua_tostring(L, -1); lua_pop(L, 1); }\
	(ret);\
})

/************************************************************************/
/* field value                                                          */
/************************************************************************/
//pop filed interge
#define POP_FIELD_INT(L, key)\
({ \
	int ret = 0; \
	lua_getfield(L, -1, key); \
	if (lua_isnumber(L, -1)) { ret = (int)lua_tointeger(L, -1); lua_pop(L, 1); }\
	(ret); \
})
//pop filed string
#define POP_FIELD_STR(L, key)\
({ \
	std::string ret = ""; \
	lua_getfield(L, -1, key); \
	if (lua_isstring(L, -1)) { ret = (std::string)lua_tostring(L, -1); lua_pop(L, 1); }\
	(ret); \
})

//数据库配置信息
struct st_db_conf
{
	std::string db_ip;
	unsigned int db_port;
	std::string db_user;
	std::string db_pwd;
	std::string db_name;
};

class LuaCfg
{
public:
	LuaCfg(){ init(); }
	~LuaCfg(){ uninit(); }

public:
	bool load_file(const char *script)
	{
		int ret = luaL_dofile(L_, script);
		if (LUA_OK != ret)
		{
			printf("Failed to load file: '%s', ret=%d!\n", script, ret);
			return false;
		}

		// -- database
		lua_getglobal(L_, "db_argv");
		db_conf_.db_ip = POP_FIELD_STR(L_, "db_ip");
		db_conf_.db_port = POP_FIELD_INT(L_, "db_port");
		db_conf_.db_user = POP_FIELD_STR(L_, "db_user");
		db_conf_.db_pwd = POP_FIELD_STR(L_, "db_pwd");
		db_conf_.db_name = POP_FIELD_STR(L_, "db_name");
		printf("db: '%s', %d, '%s', '%s', '%s'\n"
			, db_conf_.db_ip.c_str(), db_conf_.db_port, db_conf_.db_user.c_str(), db_conf_.db_pwd.c_str(), db_conf_.db_name.c_str());
		lua_pop(L_, 1);

		// -- server
		srv_port_ = POP_VALUE_INT(L_, "srv_port");
		printf("server port: %d\n", srv_port_);

		return true;
	}

protected:
	bool init()
	{
		L_ = luaL_newstate();
		if (NULL == L_)
		{
			std::cout << "Failed to create Lua State!" << std::endl;
			return false;
		}

		luaL_openlibs(L_);

		return true;
	}

	void uninit()
	{
		lua_close(L_);
	}

public:
	st_db_conf db_conf_;
	unsigned int srv_port_;

private:
	lua_State *L_;
};

int main(int argc, char** argv)
{
	LuaCfg cfg;
	if (!cfg.load_file("conf/conf.lua"))
	{
		printf("failed to load config.\n");
		return -1;
	}

    if (argc != 2)
    {
        printf("please input brks <log file config>!\n");
        return -1;
    }
    
    if(!Logger::instance()->init(std::string(argv[1])))
    {
        printf("init log module failed.\n");
        return -1;
    }
    else
    {
        printf("init log module success!\n");
    }

    std::shared_ptr<DispatchMsgService> dms(new DispatchMsgService);
    dms->open();
    
    std::shared_ptr<MysqlConnection> mysqlconn(new MysqlConnection);
    // mysqlconn->Init("127.0.0.1", 3306, "root", "123456", "dongnaobike");
    mysqlconn->Init(cfg.db_conf_.db_ip.c_str(), cfg.db_conf_.db_port, cfg.db_conf_.db_user.c_str(), cfg.db_conf_.db_pwd.c_str(), cfg.db_conf_.db_name.c_str());
    
    BusinessProcessor processor(dms, mysqlconn);
    processor.init();

    std::function< iEvent* (const iEvent*)> fun = std::bind(&DispatchMsgService::process, dms.get(), std::placeholders::_1);
    
    Interface intf(fun);
    // intf.start(9090);
    intf.start(cfg.srv_port_);

    LOG_INFO("brks start successful!");

    for(;;);
    
    return 0;
}

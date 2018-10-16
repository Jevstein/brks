
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

class LuaParser
{
    LuaParser(){}
public:
    LuaParser(const char *script){ init(script); }
    ~LuaParser(){ uninit(); }

public:
    std::string load_string(const char *str)
    {
        lua_getglobal(L_, str);
        if (lua_isstring(L_, -1))
        {
            return (std::string)lua_tostring(L_, -1);
        }
        return "";
    }

    int load_integer(const char *str)
    {
        lua_getglobal(L_, str);
        if (lua_isnumber(L_, -1))
        {
            return (int)lua_tointeger(L_, -1);
        }
        return -1;
    }

protected:
    bool init(const char *script)
    {
        L_ = luaL_newstate();
        if (NULL == L_)
        {
            std::cout << "Failed to create Lua State!" << std::endl;
            return false;
        }

        luaL_openlibs(L_);

        int ret = luaL_dofile(L_, script);
        if (ret != 0)
        {
            std::cout << "Failed to load file: '" << script << "'!" << std::endl;
            return false;
        }

        return true;
    }

    void uninit()
    {
        lua_close(L_);
    }

private:
    lua_State *L_;
};

int main(int argc, char** argv)
{
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
        printf("init log module success!");
    }

    LuaParser parser("./scripts/cfg.lua");
    // -- db
    std::string db_ip = parser.load_string("db_ip");
    int db_port = parser.load_integer("db_port");
    std::string  db_user = parser.load_string("db_user");
    std::string db_pwd = parser.load_string("db_pwd");
    std::string db_name = parser.load_string("db_name");
    // printf("db: '%s', %d, '%s', '%s', '%s' \n", db_ip.c_str(), db_port, db_user.c_str(), db_pwd.c_str(), db_name.c_str());
    // -- network
    int net_port = parser.load_integer("net_port");
    // printf("network: %d\n", net_port);

    std::shared_ptr<DispatchMsgService> dms(new DispatchMsgService);
    dms->open();
    
    std::shared_ptr<MysqlConnection> mysqlconn(new MysqlConnection);
    // mysqlconn->Init("127.0.0.1", 3306, "root", "123456", "dongnaobike");
    mysqlconn->Init(db_ip.c_str(), db_port, db_user.c_str(), db_pwd.c_str(), db_name.c_str());
    
    BusinessProcessor processor(dms, mysqlconn);
    processor.init();

    std::function< iEvent* (const iEvent*)> fun = std::bind(&DispatchMsgService::process, dms.get(), std::placeholders::_1);
    
    Interface intf(fun);
    // intf.start(9090);
    intf.start(net_port);

    LOG_INFO("brks start successful!");

    for(;;);
    
    return 0;
}
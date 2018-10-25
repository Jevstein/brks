/*
 *  brksthriftapdater.h 
 *  brksthriftapdater
 *
 *  Created by Jevstein on 2018/10/25 9:39.
 *  Copyright @ 2018year Jevstein. All rights reserved.
 *
 */

#ifdef __cplusplus
#	define EXTERN_C extern "C"
#else//!__cplusplus
#	define EXTERN_C
#endif//__cplusplus

#ifdef WIN32
#	ifdef MY_EXPORTS
#		define MY_REPORT_API EXTERN_C _declspec(dllexport)
#	else
#		define MY_REPORT_API EXTERN_C _declspec(dllimport)
#	endif
#	define CALLMODE __cdecl //__stdcall
#else//!WIN32
#	define MY_REPORT_API EXTERN_C
#	define CALLMODE
#endif//WIN32

/************************************************************************/
/* 接口声明                                                             */
/************************************************************************/
/**
* 初始化
*/
int brk_init(lua_State* L);
/**
* 反初始化
*/
int brk_uninit(lua_State* L);

/**
* 登录：int brk_get_mobile_code(1:string mobile)
*/
int brk_get_mobile_code(lua_State* L);
/**
* 登录：int brk_login(1:string mobile, 2 : string code)
*/
int brk_login(lua_State* L);
/**
*充值：int brk_recharge(1:string mobile, 2 : i32 amount)
*/
int brk_recharge(lua_State* L);
/**
*查询账户余额：int brk_get_account_balance(1:string mobile)
*/
int brk_get_account_balance(lua_State* L);
/**
*查询所有消费记录：int brk_list_account_records(1:string mobile)
*/
int brk_list_account_records(lua_State* L);
/**
* 解锁：int brk_unlock(1:string mobile, 2 : string bike_code)
*/
int brk_unlock(lua_State* L);
/**
*查询用户里程：int brk_list_travel(1:string mobile)
*/
int brk_list_travel(lua_State* L);
/**
* 结束行程：int brk_lock(1:string mobile, 2 : string bike_code)
*/
int brk_lock(lua_State* L);

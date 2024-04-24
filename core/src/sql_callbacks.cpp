#include"sql_callbacks.h"

#include<iostream>
#include<string>
#include<string.h>

namespace cbs = rena::et::core::utils::callbacks;

int cbs::sqlcb_check_table_list( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname ){
    table_checklist_t* checklist = ( table_checklist_t* ) __p_v_data;
    if ( strcmp( __pp_c_argv[0] , "HEADER" ) == 0 )
    {
        checklist -> header++;
    }
    else if ( strcmp( __pp_c_argv[0] , "DATA" ) == 0 )
    {
        checklist -> data++;
    }
    else
    {
        checklist -> unknown++;
    }
    return 0;
}

#define CHECK_TABLE_COL( argv , cid , name , type , notnull )   \
    (                                                           \
        strcmp( argv[0] , cid ) == 0 &&                         \
        strcmp( argv[1] , name ) == 0 &&                        \
        strcmp( argv[2] , type ) == 0 &&                        \
        strcmp( argv[3] , notnull ) == 0                        \
    )

int cbs::sqlcb_check_header_table_col( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname ){
    header_table_col_checklist_t* checklist = ( header_table_col_checklist_t* ) __p_v_data;
    if ( CHECK_TABLE_COL( __pp_c_argv , "0" , "NAME" , "TEXT" , "1" ) )
    {
        checklist -> name++;
    }
    else if ( CHECK_TABLE_COL( __pp_c_argv , "1" , "CREATE_TIME" , "INT" , "1" ) )
    {
        checklist -> create_time++;
    }
    else if ( CHECK_TABLE_COL( __pp_c_argv , "2" , "DESCRIPTION" , "TEXT" , "0" ) )
    {
        checklist -> description++;
    }
    else if ( CHECK_TABLE_COL( __pp_c_argv , "3" , "OWNERS" , "TEXT" , "0" ) )
    {
        checklist -> owners++;
    }
    else if ( CHECK_TABLE_COL( __pp_c_argv , "4" , "ENABLE_BALANCE" , "INT" , "1" ) )
    {
        checklist -> enable_balance++;
    }
    else
    {
        checklist -> unknown++;
    }
    return 0;
}

int cbs::sqlcb_check_data_table_col( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname ){
    data_table_col_checklist_t* checklist = ( data_table_col_checklist_t* ) __p_v_data;
    if ( CHECK_TABLE_COL( __pp_c_argv , "0" , "ID" , "INT" , "1" ) )
    {
        checklist -> id++;
    }
    else if ( CHECK_TABLE_COL( __pp_c_argv , "1" , "TIME_STAMP" , "INT" , "1" ) )
    {
        checklist -> time_stamp++;
    }
    else if ( CHECK_TABLE_COL( __pp_c_argv , "2" , "TIME_ZONE" , "INT" , "1" ) )
    {
        checklist -> time_zone++;
    }
    else if ( CHECK_TABLE_COL( __pp_c_argv , "3" , "PAYMENT" , "REAL" , "1" ) )
    {
        checklist -> payment++;
    }
    else if ( CHECK_TABLE_COL( __pp_c_argv , "4" , "CURRENCY_TYPE" , "INT" , "1" ) )
    {
        checklist -> currency_type++;
    }
    else if ( CHECK_TABLE_COL( __pp_c_argv , "5" , "DESCRIPTION" , "TEXT" , "0" ) )
    {
        checklist -> description++;
    }
    else if ( CHECK_TABLE_COL( __pp_c_argv , "6" , "CURRENCY_EXCHANGE" , "TEXT" , "1" ) )
    {
        checklist -> currency_exchange++;
    }
    else if ( CHECK_TABLE_COL( __pp_c_argv , "7" , "TOTAL_PAYMENT_AFTER_EXCHANGE" , "REAL" , "1" ) )
    {
        checklist -> total_payment_after_exchange++;
    }
    else if ( CHECK_TABLE_COL( __pp_c_argv , "8" , "BALANCE" , "REAL" , "0" ) )
    {
        checklist -> balance++;
    }
    else
    {
        checklist -> unknown++;
    }
    return 0;
}

int cbs::sqlcb_get_table_length( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname ){
    int* len = ( int* ) __p_v_data;
    *len = std::stoi( __pp_c_argv[0] );
    // this shouldn't throw exception: this callback should only be used with `SELECT COUNT(*) FROM {TABLE}` cmd
    return 0;
}

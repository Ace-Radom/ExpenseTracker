#ifndef _EXPENSETRACKER_CORE_SQL_CALLBACKS_H_
#define _EXPENSETRACKER_CORE_SQL_CALLBACKS_H_

#include"sqlite3.h"

namespace rena::et::core::utils::callbacks {

    typedef struct {
        unsigned short header;
        unsigned short data;
        unsigned short unknown;
    } table_checklist_t;

    int sqlcb_check_table_list( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname );

    typedef struct {
        unsigned short name;
        unsigned short create_time;
        unsigned short description;
        unsigned short owners;
        unsigned short enable_balance;
        unsigned short unknown;
    } header_table_col_checklist_t;

    int sqlcb_check_header_table_col( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname );

    typedef struct {
        unsigned short id;
        unsigned short time_stamp;
        unsigned short time_zone;
        unsigned short payment;
        unsigned short currency_type;
        unsigned short description;
        unsigned short currency_exchange;
        unsigned short total_payment_after_exchange;
        unsigned short balance;
        unsigned short unknown;
    } data_table_col_checklist_t;

    int sqlcb_check_data_table_col( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname );

} // namespace rena::et::core::utils::callbacks

#endif // _EXPENSETRACKER_CORE_SQL_CALLBACKS_H_

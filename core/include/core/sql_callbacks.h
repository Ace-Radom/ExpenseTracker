#ifndef _EXPENSETRACKER_CORE_SQL_CALLBACKS_H_
#define _EXPENSETRACKER_CORE_SQL_CALLBACKS_H_

namespace rena::et::core::utils::callbacks {

    typedef struct {
        unsigned short format;
        unsigned short calibration;
        unsigned short header;
        unsigned short data;
        unsigned short unknown;
    } table_checklist_t;

    int sqlcb_check_table_list( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname );

    typedef struct {
        unsigned short format_version;
        unsigned short et_version;
        unsigned short et_commit;
        unsigned short et_branch;
        unsigned short first_create_time;
        unsigned short unknown;
    } format_table_col_checklist_t;

    int sqlcb_check_format_table_col( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname );
    
    typedef struct {
        unsigned short calibration_code;
        unsigned short unknown;
    } calibration_table_col_checklist_t;

    int sqlcb_check_calibration_table_col( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname );

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

    int sqlcb_get_table_length( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname );
    int sqlcb_get_format_data( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname );
    int sqlcb_get_calibration_data( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname );
    int sqlcb_get_header_data( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname );
    int sqlcb_get_data_data( void* __p_v_data , int __i_argc , char** __pp_c_argv , char** __pp_c_azcolname );

} // namespace rena::et::core::utils::callbacks

#endif // _EXPENSETRACKER_CORE_SQL_CALLBACKS_H_

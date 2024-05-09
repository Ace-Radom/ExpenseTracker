#ifndef _EXPENSETRACKER_CORE_SQL_H_
#define _EXPENSETRACKER_CORE_SQL_H_

#include<filesystem>
#include<vector>

#include"core/exception.h"
#include"core/sql_types.h"
#include"sqlite3.h"

#define ERR_SQL_NOT_OPEN                                1
#define ERR_SQL_CREATE_FORMAT_TABLE_FAILED              2
#define ERR_SQL_CREATE_CALIBRATION_TABLE_FAILED         3
#define ERR_SQL_CREATE_HEADER_TABLE_FAILED              4
#define ERR_SQL_CREATE_DATA_TABLE_FAILED                5
#define ERR_SQL_ILLEGAL_DB                              6
#define ERR_SQL_CHECK_DB_CMD_EXEC_FAILED                7
#define ERR_SQL_GET_TABLE_LENGTH_FAILED                 8
#define ERR_SQL_CLEAN_UP_TABLE_FAILED                   9
#define ERR_SQL_INSERT_FORMAT_TABLE_FAILED             10
#define ERR_SQL_INSERT_CALIBRATION_TABLE_FAILED        11
#define ERR_SQL_INSERT_HEADER_TABLE_FAILED             12
#define ERR_SQL_INSERT_DATA_TABLE_FAILED               13
#define ERR_SQL_GET_HEADER_DATA_FAILED                 14
#define ERR_SQL_GET_DATA_DATA_FAILED                   15
#define ERR_SQL_FORMAT_TABLE_REWRITE                   16
#define ERR_SQL_CALIBRATION_TABLE_REWRITE              17
#define ERR_SQL_UNEXPECTED                            255

namespace rena::et::core::utils {

    DECLARE_CORE_EXCEPTION( sql_exception );
    
    typedef int ( *sql_callback )( void* , int , char** , char** );

    typedef struct {
        sql_callback f_cb;
        std::string s_cb_name;
    } callback_data_t;

    class sql {

        public:
            sql( std::filesystem::path __p_dbpath );
            ~sql();

            void write_header( const header_dat_t& __shd_data );
            void write_data( const data_dat_t& __sdd_data );
            const header_dat_t* get_header();
            const std::vector<data_dat_t>* get_data( int __i_id );
            unsigned int get_header_len();
            unsigned int get_data_len();

        protected:
            typedef enum {
                FORMAT,
                CALIBRATION,
                HEADER,
                DATA
            } _tablename_t;

        protected:
            void _create_tables();
            void _write_format_calibration();
            bool _check_db_legality( std::string* __out_p_s_errmsg );
            unsigned int _get_table_len( _tablename_t __e_table );

            void _exec_sqlcmd( const std::string& __s_cmd , int __i_errno );
            void _exec_sqlcmd( const std::string& __s_cmd , const callback_data_t& __scd_cb , void* __p_v_data , int __i_errno );

        private:
            std::filesystem::path _p_dbpath;
            sqlite3* _p_sql3_dbobj;

    }; // class sql

} // namespace rena::et::core::utils

#endif // _EXPENSETRACKER_CORE_SQL_H_

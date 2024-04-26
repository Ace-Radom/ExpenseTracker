#ifndef _EXPENSETRACKER_CORE_SQL_H_
#define _EXPENSETRACKER_CORE_SQL_H_

#include<filesystem>
#include<vector>

#include"sqlite3.h"

#include"exception.h"
#include"sql_types.h"

#define ERR_SQL_NOT_OPEN                                1
#define ERR_SQL_CREATE_HEADER_TABLE_FAILED              2
#define ERR_SQL_CREATE_DATA_TABLE_FAILED                3
#define ERR_SQL_ILLEGAL_DB                              4
#define ERR_SQL_CHECK_DB_CMD_EXEC_FAILED                5
#define ERR_SQL_GET_TABLE_LENGTH_FAILED                 6
#define ERR_SQL_CLEAN_UP_TABLE_FAILED                   7
#define ERR_SQL_INSERT_HEADER_TABLE_FAILED              8
#define ERR_SQL_INSERT_DATA_TABLE_FAILED                9
#define ERR_SQL_GET_HEADER_DATA_FAILED                 10
#define ERR_SQL_GET_DATA_DATA_FAILED                   11
#define ERR_SQL_UNEXPECTED                            255

namespace rena::et::core::utils {

    DECLARE_CORE_EXCEPTION( sql_exception );
    
    typedef int ( *sql_callback )( void* , int , char** , char** );

    class sql {

        public:
            sql( std::filesystem::path __p_dbpath );
            ~sql();

            void write_header( const header_dat_t& __shd_data );
            void write_data( const data_dat_t& __sdd_data );
            const header_dat_t* get_header();
            const std::vector<data_dat_t>* get_data( int __i_id );

        protected:
            typedef enum {
                HEADER,
                DATA
            } _tablename_t;

        protected:
            void _create_tables();
            bool _check_db_legality( std::string* __out_p_s_errmsg );
            unsigned int _get_table_len( _tablename_t __e_table );

            void _exec_sqlcmd( const std::string& __s_cmd , int __i_errno );
            void _exec_sqlcmd( const std::string& __s_cmd , sql_callback __f_sqlcb , void* __p_v_data , int __i_errno );

        private:
            std::filesystem::path _p_dbpath;
            sqlite3* _sql_dbobj;

    }; // class sql

} // namespace rena::et::core::utils

#endif // _EXPENSETRACKER_CORE_SQL_H_

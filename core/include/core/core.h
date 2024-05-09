#ifndef _EXPENSETRACKER_CORE_CORE_H_
#define _EXPENSETRACKER_CORE_CORE_H_

#include<filesystem>
#include<string>
#include<memory>

#include"core/exception.h"
#include"core/sql.h"
#include"g3log/g3log.hpp"
#include"g3log/logworker.hpp"

#define ERR_CORE_ILLEGAL_DB_NAME                        1
#define ERR_CORE_DB_NOT_FOUND                           2
#define ERR_CORE_CREATE_EXISTING_DB                     3
#define ERR_CORE_DB_REOPEN                              4
#define ERR_CORE_CONFIG_ERROR                         128
#define ERR_CORE_SQL_ERROR                            256
#define ERR_CORE_UNKNOWN                              512

namespace rena::et::core {

    class core {

        public:
            core();
            ~core();

            void open( const std::string& __s_name );
            void create( const std::string& __s_name );
            void close();

        protected:
            void _open_db( const std::string& __s_name , bool __b_create );
            std::filesystem::path _get_db_folder() const;
            static bool _check_db_name_legality( const std::string& __s_name );

        private:
            utils::sql* _p_sql_db = nullptr;
            std::unique_ptr<g3::LogWorker> _p_glw_worker = nullptr;

            bool _b_enable_balance;

    }; // class core

} // namespace rena::et::core

#endif // _EXPENSETRACKER_CORE_CORE_H_

#ifndef _EXPENSETRACKER_CORE_CONFIG_H_
#define _EXPENSETRACKER_CORE_CONFIG_H_

#include<filesystem>
#include<fstream>
#include<string>

#include"exception.h"

#define ERR_CONFIG_CFGFILE_CREATE_FAILED                    1
#define ERR_CONFIG_CFGFILE_READ_FAILED                      2
#define ERR_CONFIG_CFGFILE_FORMAT_ERROR                     3

namespace rena::et::core {

    DECLARE_CORE_EXCEPTION( config_exception );

    class config {

        public:
            config();
            ~config(){};

            static unsigned short BCFG_VERSION_MAJOR;
            static unsigned short BCFG_VERSION_MINOR;
            static unsigned short BCFG_VERSION_PATCH;
            static const char* BCFG_VERSION;
            static const char* BCFG_COMPILER_ID;
            static const char* BCFG_COMPILER_VERSION;
            static const char* BCFG_BUILD_SYS_NAME;
            static const char* BCFG_BUILD_GIT_COMMIT;
            static const char* BCFG_BUILD_GIT_BRANCH;
            static const char* BCFG_BUILD_TIME;

            static bool DEFAULT_ENABLE_BALANCE;

        protected:
            void _read_cfg( std::filesystem::path& __p_path );
            bool _write_default_cfg( std::filesystem::path& __p_path );

            static std::filesystem::path _get_cfg_folder();

        private:


    }; // class config

} // namespace rena::et::core

#endif // _EXPENSETRACKER_CORE_CONFIG_H_

#ifndef _EXPENSETRACKER_CORE_G3LOG_SINK_H_
#define _EXPENSETRACKER_CORE_G3LOG_SINK_H_

#include<filesystem>
#include<memory>
#include<string>

#include"exception.h"
#include"g3log/logmessage.hpp"

#define ERR_LOGSINK_OPEN_LOGFILE_FAILED                     1

namespace rena::et::core::utils {

    DECLARE_CORE_EXCEPTION( logsink_exception );

    class log_sink {

        public:
            log_sink( std::size_t __ull_refresh_after_x_msgs );
            ~log_sink();

            void write_log( g3::LogMessageMover __lmm_msg );

        protected:
            static std::filesystem::path _get_log_folder();

            inline std::ofstream& _filestream(){
                return *( this -> _p_ofs_wfile.get() );
            }

        private:
            g3::LogMessage::LogDetailsFunc _ldf_log_details_func;
            std::filesystem::path _p_logfile_path;
            std::unique_ptr<std::ofstream> _p_ofs_wfile;
            std::string _s_header;
            bool _b_first_entry;
            std::string _s_write_buf;
            size_t _ull_write_counter;
            size_t _ull_refresh_after_x_msgs;

            log_sink& operator=( const log_sink& ) = delete;
            log_sink( const log_sink& __ls_other ) = delete;

    }; // class log_sink

} // namespace rena::et::core::utils

#endif // _EXPENSETRACKER_CORE_G3LOG_SINK_H_

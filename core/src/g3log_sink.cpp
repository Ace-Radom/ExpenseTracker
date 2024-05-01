#include"g3log_sink.h"
#include"g3log/filesink.hpp"
#include"filesinkhelper.ipp"

#include<cassert>
#include<chrono>
#include<shlobj.h>
#include<sstream>
#include<windows.h>

#define LOG_PREFIX      "ExpenseTracker"
#define LOG_TIMEFORMAT  "%Y-%m-%d %H:%M:%S.%f3"

namespace fs = std::filesystem;
namespace utils = rena::et::core::utils;

utils::log_sink::log_sink( std::size_t __ull_refresh_after_x_msgs , unsigned int __ui_log_severity )
    : _ldf_log_details_func( &( this -> _log_details_to_string ) ) ,
      _p_logfile_path( this -> _get_log_folder() ) ,
      _p_ofs_wfile( new std::ofstream ) ,
      _ull_write_counter( 0 ) ,
      _ull_refresh_after_x_msgs( __ull_refresh_after_x_msgs ) ,
      _ls_log_severity( this -> _parse_log_severity( __ui_log_severity ) )
{
    if ( !fs::exists( this -> _p_logfile_path ) )
    {
        fs::create_directories( this -> _p_logfile_path );
    }
    std::string logfile_name = g3::internal::createLogFileName( LOG_PREFIX , "" );
    this -> _p_logfile_path /= logfile_name;
    this -> _p_ofs_wfile -> open( this -> _p_logfile_path , std::ios::out | std::ios::trunc );
    if ( !( this -> _p_ofs_wfile -> is_open() ) )
    {
        this -> _p_ofs_wfile.reset();
        throw logsink_exception( ERR_LOGSINK_OPEN_LOGFILE_FAILED , strerror( errno ) );
    }
    return;
}

utils::log_sink::~log_sink(){
    std::ostringstream oss;
    oss << "\nlogger sink shutdown at: ";
    auto now = std::chrono::system_clock::now();
    oss << g3::localtime_formatted( now , LOG_TIMEFORMAT ) << "\n";
    this -> _filestream() << this -> _oss_write_buf.str() << oss.str() << std::flush;
    // write exit msg to log
    return;
}

void utils::log_sink::write_log( g3::LogMessageMover __lmm_msg ){
    if ( __lmm_msg.get()._level.value < this -> _ls_log_severity.value )
    {
        return;
    }
    std::string data = __lmm_msg.get().toString( this -> _ldf_log_details_func );
    this -> _oss_write_buf << data;
    this -> _ull_write_counter++;
    if ( this -> _ull_write_counter % this -> _ull_refresh_after_x_msgs == 0 )
    {
        this -> _filestream() << this -> _oss_write_buf.str() << std::flush;
        this -> _oss_write_buf.clear();
    }
    return;
}

fs::path utils::log_sink::_get_log_folder(){
    TCHAR buf[MAX_PATH];
    SHGetSpecialFolderPath( NULL , buf , CSIDL_LOCAL_APPDATA , false );
    fs::path log_folder( buf );
    log_folder /= "ExpenseTracker";
    log_folder /= "log";
    return log_folder;
}

std::string utils::log_sink::_log_details_to_string( const g3::LogMessage& __lm_msg ){
    std::ostringstream oss;
    oss << "[" << __lm_msg.timestamp( LOG_TIMEFORMAT ) << "] " << __lm_msg.level() << "\t\t(" << __lm_msg.threadID() << ": " << __lm_msg.file() << " -> " << __lm_msg.function() << ":" << __lm_msg.line() << ")\t\t\t\t";
    return oss.str();
}

LEVELS utils::log_sink::_parse_log_severity( unsigned int __ui_log_severity ){
    switch ( __ui_log_severity )
    {
        case 0: return DEBUG;   break;
        case 1: return INFO;    break;
        case 2: return WARNING; break;
        case 3: return FATAL;   break;
        default : return INFO;  break;
    }
    return INFO;
}

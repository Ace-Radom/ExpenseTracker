#include"g3log_sink.h"
#include"g3log/filesink.hpp"
#include"filesinkhelper.ipp"

#include<cassert>
#include<chrono>
#include<shlobj.h>
#include<sstream>
#include<windows.h>

#define LOG_PREFIX "ExpenseTracker"

namespace fs = std::filesystem;
namespace utils = rena::et::core::utils;

utils::log_sink::log_sink( std::size_t __ull_refresh_after_x_msgs )
    : _ldf_log_details_func( &g3::LogMessage::DefaultLogDetailsToString ) ,
      _p_logfile_path( this -> _get_log_folder() ) ,
      _p_ofs_wfile( new std::ofstream ) ,
      _s_header( "\t\tLOG format: [YYYY/MM/DD hh:mm:ss uuu* LEVEL FILE->FUNCTION:LINE] message\n\n" ) ,
      _b_first_entry( true ) ,
      _ull_write_counter( 0 ) ,
      _ull_refresh_after_x_msgs( __ull_refresh_after_x_msgs )
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
    oss << g3::localtime_formatted( now , g3::internal::time_formatted ) << "\n";
    this -> _filestream() << this -> _s_write_buf << oss.str() << std::flush;
    // write exit msg to log
    return;
}

void utils::log_sink::write_log( g3::LogMessageMover __lmm_msg ){
    if ( this -> _b_first_entry )
    {
        this -> _filestream() << g3::internal::header( this -> _s_header );
        this -> _b_first_entry = false;
    }
    std::string data = __lmm_msg.get().toString( this -> _ldf_log_details_func );
    this -> _s_write_buf.append( data );
    this -> _ull_write_counter++;
    if ( this -> _ull_write_counter % this -> _ull_refresh_after_x_msgs == 0 )
    {
        this -> _filestream() << this -> _s_write_buf << std::flush;
        this -> _s_write_buf.clear();
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

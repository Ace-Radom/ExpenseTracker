#include"core.h"

#include<algorithm>
#include<cctype>
#include<shlobj.h>
#include<sstream>
#include<windows.h>

namespace core = rena::et::core;
namespace fs = std::filesystem;

core::core::~core(){
    if ( this -> _p_sql_db )
    {
        this -> close();
    }
    return;
}

void core::core::open( const std::string& __s_name ){
    this -> _open_db( __s_name , false );   
}

void core::core::create( const std::string& __s_name ){
    this -> _open_db( __s_name , true );
}

void core::core::close(){
    if ( this -> _p_sql_db )
    {
        delete this -> _p_sql_db;
        this -> _p_sql_db = nullptr;
    }
    return;
}

void core::core::_open_db( const std::string& __s_name , bool __b_create ){
    if ( this -> _p_sql_db )
    {
        throw core_exception( ERR_CORE_DB_REOPEN , "one database has already been opened" );
    } // reopen

    if ( !( this -> _check_db_name_legality( __s_name ) ) )
    {
        std::ostringstream oss;
        oss << "illegal database name [name=\"" << __s_name << "\"]"; 
        throw core_exception( ERR_CORE_ILLEGAL_DB_NAME , oss.str() );
    } // illegal db name

    fs::path db_path = this -> _get_db_folder() / ( __s_name + ".db" );
    if ( fs::exists( db_path ) == __b_create )
    {
        std::ostringstream oss;
        oss << "database " << ( __b_create ? "exists" : "not found" ) << " [path=\"" << db_path.string() << "\"]";
        throw core_exception( __b_create ? ERR_CORE_CREATE_EXISTING_DB : ERR_CORE_DB_NOT_FOUND , oss.str() );
    }
    this -> _p_sql_db = new utils::sql( db_path );
    return;
}

fs::path core::core::_get_db_folder() const {
    TCHAR buf[MAX_PATH];
    SHGetSpecialFolderPath( NULL , buf , CSIDL_LOCAL_APPDATA , false );
    fs::path db_folder( buf );
    db_folder /= "ExpenseTracker";
    return db_folder;
}

bool core::core::_check_db_name_legality( const std::string& __s_name ){
    return std::count_if( __s_name.begin() , __s_name.end() , []( unsigned char c ) -> bool {
        return !( std::isalnum( c ) || c == '_' );
    } ) == 0;
}

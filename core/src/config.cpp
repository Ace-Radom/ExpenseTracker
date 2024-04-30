#include"config.h"
#include"build_cfgs.h"
#include"mini.h"

#include<shlobj.h>
#include<windows.h>

namespace core = rena::et::core;
namespace fs = std::filesystem;

core::config::config(){
    this -> BCFG_VERSION_MAJOR = BUILD_CFG_VERSION_MAJOR;
    this -> BCFG_VERSION_MINOR = BUILD_CFG_VERSION_MINOR;
    this -> BCFG_VERSION_PATCH = BUILD_CFG_VERSION_PATCH;
    this -> BCFG_VERSION = BUILD_CFG_VERSION;
    this -> BCFG_COMPILER_ID = BUILD_CFG_COMPILER_ID;
    this -> BCFG_COMPILER_VERSION = BUILD_CFG_COMPILER_VERSION;
    this -> BCFG_BUILD_SYS_NAME = BUILD_CFG_BUILD_SYS_NAME;
    this -> BCFG_BUILD_GIT_COMMIT = BUILD_CFG_BUILD_GIT_COMMIT;
    this -> BCFG_BUILD_GIT_BRANCH = BUILD_CFG_BUILD_GIT_BRANCH;
    this -> BCFG_BUILD_TIME = BUILD_CFG_BUILD_TIME;

    fs::path cfg_path = this -> _get_cfg_folder();
    if ( !fs::exists( cfg_path ) )
    {
        fs::create_directories( cfg_path );
    } // cfg folder doesn't exist
    cfg_path /= "config.ini";
    if ( !fs::exists( cfg_path ) )
    {
        this -> DEFAULT_ENABLE_BALANCE = false;
        if ( !( this -> _write_default_cfg( cfg_path ) ) )
        {
            throw config_exception( ERR_CONFIG_CFGFILE_CREATE_FAILED , "failed to create config" );
        }
    } // cfg doesn't exist
    else
    {
        this -> _read_cfg( cfg_path );
    }
}

unsigned short core::config::BCFG_VERSION_MAJOR;
unsigned short core::config::BCFG_VERSION_MINOR;
unsigned short core::config::BCFG_VERSION_PATCH;
const char* core::config::BCFG_VERSION;
const char* core::config::BCFG_COMPILER_ID;
const char* core::config::BCFG_COMPILER_VERSION;
const char* core::config::BCFG_BUILD_SYS_NAME;
const char* core::config::BCFG_BUILD_GIT_COMMIT;
const char* core::config::BCFG_BUILD_GIT_BRANCH;
const char* core::config::BCFG_BUILD_TIME;

bool core::config::DEFAULT_ENABLE_BALANCE;

void core::config::_read_cfg( fs::path& __p_path ){
    mINI::INIFile inif( __p_path );
    mINI::INIStructure ini;
    if ( !inif.read( ini ) )
    {
        throw config_exception( ERR_CONFIG_CFGFILE_READ_FAILED , "failed to read config" );
    }
    if ( !ini.has( "expensetracker" ) )
    {
        throw config_exception( ERR_CONFIG_CFGFILE_FORMAT_ERROR , "config file doesn't have section \"expensetracker\"" );
    }
    if ( !ini["expensetracker"].has( "default_enable_balance" ) )
    {
        throw config_exception( ERR_CONFIG_CFGFILE_FORMAT_ERROR , "config file section \"expensetracker\" doesn't have node \"default_enable_balance\"" );
    }
    this -> DEFAULT_ENABLE_BALANCE = ini["expensetracker"]["default_enable_balance"] != "0";
    return;
}

bool core::config::_write_default_cfg( fs::path& __p_path ){
    mINI::INIFile inif( __p_path );
    mINI::INIStructure ini;
    ini["expensetracker"]["default_enable_balance"] = "0";
    return inif.generate( ini );
}

fs::path core::config::_get_cfg_folder(){
    TCHAR buf[MAX_PATH];
    SHGetSpecialFolderPath( NULL , buf , CSIDL_LOCAL_APPDATA , false );
    fs::path cfg_folder( buf );
    cfg_folder /= "ExpenseTracker";
    cfg_folder /= "cfg";
    return cfg_folder;
}

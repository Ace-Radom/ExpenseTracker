#include"cexchange/exchangerate_api.h"
#include"log.h"
#include"mini.h"

#include<shlobj.h>
#include<windows.h>

namespace cexchange = rena::et::core::utils::cexchange;
namespace fs = std::filesystem;

cexchange::ExchangeRate_API::ExchangeRate_API()
    : ExchangeRate_API_OAE( "ExchangeRate_API" , "conversion_rates" )
{
    LOG_E( INFO ) << "using ExchangeRate_API for currency exchange";

    fs::path cfg_path = this -> _get_cfg_path();
    if ( !fs::exists( cfg_path.parent_path() ) )
    {
        fs::create_directories( cfg_path.parent_path() );
    } // cfg folder doesn't exist (although it should have already been created by core config)

    if ( !fs::exists( cfg_path ) )
    {
        LOG_E( WARNING ) << "using ExchangeRate_API for currency exchange but its cfg file not found, create a new one: path: " << cfg_path;
        if ( !( this -> _write_default_cfg( cfg_path ) ) )
        {
            LOG_E( WARNING ) << "failed to create new ExchangeRate_API cfg";
            throw exchangerate_api_exception( ERR_EXCHANGERATE_API_CFG_CREATE_FAILED , "failed to create new cfg" );
        } // create new cfg failed
        throw exchangerate_api_exception( ERR_EXCHANGERATE_API_CFG_NOT_FOUND , "ExchangeRate API cfg not found, a new one has been created" );
    } // cfg not found, create one

    mINI::INIFile inif( cfg_path );
    mINI::INIStructure ini;
    if ( !inif.read( ini ) )
    {
        LOG_E( WARNING ) << "failed to read ExchangeRate_API cfg: path: " << cfg_path;
        throw exchangerate_api_exception( ERR_EXCHANGERATE_API_CFG_READ_FAILED , "read ExchangeRate API cfg failed" );
    }
    if ( !ini.has( "ExchangeRate_API") )
    {
        LOG_E( WARNING ) << "\"ExchangeRate_API\" section not found in ExchangeRate_API cfg: path: " << cfg_path;
        throw exchangerate_api_exception( ERR_EXCHANGERATE_API_CFG_FORMAT_ERROR , "cfg file doesn't have section \"ExchangeRate_API\"" );
    }
    if ( !ini["ExchangeRate_API"].has( "apikey" ) )
    {
        LOG_E( WARNING ) << "\"apikey\" node not found under section \"ExchangeRate_API\" in ExchangeRate_API cfg: path: " << cfg_path;
        throw exchangerate_api_exception( ERR_EXCHANGERATE_API_CFG_FORMAT_ERROR , "cfg file doesn't have node \"apikey\" under section \"ExchangeRate_API\"" );
    }
    this -> _s_apikey = ini["ExchangeRate_API"]["apikey"];
    if ( this -> _s_apikey.empty() )
    {
        LOG_E( WARNING ) << "empty apikey got from cfg";
        // here: do not throw and let core try to read cache
    }
    else
    {
        LOG_E( INFO ) << "apikey got from cfg";
    }
    return;
}

std::string cexchange::ExchangeRate_API::_gen_api_url( cexchange::basic_cexchange::currency_t __sc_base ) const {
    if ( this -> _s_apikey.empty() )
    {
        return "";
    }

    std::ostringstream oss;
    oss << "https://v6.exchangerate-api.com/v6/" << this -> _s_apikey << "/latest/" << this -> get_currency_data( __sc_base ).code;
    return oss.str();
}

bool cexchange::ExchangeRate_API::_write_default_cfg( const fs::path& __p_path ){
    mINI::INIFile inif( __p_path );
    mINI::INIStructure ini;
    ini["ExchangeRate_API"]["apikey"] = "";
    return inif.generate( ini );
}

fs::path cexchange::ExchangeRate_API::_get_cfg_path(){
    TCHAR buf[MAX_PATH];
    SHGetSpecialFolderPath( NULL , buf , CSIDL_LOCAL_APPDATA , false );
    fs::path cfg_path( buf );
    cfg_path /= "ExpenseTracker";
    cfg_path /= "cfg";
    cfg_path /= "ExchangeRate_API.ini";
    return cfg_path;
}

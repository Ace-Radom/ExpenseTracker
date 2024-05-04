#include"cexchange/exchangerate_api_oae.h"
#include"curl_callbacks.h"
#include"curl_control.h"
#include"log.h"

#include<curl/curl.h>

#include<algorithm>
#include<chrono>
#include<cstring>
#include<fstream>
#include<sstream>

#define CURL_SETOPT_IF_FAILED_GOTO_CLEANUP( curl , curlcode , curloption , data , errnum , errmsg ) \
    curlcode = curl_easy_setopt( curl , curloption , data );                                        \
    if ( curlcode != CURLE_OK )                                                                     \
    {                                                                                               \
        std::ostringstream oss;                                                                     \
        oss << "curl failed to set " << #curloption << ": " << curl_easy_strerror( curlcode );      \
        errnum = ERR_EXCHANGERATE_API_CURL_SETOPT_FAILED;                                           \
        errmsg = oss.str();                                                                         \
        LOG_E( WARNING ) << oss.str();                                                              \
        goto CURL_CLEANUP_SIGN;                                                                     \
    }

namespace cexchange = rena::et::core::utils::cexchange;
namespace fs = std::filesystem;

cexchange::ExchangeRate_API_OAE::ExchangeRate_API_OAE()
    : basic_cexchange( "ExchangeRate_API_OAE" ) ,
      _s_conversion_rates_key_name( "rates" )
{
    LOG_E( INFO ) << "using Exchange_Rate_API_OAE for currency exchange";
}

cexchange::ExchangeRate_API_OAE::ExchangeRate_API_OAE( const std::string& __s_api_name , const std::string& __s_conversion_rates_key_name )
    : basic_cexchange( __s_api_name ) ,
      _s_conversion_rates_key_name( __s_conversion_rates_key_name ){}

double cexchange::ExchangeRate_API_OAE::exchange( cexchange::basic_cexchange::currency_t __sc_from , cexchange::basic_cexchange::currency_t __sc_to ){
    LOG_E( INFO ) << "getting currency exchange rate: from: \"" << this -> get_currency_data( __sc_from ).code << "\", to: \"" << this -> get_currency_data( __sc_to ).code << "\"";
    
    double rate = this -> _try_read_cache( __sc_from , __sc_to );
    if ( rate > 0 )
    {
        LOG_E( INFO ) << "rate got from cache: rate: " << rate;
        return rate;
    }
    else
    {
        LOG_E( INFO ) << "unable to get rate from cache, asking ExchangeRate API remote and refreshing cache...";
        try {
            rate = this -> _try_ask_remote( __sc_from , __sc_to );
            LOG_E( INFO ) << "rate got from remote: rate: " << rate;
            return rate;
        }
        catch ( const exchangerate_api_exception& e )
        {
            LOG_E( WARNING ) << "exception caught by calling _try_ask_remote, retry to read cache to get old datas: errmsg: \"" << e.what() << "\"";
            rate = this -> _try_read_cache( __sc_from , __sc_to , true );
            if ( rate > 0 )
            {
                LOG_E( INFO ) << "got outdated rate from cache: rate: " << rate;
            }
            else
            {
                LOG_E( WARNING ) << "failed to get outdated rate from cache";
            }
            return rate;
        }
    }
    
    return -1;
}

bool cexchange::ExchangeRate_API_OAE::is_supported( cexchange::basic_cexchange::currency_t __sc_currency_num ) const noexcept {
    return std::find( this -> _a_supported_currency_list.begin() , this -> _a_supported_currency_list.end() , __sc_currency_num ) != this -> _a_supported_currency_list.end();
}

const std::array<cexchange::basic_cexchange::currency_t,154> cexchange::ExchangeRate_API_OAE::_a_supported_currency_list = {
    AED , AFN , ALL , AMD , ANG , AOA , ARS , AUD , AWG , AZN , BAM , BBD , BDT , BGN ,
    BHD , BIF , BMD , BND , BOB , BRL , BSD , BTN , BWP , BYN , BZD , CAD , CDF , CHF ,
    CLP , CNY , COP , CRC , CUP , CVE , CZK , DJF , DKK , DOP , DZD , EGP , ERN , ETB ,
    EUR , FJD , FKP , GBP , GEL , GHS , GIP , GMD , GNF , GTQ , GYD , HKD , HNL , HTG ,
    HUF , IDR , ILS , INR , IQD , IRR , ISK , JMD , JOD , JPY , KES , KGS , KHR , KMF ,
    KRW , KWD , KYD , KZT , LAK , LBP , LKR , LRD , LSL , LYD , MAD , MDL , MGA , MKD ,
    MMK , MNT , MOP , MRU , MUR , MVR , MWK , MXN , MYR , MZN , NAD , NGN , NIO , NOK ,
    NPR , NZD , OMR , PAB , PEN , PGK , PHP , PKR , PLN , PYG , QAR , RON , RSD , RUB ,
    RWF , SAR , SBD , SCR , SDG , SEK , SGD , SHP , SLE , SOS , SRD , SSP , STN , SYP ,
    SZL , THB , TJS , TMT , TND , TOP , TRY , TTD , TWD , TZS , UAH , UGX , USD , UYU ,
    UZS , VES , VND , VUV , WST , XAF , XCD , XDR , XOF , XPF , YER , ZAR , ZMW , ZWL
};

std::string cexchange::ExchangeRate_API_OAE::_gen_api_url( cexchange::basic_cexchange::currency_t __sc_base ) const {
    std::ostringstream oss;
    oss << "https://open.er-api.com/v6/latest/" << this -> get_currency_data( __sc_base ).code;
    return oss.str();
}

double cexchange::ExchangeRate_API_OAE::_try_read_cache( cexchange::basic_cexchange::currency_t __sc_from , cexchange::basic_cexchange::currency_t __sc_to , bool __b_skip_outdated_check ){
    fs::path cache_path = this -> _get_cache_path( __sc_from );
    if ( !fs::exists( cache_path ) )
    {
        LOG_E( INFO ) << "cache file doesn't exist: path: " << cache_path;
        return -1;
    } // cache doesn't exist

    std::ifstream rFile( cache_path );
    if ( !rFile.is_open() )
    {
        LOG_E( WARNING ) << "open cache file failed: path: " << cache_path << " errmsg: \"" << strerror( errno ) << "\"";
        return -1;
    } // open cache file failed

    Json::Value root;
    Json::Reader reader;
    if ( !reader.parse( rFile , root ) )
    {
        LOG_E( WARNING ) << "read cache file failed due to json parse error: errmsg: \"" << reader.getFormattedErrorMessages() << "\"";
        return -1;
    } // json format error

    LOG_E( INFO ) << "cache file opened and parsed: path: " << cache_path;
    rFile.close();

    if ( !__b_skip_outdated_check )
    {
        LOG_E( INFO ) << "do cache outdated check";
        if ( !root.isMember( "time_next_update_ts_utc" ) )
        {
            LOG_E( WARNING ) << "cannot find \"time_next_update_ts_utc\" key in cache file";
            return -1;
        } // cannot get next update time
        time_t utc_next_update_ts = root["time_next_update_ts_utc"].asInt64();
        auto now = std::chrono::system_clock::now();
        auto now_ts = std::chrono::system_clock::to_time_t( now );
        time_t utc_now_ts = std::mktime( std::gmtime( &now_ts ) );
        if ( utc_now_ts > utc_next_update_ts )
        {
            LOG_E( INFO ) << "cache should be refreshed: utc_next_update_ts: " << utc_next_update_ts << " utc_now_ts: " << utc_now_ts;
            return -1;
        } // cache needs to refresh
        LOG_E( INFO ) << "cache is up to date";
    }
    else
    {
        LOG_E( INFO ) << "cache outdated check is skipped";
    }

    if ( !root.isMember( "conv_rates" ) )
    {
        LOG_E( WARNING ) << "cannot find \"conv_rates\" object in cache file";
        return -1; 
    } // conversion rates object doesn't exist
    Json::Value conv_rates = root["conv_rates"];
    std::string target_code = this -> get_currency_data( __sc_to ).code;
    if ( !conv_rates.isMember( target_code ) )
    {
        LOG_E( WARNING ) << "cannot find \"" << target_code << "\" key under \"conv_rates\" object";
        return -1;
    }
    return conv_rates[target_code].asDouble();
}

double cexchange::ExchangeRate_API_OAE::_try_ask_remote( cexchange::basic_cexchange::currency_t __sc_from , cexchange::basic_cexchange::currency_t __sc_to ){
    curl_control::init_if_necessary();
    CURL* curl = nullptr;
    CURLcode res;

    curl = curl_easy_init();
    if ( !curl )
    {
        LOG( WARNING ) << "curl init failed";
        throw exchangerate_api_exception( ERR_EXCHANGERATE_API_CURL_INIT_FAILED , "curl init failed" );
    }

    unsigned int errnum = 0;
    std::string errmsg;
    std::string content;
    std::string url = this -> _gen_api_url( __sc_from );
    if ( url.empty() )
    {
        errnum = ERR_EXCHANGERATE_API_EMPTY_URL;
        errmsg = "empty url generated";
        LOG_E( WARNING ) << "empty url generated";
        goto CURL_CLEANUP_SIGN;
    } // empty url (for ExchangeRate_API)
    CURL_SETOPT_IF_FAILED_GOTO_CLEANUP( curl , res , CURLOPT_HTTPGET , 1L , errnum , errmsg );
    CURL_SETOPT_IF_FAILED_GOTO_CLEANUP( curl , res , CURLOPT_URL , url.c_str() , errnum , errmsg );
    CURL_SETOPT_IF_FAILED_GOTO_CLEANUP( curl , res , CURLOPT_WRITEFUNCTION , &callbacks::curlcb_write_string , errnum , errmsg );
    CURL_SETOPT_IF_FAILED_GOTO_CLEANUP( curl , res , CURLOPT_WRITEDATA , &content , errnum , errmsg );
    CURL_SETOPT_IF_FAILED_GOTO_CLEANUP( curl , res , CURLOPT_TIMEOUT , 10 , errnum , errmsg );
    CURL_SETOPT_IF_FAILED_GOTO_CLEANUP( curl , res , CURLOPT_SSL_VERIFYPEER , false , errnum , errmsg );
    res = curl_easy_perform( curl );
    if ( res != CURLE_OK )
    {
        std::ostringstream oss;
        oss << "failed to connect to ExchangeRate API gateway: " << curl_easy_strerror( res );
        errnum = ERR_EXCHANGERATE_API_CURL_PERFORM_FAILED;
        errmsg = oss.str();
        LOG_E( WARNING ) << oss.str();
        goto CURL_CLEANUP_SIGN;
    }

CURL_CLEANUP_SIGN:
    if ( curl )
    {
        curl_easy_cleanup( curl );
    }
    if ( !errmsg.empty() && errnum != 0 )
    {
        throw exchangerate_api_exception( errnum , errmsg );
    } // throw exception after cleanup

    LOG_E( INFO ) << "got response from ExchangeRate API successfully";
    LOG_E( DEBUG ) << "remote responsed content: \"" << content << "\"";
    
    Json::Value root;
    Json::Reader reader;
    if ( !reader.parse( content , root ) )
    {
        std::ostringstream oss;
        oss << "parse ExchangeRate API response failed due to json format error: errmsg: \"" << reader.getFormattedErrorMessages() << "\", content: \"" << content << "\"";
        LOG_E( WARNING ) << oss.str();
        throw exchangerate_api_exception( ERR_EXCHANGERATE_API_JSON_FORMAT_ERROR , oss.str() );
    } // json format error (although it is highly unexpected)

    if ( !root.isMember( "result" ) )
    {
        LOG_E( WARNING ) << "cannot find \"result\" key in remote response: content: \"" << content << "\"";
        throw exchangerate_api_exception( ERR_EXCHANGERATE_API_REMOTE_RESPONSE_KEY_NOT_FOUND , "result key not found" );
    } // result key not found
    if ( root["result"].asString() != "success" )
    {
        if ( !root.isMember( "error-type" ) )
        {
            LOG_E( WARNING ) << "cannot find \"error-type\" key in remote response when \"result\" key is not \"success\": content: \"" << content << "\"";
            throw exchangerate_api_exception( ERR_EXCHANGERATE_API_REMOTE_RESPONSE_KEY_NOT_FOUND , "error-type key not found when result is not success" );
        } // error-type key not found
        throw exchangerate_api_exception( ERR_EXCHANGERATE_API_REMOTE_RESPONSED_ERROR , root["error-type"].asString() );
    } // remote responsed error

    if ( !root.isMember( "time_last_update_unix" ) )
    {
        LOG_E( WARNING ) << "cannot find \"time_last_update_unix\" key in remote response: content: \"" << content << "\"";
        throw exchangerate_api_exception( ERR_EXCHANGERATE_API_REMOTE_RESPONSE_KEY_NOT_FOUND , "time_last_update_unix key not found" );
    } // time_last_update_unix key not found
    if ( !root.isMember( "time_next_update_unix" ) )
    {
        LOG_E( WARNING ) << "cannot find \"time_next_update_unix\" key in remote response: content: \"" << content << "\"";
        throw exchangerate_api_exception( ERR_EXCHANGERATE_API_REMOTE_RESPONSE_KEY_NOT_FOUND , "time_next_update_unix key not found" );
    } // time_next_update_unix key not found
    if ( !root.isMember( this -> _s_conversion_rates_key_name ) )
    {
        LOG_E( WARNING ) << "cannot find \"" << this -> _s_conversion_rates_key_name << "\" key in remote response: content: \"" << content << "\"";
        throw exchangerate_api_exception( ERR_EXCHANGERATE_API_REMOTE_RESPONSE_KEY_NOT_FOUND , this -> _s_conversion_rates_key_name + " key not found" );
    } // conv rates key not found

    this -> _write_cache( __sc_from , root );
    // format verified, write to cache

    Json::Value conv_rates = root[this->_s_conversion_rates_key_name];
    std::string target_code = this -> get_currency_data( __sc_to ).code;
    if ( !conv_rates.isMember( target_code ) )
    {
        LOG_E( WARNING ) << "cannot find \"" << target_code << "\" key under \"" << this -> _s_conversion_rates_key_name << "\" object in remote response: content: \"" << content << "\"";
        throw exchangerate_api_exception( ERR_EXCHANGERATE_API_REMOTE_RESPONSE_KEY_NOT_FOUND , target_code + " key not found" );
    } // target code key not found under conv rates object

    return conv_rates[target_code].asDouble();
}

void cexchange::ExchangeRate_API_OAE::_write_cache( cexchange::basic_cexchange::currency_t __sc_base , const Json::Value& __jsonv_root ){
    fs::path cache_path = this -> _get_cache_path( __sc_base );
    if ( !fs::exists( cache_path.parent_path() ) )
    {
        fs::create_directories( cache_path.parent_path() );
    } // create cache dir if not exists

    std::ofstream wFile( cache_path );
    if ( !wFile.is_open() )
    {
        LOG_E( WARNING ) << "open cache file failed: path: " << cache_path << " errmsg: \"" << strerror( errno ) << "\"";
        return;
    } // open cache file failed

    LOG_E( INFO ) << "caching data to cache file: path: " << cache_path;

    Json::Value root;
    root["base_currency"] = this -> get_currency_data( __sc_base ).code;
    root["time_last_update_ts_utc"] = __jsonv_root["time_last_update_unix"];
    root["time_next_update_ts_utc"] = __jsonv_root["time_next_update_unix"];
    root["conv_rates"] = __jsonv_root[this->_s_conversion_rates_key_name];

    wFile << Json::FastWriter{}.write( root );
    wFile.close();

    return;
}

fs::path cexchange::ExchangeRate_API_OAE::_get_cache_path( cexchange::basic_cexchange::currency_t __sc_base ) const {
    std::ostringstream oss;
    oss << "cache_" << this -> get_currency_data( __sc_base ).code << ".json";
    // filename
    fs::path cache_path = this -> _get_cache_folder();
    cache_path /= "ExchangeRate_API";
    cache_path /= oss.str();
    return cache_path;
}

#include"core/cexchange/cexchange_reg.h"
#include"core/cexchange/exchangerate_api_oae.h"
#include"core/cexchange/exchangerate_api.h"
#include"core/log.h"

#include<algorithm>

namespace cexchange = rena::et::core::utils::cexchange;

cexchange::basic_cexchange* cexchange::cexchange_reg::resolve( const std::string& __s_name ){
    try {
        return _um_creation_reg.at( __s_name )();
    }
    catch ( const std::out_of_range& e )
    {
        LOG_E( WARNING ) << "cannot resolve cexchange api: name: \"" << __s_name << "\"";
        return nullptr;
    }
}

const std::unordered_map<std::string,std::function<cexchange::basic_cexchange*()>> cexchange::cexchange_reg::_um_creation_reg = {
    { "ExchangeRate_API_OAE" , [](){ return new ExchangeRate_API_OAE(); } } ,
    { "ExchangeRate_API" , [](){ return new ExchangeRate_API(); } }
};

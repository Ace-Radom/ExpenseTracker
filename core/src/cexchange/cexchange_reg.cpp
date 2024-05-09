#include"core/cexchange/cexchange_reg.h"
#include"core/cexchange/exchangerate_api_oae.h"
#include"core/cexchange/exchangerate_api.h"

#include<algorithm>

namespace cexchange = rena::et::core::utils::cexchange;

cexchange::basic_cexchange* cexchange::cexchange_reg::resolve( const std::string& __s_name ){
    auto pos = std::find_if( _vec_creation_reg.begin() , _vec_creation_reg.end() , [=]( const creation_reg_t& __scr_item ) -> bool {
        return __scr_item.name == __s_name;
    } );
    return ( pos == _vec_creation_reg.end() ) ? nullptr : pos -> f_new();
}

const std::vector<cexchange::cexchange_reg::creation_reg_t> cexchange::cexchange_reg::_vec_creation_reg = {
    { "ExchangeRate_API_OAE" , [](){ return new ExchangeRate_API_OAE(); } } ,
    { "ExchangeRate_API" , [](){ return new ExchangeRate_API(); } }
};

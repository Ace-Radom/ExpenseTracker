#include"core/curl_callbacks.h"

namespace cbs = rena::et::core::utils::callbacks;

int cbs::curlcb_write_string( char* __p_c_data , size_t __ull_size , size_t __ull_nmemb , void* __p_v_udata ){
    std::string* content = ( std::string* ) __p_v_udata;
    size_t realsize = __ull_size * __ull_nmemb;
    content -> append( __p_c_data );
    return realsize;
}

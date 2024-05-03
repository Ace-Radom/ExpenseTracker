#include"curl_control.h"

#include<curl/curl.h>

namespace utils = rena::et::core::utils;

void utils::curl_control::init_if_necessary(){
    if ( !_b_is_inited )
    {
        curl_global_init( CURL_GLOBAL_ALL );
        _b_is_inited = true;
    }
    return;
}

void utils::curl_control::cleanup_if_necessary(){
    if ( _b_is_inited )
    {
        curl_global_cleanup();
        _b_is_inited = false;
    }
    return;
}

bool utils::curl_control::_b_is_inited = false;

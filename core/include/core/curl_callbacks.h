#ifndef _EXPENSETRACKER_CORE_CURL_CALLBACKS_H_
#define _EXPENSETRACKER_CORE_CURL_CALLBACKS_H_

#include<string>

namespace rena::et::core::utils::callbacks {

    int curlcb_write_string( char* __p_c_data , size_t __ull_size , size_t __ull_nmemb , void* __p_v_udata );

} // namespace rena::et::core::utils::callbacks

#endif // _EXPENSETRACKER_CORE_CURL_CALLBACKS_H_

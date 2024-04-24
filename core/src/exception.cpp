#include"exception.h"

#include<sstream>

using namespace rena::et::core;

core_exception::core_exception( int __i_errno , const char* __p_c_errmsg ){
    std::ostringstream oss;
    oss << "[" << __i_errno << "] " << __p_c_errmsg;
    this -> _s_errmsg = oss.str();
    return;
}

core_exception::core_exception( int __i_errno , const std::string& __s_errmsg ){
    std::ostringstream oss;
    oss << "[" << __i_errno << "] " << __s_errmsg;
    this -> _s_errmsg = oss.str();
    return;
}

const char* core_exception::what() const noexcept {
    return this -> _s_errmsg.c_str();
}

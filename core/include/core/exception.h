#ifndef _EXPENSETRACKER_CORE_EXCEPTION_H_
#define _EXPENSETRACKER_CORE_EXCEPTION_H_

#include<exception>
#include<string>

#define DECLARE_CORE_EXCEPTION( sub_exception )                             \
    class sub_exception final : public rena::et::core::core_exception {     \
                                                                            \
        public:                                                             \
            sub_exception( int __i_errno , const char* __p_c_errmsg )       \
                : core_exception( __i_errno , __p_c_errmsg ){};             \
            sub_exception( int __i_errno , const std::string& __s_errmsg )  \
                : core_exception( __i_errno , __s_errmsg ){};               \
                                                                            \
    };

namespace rena::et::core {

    class core_exception : public std::exception {

        public:
            core_exception( int __i_errno , const char* __p_c_errmsg );
            core_exception( int __i_errno , const std::string& __s_errmsg );

            const char* what() const noexcept override;

        private:
            std::string _s_errmsg;

    }; // class core_exception

} // namespace rena::et::core

#endif // _EXPENSETRACKER_CORE_EXCEPTION_H_

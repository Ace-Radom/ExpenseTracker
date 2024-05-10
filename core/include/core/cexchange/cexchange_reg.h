#ifndef _EXPENSETRACKER_CORE_CEXCHANGE_REG_H_
#define _EXPENSETRACKER_CORE_CEXCHANGE_REG_H_

#include<functional>
#include<string>
#include<unordered_map>

#include"core/basic_cexchange.h"

namespace rena::et::core::utils::cexchange {

    class cexchange_reg {

        public:
            static basic_cexchange* resolve( const std::string& __s_name );

        private:
            static const std::unordered_map<std::string,std::function<basic_cexchange*()>> _um_creation_reg;

    }; // class cexchange_reg

} // namespace rena::et::core::utils::cexchange

#endif // _EXPENSETRACKER_CORE_CEXCHANGE_REG_H_

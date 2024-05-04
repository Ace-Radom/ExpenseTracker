#ifndef _EXPENSETRACKER_CORE_CEXCHANGE_REG_H_
#define _EXPENSETRACKER_CORE_CEXCHANGE_REG_H_

#include<functional>
#include<string>
#include<vector>

#include"basic_cexchange.h"

namespace rena::et::core::utils::cexchange {

    class cexchange_reg {

        public:
            static basic_cexchange* resolve( const std::string& __s_name );

        private:
            typedef struct {
                std::string name;
                std::function<basic_cexchange*()> f_new;
            } creation_reg_t;

            static const std::vector<creation_reg_t> _vec_creation_reg;

    }; // class cexchange_reg

} // namespace rena::et::core::utils::cexchange

#endif // _EXPENSETRACKER_CORE_CEXCHANGE_REG_H_

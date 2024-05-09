#ifndef _EXPENSETRACKER_CORE_CEXCHANGE_EXCHANGERATE_API_H_
#define _EXPENSETRACKER_CORE_CEXCHANGE_EXCHANGERATE_API_H_

#include<filesystem>

#include"core/cexchange/exchangerate_api_oae.h"

#define ERR_EXCHANGERATE_API_CFG_CREATE_FAILED             10
#define ERR_EXCHANGERATE_API_CFG_NOT_FOUND                 11
#define ERR_EXCHANGERATE_API_CFG_READ_FAILED               12
#define ERR_EXCHANGERATE_API_CFG_FORMAT_ERROR              13

namespace rena::et::core::utils::cexchange {

    class ExchangeRate_API final : public ExchangeRate_API_OAE {

        public:
            ExchangeRate_API();
            ~ExchangeRate_API(){};

        protected:
            std::string _gen_api_url( currency_t __sc_base ) const override;
            bool _write_default_cfg( const std::filesystem::path& __p_path );

            static std::filesystem::path _get_cfg_path();

        private:
            std::string _s_apikey;

    }; // class ExchangeRate_API

} // namespace rena::et::core::utils::cexchange

#endif // _EXPENSETRACKER_CORE_CEXCHANGE_EXCHANGERATE_API_H_

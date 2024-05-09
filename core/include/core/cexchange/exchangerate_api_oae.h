#ifndef _EXPENSETRACKER_CORE_CEXCHANGE_ExchangeRate_API_OAE_H_
#define _EXPENSETRACKER_CORE_CEXCHANGE_ExchangeRate_API_OAE_H_

#include<array>
#include<filesystem>

#include"core/basic_cexchange.h"
#include"core/exception.h"
#include"json/json.h"

#define ERR_EXCHANGERATE_API_CURL_INIT_FAILED                   1
#define ERR_EXCHANGERATE_API_CURL_SETOPT_FAILED                 2
#define ERR_EXCHANGERATE_API_CURL_PERFORM_FAILED                3
#define ERR_EXCHANGERATE_API_JSON_FORMAT_ERROR                  4
#define ERR_EXCHANGERATE_API_REMOTE_RESPONSE_KEY_NOT_FOUND      5
#define ERR_EXCHANGERATE_API_REMOTE_RESPONSED_ERROR             6
#define ERR_EXCHANGERATE_API_EMPTY_URL                          7

namespace rena::et::core::utils::cexchange {

    DECLARE_CORE_EXCEPTION( exchangerate_api_exception );

    class ExchangeRate_API_OAE : public basic_cexchange {

        public:
            ExchangeRate_API_OAE();
            ExchangeRate_API_OAE( const std::string& __s_api_name , const std::string& __s_conversion_rates_key_name );
            virtual ~ExchangeRate_API_OAE(){};

            double exchange( currency_t __sc_from , currency_t __sc_to ) override;
            bool is_supported( currency_t __sc_currency_num ) const noexcept override;

        protected:
            static const std::array<currency_t,154> _a_supported_currency_list;
            virtual std::string _gen_api_url( currency_t __sc_base ) const;
            double _try_read_cache( currency_t __sc_from , currency_t __sc_to , bool __b_skip_outdated_check = false );
            double _try_ask_remote( currency_t __sc_from , currency_t __sc_to );
            void _write_cache( currency_t __sc_base , const Json::Value& __jsonv_root );
            std::filesystem::path _get_cache_path( currency_t __sc_base ) const;

            std::string _s_conversion_rates_key_name;

    }; // class ExchangeRate_API_OAE

} // namespace rena::et::core::utils::cexchange

#endif // _EXPENSETRACKER_CORE_CEXCHANGE_ExchangeRate_API_OAE_H_

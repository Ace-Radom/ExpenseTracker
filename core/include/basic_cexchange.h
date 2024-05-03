#ifndef _EXPENSETRACKER_CORE_BASIC_CEXCHANGE_H_
#define _EXPENSETRACKER_CORE_BASIC_CEXCHANGE_H_

#include<string>

namespace rena::et::core::utils {

    class basic_cexchange {

        public:
            basic_cexchange(){};
            ~basic_cexchange(){};

            /**
             * Currency List, follows the Three Letter Currency Code defined in ISO 4217 as of 1 April 2022.
             */
            typedef enum {
                AED , AFN , ALL , AMD , ANG , AOA , ARS , AUD , AWG , AZN , BAM , BBD , BDT , BGN , BHD , BIF , BMD , BND , BOB , BOV ,
                BRL , BSD , BTN , BWP , BYN , BZD , CAD , CDF , CHE , CHF , CHW , CLF , CLP , CNY , COP , COU , CRC , CUP , CVE , CZK ,
                DJF , DKK , DOP , DZD , EGP , ERN , ETB , EUR , FJD , FKP , GBP , GEL , GHS , GIP , GMD , GNF , GTQ , GYD , HKD , HNL ,
                HTG , HUF , IDR , ILS , INR , IQD , IRR , ISK , JMD , JOD , JPY , KES , KGS , KHR , KMF , KPW , KRW , KWD , KYD , KZT ,
                LAK , LBP , LKR , LRD , LSL , LYD , MAD , MDL , MGA , MKD , MMK , MNT , MOP , MRU , MUR , MVR , MWK , MXN , MXV , MYR ,
                MZN , NAD , NGN , NIO , NOK , NPR , NZD , OMR , PAB , PEN , PGK , PHP , PKR , PLN , PYG , QAR , RON , RSD , RUB , RWF ,
                SAR , SBD , SCR , SDG , SEK , SGD , SHP , SLE , SLL , SOS , SRD , SSP , STN , SVC , SYP , SZL , THB , TJS , TMT , TND ,
                TOP , TRY , TTD , TWD , TZS , UAH , UGX , USD , USN , UYI , UYU , UYW , UZS , VED , VES , VND , VUV , WST , XAF , XAG ,
                XAU , XBA , XBB , XBC , XBD , XCD , XDR , XOF , XPD , XPF , XPT , XSU , XTS , XUA , XXX , YER , ZAR , ZMW , ZWL , NUL
            } currency_t;

            typedef struct {
                currency_t num;
                std::string code;
                std::string name;
            } currency_dat_t;

            static currency_dat_t get_currency_data( currency_t __sc_currency_num );
            virtual double exchange( currency_t __sc_from , currency_t __sc_to ) = 0;
            virtual bool is_supported( currency_t __sc_currency_num ) = 0;

    }; // class basic_cexchange

} // namespace rena::et::core::utils

#endif // _EXPENSETRACKER_CORE_BASIC_CEXCHANGE_H_

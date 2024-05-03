#ifndef _EXPENSETRACKER_CORE_SQL_TYPES_H_
#define _EXPENSETRACKER_CORE_SQL_TYPES_H_

#include<string>

namespace rena::et::core::utils {

    typedef struct {
        unsigned int format_version;
        std::string et_version;
        std::string et_commit;
        std::string et_branch;
        time_t first_create_time;
    } format_dat_t;

    typedef struct {
        std::string calibration_code;
    } calibration_dat_t;

    typedef struct {
        std::string name;
        unsigned long create_time;
        std::string description;
        std::string owners;
        bool enable_balance;
    } header_dat_t;

    typedef struct {
        struct {
            unsigned long time_stamp;
            unsigned short time_zone;
        } data_record_time;
        struct {
            double payment;
            unsigned short currency_type;
            std::string description;
        } main_data_body;
        struct {
            double currency_exchange;
            double total_payment_after_exchange;
            double balance;
        } pre_calculated_datas;
    } data_dat_t;

} // namespace rena::et::core::utils

#endif // _EXPENSETRACKER_CORE_SQL_TYPES_H_

#ifndef _EXPENSETRACKER_CORE_CURL_CONTROL_H_
#define _EXPENSETRACKER_CORE_CURL_CONTROL_H_

#define CURL_CLEANUP_SIGN curl_cleanup

namespace rena::et::core::utils {

    class curl_control {

        public:
            static void init_if_necessary();
            static void cleanup_if_necessary();

        private:
            static bool _b_is_inited;

    }; // class curl_control

} // namespace rena::et::core::utils

#endif // _EXPENSETRACKER_CORE_CURL_CONTROL_H_

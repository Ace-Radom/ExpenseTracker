#ifndef _EXPENSETRACKER_CORE_STRCONV_H_
#define _EXPENSETRACKER_CORE_STRCONV_H_

#include<filesystem>
#include<string>

namespace rena::et::core::utils {

    std::string conv_path_to_ansistr( const std::filesystem::path& __p_path );

} // namespace rena::et::core::utils

#endif // _EXPENSETRACKER_CORE_STRCONV_H_

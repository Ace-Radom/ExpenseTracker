#include"strconv.h"

#include<windows.h>

namespace fs = std::filesystem;
namespace utils = rena::et::core::utils;

std::string utils::conv_path_to_ansistr( const fs::path& __p_path ){
    std::wstring wstr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes( __p_path.string() );
    int required_size = WideCharToMultiByte(
        CP_ACP ,
        0 ,
        wstr.c_str() ,
        -1 ,
        NULL ,
        0 ,
        NULL ,
        NULL
    );
    std::string ansistr( required_size , '\0' );
    WideCharToMultiByte(
        CP_ACP ,
        0 ,
        wstr.c_str() ,
        -1 ,
        &ansistr[0] ,
        required_size ,
        NULL ,
        NULL
    );
    return ansistr;
}

#ifndef _EXPENSETRACKER_CORE_LOG_H_
#define _EXPENSETRACKER_CORE_LOG_H_

#include"core/config.h"
#include"g3log/g3log.hpp"

#define LOG_E( level )  \
    if ( !rena::et::core::config::ENABLE_LOG ) {} else LOG( level )

#define LOG_IF_E( level , boolean_expression )  \
    if ( !rena::et::core::config::ENABLE_LOG ) {} else LOG_IF( level , boolean_expression )

#define CHECK_E( boolean_expression )   \
    if ( !rena::et::core::config::ENABLE_LOG ) {} else CHECK( boolean_expression )

#endif // _EXPENSETRACKER_CORE_LOG_H_

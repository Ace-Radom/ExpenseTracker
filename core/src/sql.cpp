#include"sql.h"
#include"sql_callbacks.h"

#include<assert.h>
#include<sstream>

namespace fs = std::filesystem;
namespace utils = rena::et::core::utils;

/**
 * The header table of ExpenseTracker in each database should be kept in the following form:
 *   ┌──────────┬───────────────────┬─────────────┬──────────┬────────────────┐
 *   │   Name   │ Create Time (UTC) │ Description │  Owners  │ Enable Balance │
 *   ├──────────┼───────────────────┼─────────────┼──────────┼────────────────┤
 *   │   TEXT   │        INT        │    TEXT     │   TEXT   │      INT       │
 *   └──────────┴───────────────────┴─────────────┴──────────┴────────────────┘
 *
 * The data table of ExpenseTracker should be kept in the following form:
 *   ┌───────────────────┬────────────────────────┬───────────────────────────────────────┬─────────────────────────────────────────────────────────────────────────┐
 *   │        ID         │    Data Record Time    │            Main Data Body             │                          Pre-calculated Datas                           |
 *   ├───────────────────┼────────────┬───────────┼─────────┬───────────────┬─────────────┼───────────────────┬──────────────────────────────┬──────────────────────┤
 *   │                   │ Time Stamp │ Time Zone │ Payment │ Currency Type │ Description │ Currency Exchange | Total Payment after Exchange | Balance (if enabled) |
 *   ├───────────────────┼────────────┼───────────┼─────────┼───────────────┼─────────────┼───────────────────┼──────────────────────────────┼──────────────────────┤
 *   │  INT PRIMARY KEY  │    INT     │    INT    │  REAL   │      INT      │    TEXT     │       REAL        │             REAL             │         REAL         │
 *   └───────────────────┴────────────┴───────────┴─────────┴───────────────┴─────────────┴───────────────────┴──────────────────────────────┴──────────────────────┘
 */

/**
 * Open a database and check its legality. If it doesn't exist, create it.
 */
utils::sql::sql( fs::path __p_dbpath ){
    this -> _p_dbpath = __p_dbpath;
    bool is_new = !fs::exists( this -> _p_dbpath );
    int rc = sqlite3_open( this -> _p_dbpath.string().c_str() , &( this -> _p_sql3_dbobj ) );
    if ( rc != SQLITE_OK )
    {
        throw sql_exception( ERR_SQL_NOT_OPEN , sqlite3_errmsg( this -> _p_sql3_dbobj ) );
    }
    if ( is_new )
    {
        this -> _create_tables();
    }
    else
    {
        std::string errmsg;
        if ( !( this -> _check_db_legality( &errmsg ) ) )
        {
            throw sql_exception( ERR_SQL_ILLEGAL_DB , errmsg );
        }
    }
    return;
}

utils::sql::~sql(){
    sqlite3_close( this -> _p_sql3_dbobj );
    return;
}

/**
 * Write database header table datas.
 * Count header length before calling this function to make sure that datas won't be overwritten accidentally!!!
 */
void utils::sql::write_header( const utils::header_dat_t& __shd_data ){
    if ( this -> _get_table_len( HEADER ) > 0 )
    {
        this -> _exec_sqlcmd(
            R"(DELETE FROM HEADER;)" ,
            ERR_SQL_CLEAN_UP_TABLE_FAILED
        );
    } // if header already contains datas, erase them (count header before write it!!!)
    std::ostringstream oss;
    oss << "INSERT INTO HEADER VALUES ('" << __shd_data.name << "',"
                                          << __shd_data.create_time << ",'"
                                          << __shd_data.description << "','"
                                          << __shd_data.owners << "',"
                                          << ( __shd_data.enable_balance ? "1" : "0" ) << ");";
    this -> _exec_sqlcmd(
        oss.str() ,
        ERR_SQL_INSERT_HEADER_TABLE_FAILED
    );
    return;
}

/**
 * Write database data table datas to the end of the table.
 */
void utils::sql::write_data( const utils::data_dat_t& __sdd_data ){
    unsigned int id = this -> _get_table_len( DATA );
    // index of this data
    std::ostringstream oss;
    oss << "INSERT INTO DATA VALUES (" << id << ","
                                       << __sdd_data.data_record_time.time_stamp << ","
                                       << __sdd_data.data_record_time.time_zone << ","
                                       << __sdd_data.main_data_body.payment << ","
                                       << __sdd_data.main_data_body.currency_type << ",'"
                                       << __sdd_data.main_data_body.description << "',"
                                       << __sdd_data.pre_calculated_datas.currency_exchange << ","
                                       << __sdd_data.pre_calculated_datas.total_payment_after_exchange << ","
                                       << __sdd_data.pre_calculated_datas.balance << ");";
    this -> _exec_sqlcmd(
        oss.str() ,
        ERR_SQL_INSERT_DATA_TABLE_FAILED
    );
    return;
}

/**
 * Get database header table data.
 * Remember to delete the data you get!!!
 */
const utils::header_dat_t* utils::sql::get_header(){
    assert( this -> _get_table_len( HEADER ) == 1 );

    header_dat_t* data = new header_dat_t;
    this -> _exec_sqlcmd(
        R"(SELECT * FROM HEADER;)" ,
        &callbacks::sqlcb_get_header_data ,
        ( void* ) data ,
        ERR_SQL_GET_HEADER_DATA_FAILED
    );
    return data;
}

/**
 * Get database data table datas.
 * If __i_id == -1, all datas in the table will be returned. Otherwise it will only get the data by index. If it doesn't exist, null will be returned.
 * Remember to delete the data you get!!!
 */
const std::vector<utils::data_dat_t>* utils::sql::get_data( int __i_id ){
    unsigned int len = this -> _get_table_len( DATA );
    if ( __i_id >= len && __i_id != -1 )
    {
        return nullptr;
    }

    std::vector<data_dat_t>* data = new std::vector<data_dat_t>;
    if ( __i_id == -1 )
    {
        this -> _exec_sqlcmd(
            R"(SELECT * FROM DATA ORDER BY ID ASC;)" ,
            &callbacks::sqlcb_get_data_data ,
            ( void* ) data ,
            ERR_SQL_GET_DATA_DATA_FAILED
        );
    }
    else
    {
        std::ostringstream oss;
        oss << "SELECT * FROM DATA WHERE ID=" << __i_id << ";";
        this -> _exec_sqlcmd(
            oss.str() ,
            &callbacks::sqlcb_get_data_data ,
            ( void* ) data ,
            ERR_SQL_GET_DATA_DATA_FAILED
        );
    }
    return data;
}

unsigned int utils::sql::get_header_len(){
    return this -> _get_table_len( HEADER );
}

unsigned int utils::sql::get_data_len(){
    return this -> _get_table_len( DATA );
}

/**
 * Create tables in database.
 */
void utils::sql::_create_tables(){
    this -> _exec_sqlcmd( 
        R"(
CREATE TABLE HEADER(
    NAME            TEXT    NOT NULL,
    CREATE_TIME     INT     NOT NULL,
    DESCRIPTION     TEXT,
    OWNERS          TEXT,
    ENABLE_BALANCE  INT     NOT NULL
);
)" ,
        ERR_SQL_CREATE_HEADER_TABLE_FAILED
    );
    this -> _exec_sqlcmd( 
        R"(
CREATE TABLE DATA(
    ID                              INT PRIMARY KEY     NOT NULL,
    TIME_STAMP                      INT                 NOT NULL,
    TIME_ZONE                       INT                 NOT NULL,
    PAYMENT                         REAL                NOT NULL,
    CURRENCY_TYPE                   INT                 NOT NULL,
    DESCRIPTION                     TEXT,
    CURRENCY_EXCHANGE               TEXT                NOT NULL,
    TOTAL_PAYMENT_AFTER_EXCHANGE    REAL                NOT NULL,
    BALANCE                         REAL
);
)" ,
        ERR_SQL_CREATE_DATA_TABLE_FAILED
    );
    return;
}

/**
 * Check database legality / format. (table list, header table cols, data table cols)
 */
bool utils::sql::_check_db_legality( std::string* __out_p_s_errmsg ){
    assert( __out_p_s_errmsg );

    callbacks::table_checklist_t table_checklist = { 0 , 0 , 0 };
    this -> _exec_sqlcmd( 
        R"(SELECT name FROM sqlite_master WHERE type='table';)" ,
        &callbacks::sqlcb_check_table_list ,
        ( void* ) &table_checklist ,
        ERR_SQL_CHECK_DB_CMD_EXEC_FAILED
    );
    if ( !( table_checklist.header == 1 &&
            table_checklist.data == 1   &&
            table_checklist.unknown == 0 ) )
    {
        std::ostringstream oss;
        oss << "illegal database table list [.header=" << table_checklist.header
            << ", .data=" << table_checklist.data
            << ", .unknown=" << table_checklist.unknown << "]";
        *__out_p_s_errmsg = oss.str();
        return false;
    } // illegal table list

    callbacks::header_table_col_checklist_t header_table_col_checklist = { 0 , 0 , 0 , 0 , 0 , 0 };
    this -> _exec_sqlcmd(
        R"(PRAGMA TABLE_INFO(HEADER);)" ,
        &callbacks::sqlcb_check_header_table_col ,
        ( void* ) &header_table_col_checklist ,
        ERR_SQL_CHECK_DB_CMD_EXEC_FAILED
    );
    if ( !( header_table_col_checklist.name == 1           && 
            header_table_col_checklist.create_time == 1    &&
            header_table_col_checklist.description == 1    &&
            header_table_col_checklist.owners == 1         &&
            header_table_col_checklist.enable_balance == 1 &&
            header_table_col_checklist.unknown == 0 ) )
    {
        std::ostringstream oss;
        oss << "illegal database header table [.name=" << header_table_col_checklist.name
            << ", .create_time=" << header_table_col_checklist.create_time
            << ", .description=" << header_table_col_checklist.description
            << ", .owners=" << header_table_col_checklist.owners
            << ", .enable_balance=" << header_table_col_checklist.enable_balance
            << ", .unknown=" << header_table_col_checklist.unknown << "]";
        *__out_p_s_errmsg = oss.str();
        return false;
    } // illegal header table

    callbacks::data_table_col_checklist_t data_table_col_checklist = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };
    this -> _exec_sqlcmd(
        R"(PRAGMA TABLE_INFO(DATA);)" ,
        &callbacks::sqlcb_check_data_table_col ,
        ( void* ) &data_table_col_checklist ,
        ERR_SQL_CHECK_DB_CMD_EXEC_FAILED
    );
    if ( !( data_table_col_checklist.id == 1                           &&
            data_table_col_checklist.time_stamp == 1                   &&
            data_table_col_checklist.time_zone == 1                    &&
            data_table_col_checklist.payment == 1                      &&
            data_table_col_checklist.currency_type == 1                &&
            data_table_col_checklist.description == 1                  &&
            data_table_col_checklist.currency_exchange == 1            &&
            data_table_col_checklist.total_payment_after_exchange == 1 &&
            data_table_col_checklist.balance == 1                      &&
            data_table_col_checklist.unknown == 0 ) )
    {
        std::ostringstream oss;
        oss << "illegal database data table [.id=" << data_table_col_checklist.id
            << ", .time_stamp=" << data_table_col_checklist.time_stamp
            << ", .time_zone=" << data_table_col_checklist.time_zone
            << ", .payment=" << data_table_col_checklist.payment
            << ", .currency_type=" << data_table_col_checklist.currency_type
            << ", .description=" << data_table_col_checklist.description
            << ", .currency_exchange=" << data_table_col_checklist.currency_exchange
            << ", .total_payment_after_exchange=" << data_table_col_checklist.total_payment_after_exchange
            << ", .balance=" << data_table_col_checklist.balance
            << ", .unknown=" << data_table_col_checklist.unknown << "]";
        *__out_p_s_errmsg = oss.str();
        return false;
    } // illegal data table

    return true;
}

unsigned int utils::sql::_get_table_len( utils::sql::_tablename_t __e_table ){
    std::ostringstream oss;
    oss << "SELECT COUNT(*) FROM ";
    switch ( __e_table )
    {
        case HEADER: oss << "HEADER;"; break;
        case DATA:   oss << "DATA;";   break;
        default: 
            throw sql_exception( ERR_SQL_UNEXPECTED , "database table name enum error" );
            break;
    }
    int len;
    this -> _exec_sqlcmd(
        oss.str() ,
        &callbacks::sqlcb_get_table_length ,
        ( void* ) &len ,
        ERR_SQL_GET_TABLE_LENGTH_FAILED
    );
    return len;
}

/**
 * Execute sql command without callback function.
 */
void utils::sql::_exec_sqlcmd( const std::string& __s_cmd , int __i_errno ){
    char* errmsg = { 0 };
    int rc = sqlite3_exec( this -> _p_sql3_dbobj , __s_cmd.c_str() , nullptr , nullptr , &errmsg );
    if ( rc != SQLITE_OK )
    {
        std::ostringstream oss;
        oss << errmsg << " [rc=" << rc << "]";
        sqlite3_free( errmsg );
        throw sql_exception( __i_errno , oss.str() );
    }
    return;
}

/**
 * Execute sql command with callback function
 */
void utils::sql::_exec_sqlcmd( const std::string& __s_cmd , sql_callback __f_sqlcb , void* __p_v_data , int __i_errno ){
    char* errmsg = { 0 };
    int rc = sqlite3_exec( this -> _p_sql3_dbobj , __s_cmd.c_str() , __f_sqlcb , __p_v_data , &errmsg );
    if ( rc != SQLITE_OK )
    {
        std::ostringstream oss;
        oss << errmsg << " [rc=" << rc << "]";
        sqlite3_free( errmsg );
        throw sql_exception( __i_errno , oss.str() );
    }
    return;
}

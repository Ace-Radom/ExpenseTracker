#include"core/sql.h"
#include"core/build_cfgs.h"
#include"core/sql_callbacks.h"
#include"core/log.h"

#include<openssl/sha.h>

#include<cassert>
#include<chrono>
#include<sstream>
#include<cstring>

namespace fs = std::filesystem;
namespace utils = rena::et::core::utils;

#define DB_VERSION 1

/**
 * Current ExpenseTracker database version is: 1
 *
 * The format table of ExpenseTracker in each database should be kept in the following form:
 *   ┌────────────────┬────────────────────────┬───────────────────────┬───────────────────────┬─────────────────────────┐
 *   │ Format Version │ ExpenseTracker Version │ ExpenseTracker Commit │ ExpenseTracker Branch │ First Create Time (UTC) │
 *   ├────────────────┼────────────────────────┼───────────────────────┼───────────────────────┼─────────────────────────┤
 *   │      INT       │          TEXT          │         TEXT          │         TEXT          │           INT           │
 *   └────────────────┴────────────────────────┴───────────────────────┴───────────────────────┴─────────────────────────┘
 * After database creation, the format table SHOULDN'T BE CHANGED any more.
 *
 * The calibration table of ExpenseTracker in each databse should be kept in the following form:
 *   ┌──────────────────┐
 *   │ Calibration Code │
 *   ├──────────────────┤
 *   │       TEXT       │
 *   └──────────────────┘
 * This table contains a hash of the format table. 
 * Same as the format table, the calibration table SHOULD'T BE CHANGED any more after database creation.
 *
 * The header table of ExpenseTracker in each database should be kept in the following form:
 *   ┌──────────┬───────────────────┬─────────────┬──────────┬──────────────────────┬────────────────┐
 *   │   Name   │ Create Time (UTC) │ Description │  Owners  │ Target Currency Type │ Enable Balance │
 *   ├──────────┼───────────────────┼─────────────┼──────────┼──────────────────────┼────────────────┤
 *   │   TEXT   │        INT        │    TEXT     │   TEXT   │         INT          │      INT       │
 *   └──────────┴───────────────────┴─────────────┴──────────┴──────────────────────┴────────────────┘
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

#define BUILD_CALLBACK_DATA( cb_func ) { &cb_func , #cb_func }

std::string _make_calibration_str( int __s_dbv , const std::string& __s_etv , const std::string& __s_commit , const std::string& __s_branch , time_t __t_utc_now ){
    std::ostringstream oss;
    oss << __s_dbv << __s_etv << __s_commit << __s_branch << __t_utc_now;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    const char* buf = oss.str().c_str();
    SHA256( reinterpret_cast<const unsigned char*>( buf ) , strlen( buf ) , hash );
    std::string out;
    for ( int i = 0 ; i < SHA256_DIGEST_LENGTH ; i++ )
    {
        char temp[3];
        memset( temp , '\0' , sizeof( temp ) );
        sprintf( temp , "%02x" , hash[i] );
        out.append( temp );
    }
    return out;
}

/**
 * Open a database and check its legality. If it doesn't exist, create it.
 */
utils::sql::sql( fs::path __p_dbpath ){
    this -> _p_dbpath = __p_dbpath;
    bool is_new = !fs::exists( this -> _p_dbpath );
    int rc = sqlite3_open( this -> _p_dbpath.string().c_str() , &( this -> _p_sql3_dbobj ) );
    if ( rc != SQLITE_OK )
    {
        LOG_E( WARNING ) << "open sql database failed: path: " << this -> _p_dbpath << " errmsg: \"" << sqlite3_errmsg( this -> _p_sql3_dbobj ) << "\"";
        throw sql_exception( ERR_SQL_NOT_OPEN , sqlite3_errmsg( this -> _p_sql3_dbobj ) );
    }
    LOG_E( INFO ) << "sql database opened: path: " << this -> _p_dbpath;
    if ( is_new )
    {
        LOG_E( INFO ) << "new sql database, create tables";
        this -> _create_tables();
    }
    else
    {
        std::string errmsg;
        if ( !( this -> _check_db_legality( &errmsg ) ) )
        {
            LOG_E( WARNING ) << "illegal sql database: " << " errmsg: \"" << errmsg << "\"";
            throw sql_exception( ERR_SQL_ILLEGAL_DB , errmsg );
        }
    }
    LOG_E( INFO ) << "sql database opened and verified";
    return;
}

utils::sql::~sql(){
    sqlite3_close( this -> _p_sql3_dbobj );
    LOG_E( INFO ) << "sql database closed";
    return;
}

/**
 * Write database header table datas.
 * Count header length before calling this function to make sure that datas won't be overwritten accidentally!!!
 */
void utils::sql::write_header( const utils::header_dat_t& __shd_data ){
    if ( this -> _get_table_len( HEADER ) > 0 )
    {
        LOG_E( INFO ) << "header is not empty, erase old header datas";
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
    LOG_E( INFO ) << "writing header";
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
    LOG_E( INFO ) << "writing data";
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

    LOG_E( INFO ) << "getting header";
    header_dat_t* data = new header_dat_t;
    this -> _exec_sqlcmd(
        R"(SELECT * FROM HEADER;)" ,
        BUILD_CALLBACK_DATA( callbacks::sqlcb_get_header_data ) ,
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
        LOG_E( WARNING ) << "data id out of range: id: " << __i_id;
        return nullptr;
    }

    LOG_E( INFO ) << "getting data: id: " << __i_id;
    std::vector<data_dat_t>* data = new std::vector<data_dat_t>;
    if ( __i_id == -1 )
    {
        this -> _exec_sqlcmd(
            R"(SELECT * FROM DATA ORDER BY ID ASC;)" ,
            BUILD_CALLBACK_DATA( callbacks::sqlcb_get_data_data ) ,
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
            BUILD_CALLBACK_DATA( callbacks::sqlcb_get_data_data ) ,
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
    LOG_E( INFO ) << "creating format table";
    this -> _exec_sqlcmd(
        R"(
CREATE TABLE FORMAT(
    FORMAT_VERSION          INT     NOT NULL,
    EXPENSETRACKER_VERSION  TEXT    NOT NULL,
    EXPENSETRACKER_COMMIT   TEXT    NOT NULL,
    EXPENSETRACKER_BRANCH   TEXT    NOT NULL,
    FIRST_CREATE_TIME       INT     NOT NULL
);
)" ,
        ERR_SQL_CREATE_FORMAT_TABLE_FAILED
    );
    LOG_E( INFO ) << "creating calibration table";
    this -> _exec_sqlcmd(
        R"(
CREATE TABLE CALIBRATION(
    CALIBRATION_CODE    TEXT    NOT NULL
);
)" ,
        ERR_SQL_CREATE_CALIBRATION_TABLE_FAILED
    );
    LOG_E( INFO ) << "creating header table";
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
    LOG_E( INFO ) << "creating data table";
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
    this -> _write_format_calibration();
    return;
}

void utils::sql::_write_format_calibration(){
    if ( this -> _get_table_len( FORMAT ) > 0 )
    {
        LOG_E( WARNING ) << "format table rewrite";
        throw sql_exception( ERR_SQL_FORMAT_TABLE_REWRITE , "format table rewrite" );
    } // format table rewrite
    if ( this -> _get_table_len( CALIBRATION ) > 0 )
    {
        LOG_E( WARNING ) << "calibration table rewrite";
        throw sql_exception( ERR_SQL_CALIBRATION_TABLE_REWRITE , "calibration table rewrite" );
    } // calibration table rewrite

    auto now = std::chrono::system_clock::now();
    auto now_ts = std::chrono::system_clock::to_time_t( now );
    time_t utc_now_ts = std::mktime( std::gmtime( &now_ts ) );

    std::ostringstream oss;
    oss << "INSERT INTO FORMAT VALUES (" << DB_VERSION << ",'"
                                         << BUILD_CFG_VERSION << "','"
                                         << BUILD_CFG_BUILD_GIT_COMMIT << "','"
                                         << BUILD_CFG_BUILD_GIT_BRANCH << "',"
                                         << utc_now_ts << ");";
    LOG_E( INFO ) << "writing format";
    this -> _exec_sqlcmd(
        oss.str() ,
        ERR_SQL_INSERT_FORMAT_TABLE_FAILED
    );

    oss.str( "" );
    oss << "INSERT INTO CALIBRATION VALUES ('" << _make_calibration_str(
        DB_VERSION ,
        BUILD_CFG_VERSION ,
        BUILD_CFG_BUILD_GIT_COMMIT ,
        BUILD_CFG_BUILD_GIT_BRANCH ,
        utc_now_ts
    ) << "');";
    LOG_E( INFO ) << "writing calibration";
    this -> _exec_sqlcmd(
        oss.str() ,
        ERR_SQL_INSERT_CALIBRATION_TABLE_FAILED
    );

    return;
}

/**
 * Check database legality / format. (table list, header table cols, data table cols)
 */
bool utils::sql::_check_db_legality( std::string* __out_p_s_errmsg ){
    assert( __out_p_s_errmsg );

    LOG_E( INFO ) << "start sql database legality check";

    LOG_E( INFO ) << "checking table list";
    callbacks::table_checklist_t table_checklist = { 0 , 0 , 0 , 0 , 0 };
    this -> _exec_sqlcmd( 
        R"(SELECT name FROM sqlite_master WHERE type='table';)" ,
        BUILD_CALLBACK_DATA( callbacks::sqlcb_check_table_list ) ,
        ( void* ) &table_checklist ,
        ERR_SQL_CHECK_DB_CMD_EXEC_FAILED
    );
    if ( !( table_checklist.format == 1      &&
            table_checklist.calibration == 1 &&
            table_checklist.header == 1      &&
            table_checklist.data == 1        &&
            table_checklist.unknown == 0 ) )
    {
        std::ostringstream oss;
        oss << "illegal database table list [.format=" << table_checklist.format
            << ", .calibration=" << table_checklist.calibration
            << ", .header=" << table_checklist.header
            << ", .data=" << table_checklist.data
            << ", .unknown=" << table_checklist.unknown << "]";
        *__out_p_s_errmsg = oss.str();
        LOG_E( WARNING ) << "check table list failed: errmsg: \"" << oss.str() << std::endl; 
        return false;
    } // illegal table list

    LOG_E( INFO ) << "checking format table col list";
    callbacks::format_table_col_checklist_t format_table_col_checklist = { 0 , 0 , 0 , 0 , 0 , 0 };
    this -> _exec_sqlcmd(
        R"(PRAGMA TABLE_INFO(FORMAT);)" ,
        BUILD_CALLBACK_DATA( callbacks::sqlcb_check_format_table_col ) ,
        ( void* ) &format_table_col_checklist ,
        ERR_SQL_CHECK_DB_CMD_EXEC_FAILED
    );
    if ( !( format_table_col_checklist.format_version == 1    &&
            format_table_col_checklist.et_version == 1        &&
            format_table_col_checklist.et_commit == 1         &&
            format_table_col_checklist.et_branch == 1         &&
            format_table_col_checklist.first_create_time == 1 &&
            format_table_col_checklist.unknown == 0 ) )
    {
        std::ostringstream oss;
        oss << "illegal database format table [.format_version=" << format_table_col_checklist.format_version
            << ", .et_version=" << format_table_col_checklist.et_version
            << ", .et_commit=" << format_table_col_checklist.et_commit
            << ", et_branch=" << format_table_col_checklist.et_branch
            << ", first_create_time=" << format_table_col_checklist.first_create_time
            << ", unknown=" << format_table_col_checklist.unknown << "]";
        *__out_p_s_errmsg = oss.str();
        LOG_E( WARNING ) << "check format table col list failed: errmsg: \"" << oss.str() << "\"";
        return false;
    } // illegal format table

    LOG_E( INFO ) << "checking calibration table col list";
    callbacks::calibration_table_col_checklist_t calibration_table_col_checklist = { 0 , 0 };
    this -> _exec_sqlcmd(
        R"(PRAGMA TABLE_INFO(CALIBRATION);)" ,
        BUILD_CALLBACK_DATA( callbacks::sqlcb_check_calibration_table_col ) ,
        ( void* ) &calibration_table_col_checklist ,
        ERR_SQL_CHECK_DB_CMD_EXEC_FAILED
    );
    if ( !( calibration_table_col_checklist.calibration_code == 1 &&
            calibration_table_col_checklist.unknown == 0 ))
    {
        std::ostringstream oss;
        oss << "illegal database calibration table [.calibration_code=" << calibration_table_col_checklist.calibration_code
            << ", .unknown=" << calibration_table_col_checklist.unknown << "]";
        *__out_p_s_errmsg = oss.str();
        LOG_E( WARNING ) << "check calibration table col list failed: errmsg: \"" << oss.str() << "\"";
        return false;
    } // illegal calibration table

    LOG_E( INFO ) << "checking format table length";
    unsigned int format_table_len = this -> _get_table_len( FORMAT );
    if ( format_table_len != 1 )
    {
        std::ostringstream oss;
        oss << "illegal database format table length [len=" << format_table_len << "]";
        *__out_p_s_errmsg = oss.str();
        LOG_E( WARNING ) << "format table length is not 1, but " << format_table_len;
        return false;
    } // illegal format table length

    LOG_E( INFO ) << "checking calibration table length";
    unsigned int calibration_table_len = this -> _get_table_len( CALIBRATION );
    if ( calibration_table_len != 1 )
    {
        std::ostringstream oss;
        oss << "illegal database calibration table length [len=" << calibration_table_len << "]";
        *__out_p_s_errmsg = oss.str();
        LOG_E( WARNING ) << "calibration table length is not 1, but " << calibration_table_len;
        return false;
    } // illegal calibration table length

    LOG_E( INFO ) << "checking calibration code";
    format_dat_t* format_data = new format_dat_t;
    calibration_dat_t* calibration_data = new calibration_dat_t;
    this -> _exec_sqlcmd(
        R"(SELECT * FROM FORMAT;)" ,
        BUILD_CALLBACK_DATA( callbacks::sqlcb_get_format_data ) ,
        ( void* ) format_data ,
        ERR_SQL_CHECK_DB_CMD_EXEC_FAILED
    );
    this -> _exec_sqlcmd(
        R"(SELECT * FROM CALIBRATION;)" ,
        BUILD_CALLBACK_DATA( callbacks::sqlcb_get_calibration_data ) ,
        ( void* ) calibration_data ,
        ERR_SQL_CHECK_DB_CMD_EXEC_FAILED
    );
    std::string code_now = _make_calibration_str(
        format_data -> format_version ,
        format_data -> et_version ,
        format_data -> et_commit ,
        format_data -> et_branch ,
        format_data -> first_create_time
    );
    if ( calibration_data -> calibration_code != code_now )
    {
        std::ostringstream oss;
        oss << "illegal database calibration code [code=\"" << calibration_data -> calibration_code << "\", code_now=\"" << code_now << "\"]";
        LOG_E( WARNING ) << "check calibration code failed: code is \"" << code_now << "\", should be \"" << calibration_data -> calibration_code << "\"";
        delete format_data;
        delete calibration_data;
        return false;
    } // illegal calibration code
    delete format_data;
    delete calibration_data;

    LOG_E( INFO ) << "checking header table col list";
    callbacks::header_table_col_checklist_t header_table_col_checklist = { 0 , 0 , 0 , 0 , 0 , 0 };
    this -> _exec_sqlcmd(
        R"(PRAGMA TABLE_INFO(HEADER);)" ,
        BUILD_CALLBACK_DATA( callbacks::sqlcb_check_header_table_col ) ,
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
        LOG_E( WARNING ) << "check header table col list failed: errmsg: \"" << oss.str() << "\""; 
        return false;
    } // illegal header table

    LOG_E( INFO ) << "checking data table col list";
    callbacks::data_table_col_checklist_t data_table_col_checklist = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };
    this -> _exec_sqlcmd(
        R"(PRAGMA TABLE_INFO(DATA);)" ,
        BUILD_CALLBACK_DATA( callbacks::sqlcb_check_data_table_col ) ,
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
        LOG_E( WARNING ) << "check data table col list failed: errmsg: \"" << oss.str() << "\"";
        return false;
    } // illegal data table

    LOG_E( INFO ) << "sql database verified";

    return true;
}

unsigned int utils::sql::_get_table_len( utils::sql::_tablename_t __e_table ){
    LOG_E( INFO ) << "getting table length: table: " << static_cast<int>( __e_table );
    std::ostringstream oss;
    oss << "SELECT COUNT(*) FROM ";
    switch ( __e_table )
    {
        case FORMAT:      oss << "FORMAT;"; break;
        case CALIBRATION: oss << "CALIBRATION"; break;
        case HEADER:      oss << "HEADER;"; break;
        case DATA:        oss << "DATA;";   break;
        default: 
            throw sql_exception( ERR_SQL_UNEXPECTED , "database table name enum error" );
            break;
    }
    int len;
    this -> _exec_sqlcmd(
        oss.str() ,
        BUILD_CALLBACK_DATA( callbacks::sqlcb_get_table_length ) ,
        ( void* ) &len ,
        ERR_SQL_GET_TABLE_LENGTH_FAILED
    );
    return len;
}

/**
 * Execute sql command without callback function.
 */
void utils::sql::_exec_sqlcmd( const std::string& __s_cmd , int __i_errno ){
    LOG_E( INFO ) << "executing sql command: cmd: \"" << __s_cmd << "\"";
    char* errmsg = { 0 };
    int rc = sqlite3_exec( this -> _p_sql3_dbobj , __s_cmd.c_str() , nullptr , nullptr , &errmsg );
    if ( rc != SQLITE_OK )
    {
        std::ostringstream oss;
        oss << errmsg << " [rc=" << rc << "]";
        sqlite3_free( errmsg );
        LOG_E( WARNING ) << "sql command execute failed: errmsg: \"" << oss.str() << "\"";
        throw sql_exception( __i_errno , oss.str() );
    }
    return;
}

/**
 * Execute sql command with callback function
 */
void utils::sql::_exec_sqlcmd( const std::string& __s_cmd , const callback_data_t& __scd_cb , void* __p_v_data , int __i_errno ){
    LOG_E( INFO ) << "executing sql command: cmd: \"" << __s_cmd << "\" callback: \"" << __scd_cb.s_cb_name << "\"";
    char* errmsg = { 0 };
    int rc = sqlite3_exec( this -> _p_sql3_dbobj , __s_cmd.c_str() , __scd_cb.f_cb , __p_v_data , &errmsg );
    if ( rc != SQLITE_OK )
    {
        
        std::ostringstream oss;
        oss << errmsg << " [rc=" << rc << "]";
        sqlite3_free( errmsg );
        LOG_E( WARNING ) << "sql command execute failed: errmsg: \"" << oss.str() << "\"";
        throw sql_exception( __i_errno , oss.str() );
    }
    return;
}

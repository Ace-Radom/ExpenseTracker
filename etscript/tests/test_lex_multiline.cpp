#include"etscript/lex.h"

using namespace rena::et::etscript;

int main(){
    lex lex;
    std::string src = R"(if ( version > 20000 )
{
    do( "\"test1\"" );
}
else
{
    do( "test2\n" );
})";
    lex.mount_src( src );
    lex.parse();
    std::hash<std::string> hashstr;
    std::vector<lex::token_data_t> v = {
        { lex::If , hashstr( "if" ) , "if" , 0 , 1 , 1 } ,
        { lex::LftRndBrkt , 0 , "" , 0 , 1 , 4 } ,
        { lex::Word , hashstr( "version" ) , "version" , 0 , 1 , 6 } ,
        { lex::Gter , 0 , "" , 0 , 1 , 14 } ,
        { lex::Num , 0 , "" , 20000 , 1 , 16 } ,
        { lex::RhtRndBrkt , 0 , "" , 0 , 1 , 22 } ,
        { lex::LftCurBrkt , 0 , "" , 0 , 2 , 1 } ,
        { lex::Word , hashstr( "do" ) , "do" , 0 , 3 , 5 } ,
        { lex::LftRndBrkt , 0 , "" , 0 , 3 , 7 } ,
        { lex::Str , 0 , "\"test1\"" , 0 , 3 , 9 } ,
        { lex::RhtRndBrkt , 0 , "" , 0 , 3 , 21 } ,
        { lex::SemiCol , 0 , "" , 0 , 3 , 22 } ,
        { lex::RhtCurBrkt , 0 , "" , 0 , 4 , 1 } ,
        { lex::Else , hashstr( "else" ) , "else" , 0 , 5 , 1 } ,
        { lex::LftCurBrkt , 0 , "" , 0 , 6 , 1 } ,
        { lex::Word , hashstr( "do" ) , "do" , 0 , 7 , 5 } ,
        { lex::LftRndBrkt , 0 , "" , 0 , 7 , 7 } ,
        { lex::Str , 0 , "test2\n" , 0 , 7 , 9 } ,
        { lex::RhtRndBrkt , 0 , "" , 0 , 7 , 19 } ,
        { lex::SemiCol , 0 , "" , 0 , 7 , 20 } ,
        { lex::RhtCurBrkt , 0 , "" , 0 , 8 , 1 }
    };
    for ( int i = 0 ; i < lex.token_len() ; i++ )
    {
        auto token = lex.next_token();
        if ( !( token.type == v[i].type && token.hash == v[i].hash && token.name == v[i].name && token.value == v[i].value && token.line == v[i].line && token.column == v[i].column ) )
        {
            return i + 1;
        }
    }
    return 0;
}

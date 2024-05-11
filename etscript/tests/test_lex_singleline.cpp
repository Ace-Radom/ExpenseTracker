#include"etscript/lex.h"

using namespace rena::et::etscript;

int main(){
    lex lex;
    std::string src = R"(int i = 0; if ( ( type == 1 && version != 20000 ) || ( version >= 10000 && version <= 20000 && type > 0 && type < 3 && !balance ) ){ while ( i < 100 ){ i = i + 015; if ( ( i - 10 ) * 0x20 / 10 % 5 == 1 ){ i = 100; } } })";
    lex.mount_src( src );
    lex.parse();
    std::hash<std::string> hashstr;
    std::vector<lex::token_data_t> v = {
        { lex::Word , hashstr( "int" ) , "int" , 0 , 1 , 1 } ,
        { lex::Word , hashstr( "i" ) , "i" , 0 , 1 , 5 } ,
        { lex::Assign , 0 , "" , 0 , 1 , 7 } ,
        { lex::Num , 0  , "" , 0 , 1 , 9 } ,
        { lex::SemiCol , 0 , "" , 0 , 1 , 10 } ,
        { lex::If , hashstr( "if" ) , "if" , 0 , 1 , 12 } ,
        { lex::LftRndBrkt , 0 , "" , 0 , 1 , 15 } ,
        { lex::LftRndBrkt , 0 , "" , 0 , 1 , 17 } ,
        { lex::Word , hashstr( "type" ) , "type" , 0 , 1 , 19 } ,
        { lex::Eq , 0 , "" , 0 , 1 , 24 } ,
        { lex::Num , 0 , "" , 1 , 1 , 27 } ,
        { lex::LAnd , 0 , "" , 0 , 1 , 29 } ,
        { lex::Word , hashstr( "version" ) , "version" , 0 , 1 , 32 } ,
        { lex::NotEq , 0 , "" , 0 , 1 , 40 } ,
        { lex::Num , 0 , "" , 20000 , 1 , 43 } ,
        { lex::RhtRndBrkt , 0 , "" , 0 , 1 , 49 } ,
        { lex::LOr , 0 , "" , 0 , 1 , 51 } ,
        { lex::LftRndBrkt , 0 , "" , 0 , 1 , 54 } ,
        { lex::Word , hashstr( "version" ) , "version" , 0 , 1 , 56 } ,
        { lex::GterEq , 0 , "" , 0 , 1 , 64 } ,
        { lex::Num , 0 , "" , 10000 , 1 , 67 } ,
        { lex::LAnd , 0 , "" , 0 , 1 , 73 } ,
        { lex::Word , hashstr( "version" ) , "version" , 0 , 1 , 76 } ,
        { lex::LwerEq , 0 , "" , 0 , 1 , 84 } ,
        { lex::Num , 0 , "" , 20000 , 1 , 87 } ,
        { lex::LAnd , 0 , "" , 0 , 1 , 93 } ,
        { lex::Word , hashstr( "type" ) , "type" , 0 , 1 , 96 } ,
        { lex::Gter , 0 , "" , 0 , 1 , 101 } ,
        { lex::Num , 0 , "" , 0 , 1 , 103 } ,
        { lex::LAnd , 0 , "" , 0 , 1 , 105 } ,
        { lex::Word , hashstr( "type" ) , "type" , 0 , 1 , 108 } ,
        { lex::Lwer , 0 , "" , 0 , 1 , 113 } ,
        { lex::Num , 0 , "" , 3 , 1 , 115 } ,
        { lex::LAnd , 0 , "" , 0 , 1 , 117 } ,
        { lex::Not , 0 , "" , 0 , 1 , 120 } ,
        { lex::Word , hashstr( "balance" ) , "balance" , 0 , 1 , 121 } ,
        { lex::RhtRndBrkt , 0 , "" , 0 , 1 , 129 } ,
        { lex::RhtRndBrkt , 0 , "" , 0 , 1 , 131 } ,
        { lex::LftCurBrkt , 0 , "" , 0 , 1 , 132 } ,
        { lex::While , hashstr( "while" ) , "while" , 0 , 1 , 134 } ,
        { lex::LftRndBrkt , 0 , "" , 0 , 1 , 140 } ,
        { lex::Word , hashstr( "i" ) , "i" , 0 , 1 , 142 } ,
        { lex::Lwer , 0 , "" , 0 , 1 , 144 } ,
        { lex::Num , 0 , "" , 100 , 1 , 146 } ,
        { lex::RhtRndBrkt , 0 , "" , 0 , 1 , 150 } ,
        { lex::LftCurBrkt , 0 , "" , 0 , 1 , 151 } ,
        { lex::Word , hashstr( "i" ) , "i" , 0 , 1 , 153 } ,
        { lex::Assign , 0 , "" , 0 , 1 , 155 } ,
        { lex::Word , hashstr( "i" ) , "i" , 0 , 1 , 157 } ,
        { lex::Add , 0 , "" , 0 , 1 , 159 } ,
        { lex::Num , 0 , "" , 015 , 1 , 161 } ,
        { lex::SemiCol , 0 , "" , 0 , 1 , 164 } ,
        { lex::If , hashstr( "if" ) , "if" , 0 , 1 , 166 } ,
        { lex::LftRndBrkt , 0 , "" , 0 , 1 , 169 } ,
        { lex::LftRndBrkt , 0 , "" , 0 , 1 , 171 } ,
        { lex::Word , hashstr( "i" ) , "i" , 0 , 1 , 173 } ,
        { lex::Sub , 0 , "" , 0 , 1 , 175 } ,
        { lex::Num , 0 , "" , 10 , 1 , 177 } ,
        { lex::RhtRndBrkt , 0 , "" , 0 , 1 , 180 } ,
        { lex::Mul , 0 , "" , 0 , 1 , 182 } ,
        { lex::Num , 0 , "" , 0x20 , 1 , 184 } ,
        { lex::Div , 0 , "" , 0 , 1 , 189 } ,
        { lex::Num , 0 , "" , 10 , 1 , 191 } ,
        { lex::Mod , 0 , "" , 0 , 1 , 194 } ,
        { lex::Num , 0 , "" , 5 , 1 , 196 } ,
        { lex::Eq , 0 , "" , 0 , 1 , 198 } ,
        { lex::Num , 0 , "" , 1 , 1 , 201 } ,
        { lex::RhtRndBrkt , 0 , "" , 0 , 1 , 203 } ,
        { lex::LftCurBrkt , 0 , "" , 0 , 1 , 204 } ,
        { lex::Word , hashstr( "i" ) , "i" , 0 , 1 , 206 } ,
        { lex::Assign , 0 , "" , 0 , 1 , 208 } ,
        { lex::Num , 0 , "" , 100 , 1 , 210 } ,
        { lex::SemiCol , 0 , "" , 0 , 1 , 213 } ,
        { lex::RhtCurBrkt , 0 , "" , 0 , 1 , 215 } ,
        { lex::RhtCurBrkt , 0 , "" , 0 , 1 , 217 } ,
        { lex::RhtCurBrkt , 0 , "" , 0 , 1 , 219 }
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

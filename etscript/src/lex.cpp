#include"etscript/lex.h"
#include"core/log.h"

namespace etscript = rena::et::etscript;

etscript::lex::lex(){

}

etscript::lex::~lex(){

}

void etscript::lex::mount_src( const std::string& __s_src ){
    this -> _s_src = __s_src;
    this -> _it_src = this -> _s_src.begin();
    this -> _i_line = 1;
    this -> _i_col = 1;
    this -> _v_tokens.clear();
    return;
}

void etscript::lex::parse(){
    while ( this -> _it_src != this -> _s_src.end() )
    {
        long long hash = 0;
        char token = *( this -> _it_src );
        if ( token == '\n' )
        {
            LOG_E( DEBUG ) << "met break line";
            this -> _i_line++;
            this -> _i_col = 1;
            this -> _it_src++;
        } // break line
        else if ( ( token >= 'a' && token <= 'z' ) || ( token >= 'A' && token <= 'Z' ) || ( token == '_' ) )
        {
            token_data_t this_token_data;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            std::string name;
            do {
                name += token;
                hash = hash * 147 + ( long long ) token;
                token = this -> next();
            } while ( ( token >= 'a' && token <= 'z' ) || ( token >= 'A' && token <= 'Z' ) || ( token >= '0' && token <= '9' ) || ( token == '_' ) );
            this_token_data.hash = hash;
            this_token_data.name = name;

            std::transform( name.begin() , name.end() , name.begin() , []( char c ) -> char { return std::tolower( c ); } );
            if ( name == "if" )         this_token_data.type = If;
            else if ( name == "else" )  this_token_data.type = Else;
            else if ( name == "while" ) this_token_data.type = While;
            else                        this_token_data.type = Word;
            // low all chars and check if they are reserved words

            this -> _v_tokens.push_back( this_token_data );
            continue;
        } // word
        else if ( token >= '0' && token <= '9' )
        {
            token_data_t this_token_data;
            this_token_data.type = Num;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            int token_val = token - '0';
            if ( token_val > 0 )
            {
                do {
                    token_val = token_val * 10 + token - '0';
                    token = this -> next();
                } while ( token >= '0' && token <= '9' );
            } // DEC
            else
            {
                token = this -> next();
                if ( token == 'x' || token == 'X' )
                {
                    token = this -> next();
                    while ( ( token >= '0' && token <= '9' ) || ( token >= 'a' && token <= 'f' ) || ( token >= 'A' && token <= 'F' ) )
                    {
                        token_val = token_val * 16 + ( token & 15 ) + ( token >= 'A' ? 9 : 0 );
                        token = this -> next();
                    }
                } // HEX
                else
                {
                    while ( token >= '0' && token <= '7' )
                    {
                        token_val = token_val * 8 + token - '0';
                        token = this -> next();
                    }
                } // OCT
            }
            this_token_data.value = token_val;
            this -> _v_tokens.push_back( this_token_data );
            continue;
        } // number
        else if ( token == '/' )
        {
            token = this -> next();
            if ( token == '/' )
            {
                while ( token != '\n' )
                {
                    token = this -> next();
                }
            } // comment
            else
            {
                token_data_t this_token_data;
                this_token_data.type = Div;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
            } // div
            continue;
        } // '/' (comment / div)
        else if ( token == '|' )
        {
            token = this -> next();
            if ( token == '|' )
            {
                token_data_t this_token_data;
                this_token_data.type = Lor; //>>LOr
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
                continue;
            }
        }// '||' implementation
        else if ( token == '=' )
        {
            token = this -> next();
            if ( token == '=' )
            {
                token_data_t this_token_data;
                this_token_data.type = Eq;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
            }//'=='
            else
            {
                token_data_t this_token_data;
                this_token_data.type = Assign;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col;
                this -> _v_tokens.push_back( this_token_data );
            }//'='
            continue;
        }//'=' and '==' implementation
        else if ( token == '!' )
        {
            token = this -> next();
            if ( token == '=' )
            {
                token_data_t this_token_data;
                this_token_data.type = Ne; //couldve used something better like NotEq, Ne is unintuitive
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
            }
            else
            {
                token_data_t this_token_data;
                this_token_data.type = Not;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col;
                this -> _v_tokens.push_back( this_token_data );
            }
            continue;
        }//'!' and '!=' implementation
        else if ( token == '<' )
        {
            token = this -> next();
            if ( token == '=' )
            {
                token_data_t this_token_data;
                this_token_data.type = Le; //unintuitive, >>LwerEq
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
            }
            else
            {
                token_data_t this_token_data;
                this_token_data.type = Lt; //>>Lwer
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col;
                this -> _v_tokens.push_back( this_token_data );
            }
            continue;
        }//'<' and '<=' implementation
        else if ( token == '>' )
        {
            token = this -> next();
            if ( token == '=' )
            {
                token_data_t this_token_data;
                this_token_data.type = Ge; //>>GterEq
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
            }
            else
            {
                token_data_t this_token_data;
                this_token_data.type = Gt; //>>Gter
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col;
                this -> _v_tokens.push_back( this_token_data );
            }
            continue;
        }//'>' and '>=' implementation
        else if ( token == '&' )
        {
            token = this -> next();
            if ( token == '&' )
           {
                token_data_t this_token_data;
                this_token_data.type = Land; //>>LAnd, or you just forgot 'Land' being a single word
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
                continue;
           } 
        }//'&&' implementation
        else if ( token == '(' )
        {
            token_data_t this_token_data;
            this_token_data.type = Lrb; //>>LftRndBrkt
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        }//'(' only
        else if ( token == ')' )
        {
            token_data_t this_token_data;
            this_token_data.type = Rrb; //>>RhtRndBrkt
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        }//')' only
        else if ( token == '{' )
        {
            token_data_t this_token_data;
            this_token_data.type = Lcb; //>>LftCurBrkt, also why its not small/medium/big brackets?
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        }//'{' only
        else if ( token == '}' )
        {
            token_data_t this_token_data;
            this_token_data.type = Rcb; //>>RhtCurBrkt
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        }//'}' only
        else if ( token == ';' )
        {
            token_data_t this_token_data;
            this_token_data.type = Semic; //>>SemiCol
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        }//';' only
        else if ( token == '+' )
        {
            token_data_t this_token_data;
            this_token_data.type = Add;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        }//'+' only
        else if ( token == '-' )
        {
            token_data_t this_token_data;
            this_token_data.type = Sub;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        }//'-' only
        else if ( token == '*' )
        {
            token_data_t this_token_data;
            this_token_data.type = Mul; //>>Multi
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        }//'*' only
        else if ( token == '%' )
        {
            token_data_t this_token_data;
            this_token_data.type = Mod;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        }//'%' only

        this -> next();
    }

}

char etscript::lex::next(){
    this -> _i_col++;
    this -> _it_src++;
    return *( this -> _it_src );
}

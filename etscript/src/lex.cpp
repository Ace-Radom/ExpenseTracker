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
        char token = *( this -> _it_src );
        token_data_t this_token_data = {};
        if ( token == '\n' )
        {
            LOG_E( DEBUG ) << "met break line";
            this -> _i_line++;
            this -> _i_col = 1;
            this -> _it_src++;
            continue;
        } // break line
        else if ( ( token >= 'a' && token <= 'z' ) || ( token >= 'A' && token <= 'Z' ) || ( token == '_' ) )
        {
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            std::string name;
            do {
                name += token;
                token = this -> next();
            } while ( ( token >= 'a' && token <= 'z' ) || ( token >= 'A' && token <= 'Z' ) || ( token >= '0' && token <= '9' ) || ( token == '_' ) );
            this_token_data.hash = std::hash<std::string>{}( name );
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
            this_token_data.type = Num;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            int token_val = token - '0';
            if ( token_val > 0 )
            {
                token = this -> next();
                while ( token >= '0' && token <= '9' )
                {
                    token_val = token_val * 10 + token - '0';
                    token = this -> next();
                }
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
        else if ( token == '"' )
        {
            this_token_data.type = Str;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            std::string name;
            token = this -> next();
            while ( token != '"' && token != '\n' )
            {
                if ( token == '\\' )
                {
                    token = this -> next();
                    if ( token == '"' )
                    {
                        name += '"';
                    }
                    else if ( token == 'n' )
                    {
                        name += "\n";
                    }
                    else
                    {
                        name = name + '\\' + token;
                    }
                } // escape chars
                else
                {
                    name += token;
                }
                token = this -> next();
            }
            this_token_data.name = name;
            this -> _v_tokens.push_back( this_token_data );
            if ( token == '\n' )
            {
                continue;
            } // no need to parse right qm, but linebreak does
        } // str
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
                this_token_data.type = Div;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
            } // div
            continue;
        } // '/' and '//' implementation
        else if ( token == '|' )
        {
            token = this -> next();
            if ( token == '|' )
            {
                this_token_data.type = LOr;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
            } // '||'
        } // '||' implementation
        else if ( token == '=' )
        {
            token = this -> next();
            if ( token == '=' )
            {
                this_token_data.type = Eq;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
            } // '=='
            else
            {
                this_token_data.type = Assign;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
                continue;
            } // '='
        } // '=' and '==' implementation
        else if ( token == '!' )
        {
            token = this -> next();
            if ( token == '=' )
            {
                this_token_data.type = NotEq;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
            } // '!='
            else
            {
                this_token_data.type = Not;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
                continue;
            } // '!'
        } // '!' and '!=' implementation
        else if ( token == '<' )
        {
            token = this -> next();
            if ( token == '=' )
            {
                this_token_data.type = LwerEq;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
            } // '<='
            else
            {
                this_token_data.type = Lwer;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
                continue;
            } // '<'
        } // '<' and '<=' implementation
        else if ( token == '>' )
        {
            token = this -> next();
            if ( token == '=' )
            {
                this_token_data.type = GterEq;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
            } // '>='
            else
            {
                this_token_data.type = Gter;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
                continue;
            } // '>'
        } // '>' and '>=' implementation
        else if ( token == '&' )
        {
            token = this -> next();
            if ( token == '&' )
           {
                this_token_data.type = LAnd;
                this_token_data.line = this -> _i_line;
                this_token_data.column = this -> _i_col - 1;
                this -> _v_tokens.push_back( this_token_data );
           } 
        } // '&&' implementation
        else if ( token == '(' )
        {
            this_token_data.type = LftRndBrkt;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        } // '(' only
        else if ( token == ')' )
        {
            this_token_data.type = RhtRndBrkt;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        } // ')' only
        else if ( token == '{' )
        {
            this_token_data.type = LftCurBrkt;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        } // '{' only
        else if ( token == '}' )
        {
            this_token_data.type = RhtCurBrkt;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        } // '}' only
        else if ( token == ';' )
        {
            this_token_data.type = SemiCol;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        } // ';' only
        else if ( token == '+' )
        {
            this_token_data.type = Add;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        } // '+' only
        else if ( token == '-' )
        {
            this_token_data.type = Sub;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        } // '-' only
        else if ( token == '*' )
        {
            this_token_data.type = Mul;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        } // '*' only
        else if ( token == '%' )
        {
            this_token_data.type = Mod;
            this_token_data.line = this -> _i_line;
            this_token_data.column = this -> _i_col;
            this -> _v_tokens.push_back( this_token_data );
        } // '%' only

        this -> next();
    }
    this -> reset_token_get_pos();
    return;
}

size_t etscript::lex::token_len() const noexcept {
    return this -> _v_tokens.size();
}

void etscript::lex::reset_token_get_pos(){
    this -> _it_tokens = this -> _v_tokens.begin();
    return;
}

const etscript::lex::token_data_t etscript::lex::next_token(){
    auto token = *( this -> _it_tokens );
    this -> _it_tokens++;
    return token;
}

char etscript::lex::next(){
    this -> _i_col++;
    this -> _it_src++;
    return *( this -> _it_src );
}

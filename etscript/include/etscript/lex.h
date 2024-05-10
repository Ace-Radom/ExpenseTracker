#ifndef _EXPENSETRACKER_ETSCRIPT_LEX_H_
#define _EXPENSETRACKER_ETSCRIPT_LEX_H_

#include<string>
#include<vector>

namespace rena::et::etscript {

    class lex {

        public:
            lex();
            ~lex();

            void mount_src( const std::string& __s_src );
            void parse();
            void trace();

        public:
            typedef enum {
                Assign , Not , LOr , LAnd , Eq , NotEq , Lwer , LwerEq , Gter , GterEq , Add , Sub , Mul , Div , Mod ,
                If , Else , While ,
                Word , Num ,
                LftRndBrkt , RhtRndBrkt , LftCurBrkt , RhtCurBrkt , SemiCol ,
                Unknown
            } token_type_t;

            typedef struct {
                token_type_t type;
                long long hash;
                std::string name;
                long long value;
                int line;
                int column;
            } token_data_t;

        protected:
            char next();

        private:
            std::string _s_src;
            std::string::iterator _it_src;
            int _i_line;
            int _i_col;
            std::vector<token_data_t> _v_tokens;

    }; // class lex

} // namespace rena::et::etscript

#endif // _EXPENSETRACKER_ETSCRIPT_LEX_H_

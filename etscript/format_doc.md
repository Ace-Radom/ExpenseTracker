```
program             ::= {global_calls}
global_calls        ::= var_decl | function_call
var_decl            ::= type id '=' value ';'
function_call       ::= non_empty_statement | empty_statement
non_empty_statement ::= if_statement | while_statement | '{' {core_call} '}' | core_call
if_statement        ::= 'if' '(' expression ')' function_call ['else' non_empty_statement]
while_statement     ::= 'while' '(' expression ')' non_empty_statement
core_call           ::= core_function '(' {arguments} ')' ';'
arguments           ::= value [','] | text [',']
```
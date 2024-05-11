#include"etscript/vm.h"

using namespace rena::et::etscript;

/**
 * ic_sub:
 *      ENT     1
 *      LEA     3
 *      LI
 *      PUSH
 *      LEA     2
 *      LI
 *      MOD
 *      ADJ     1
 *      LEV
 *
 * ic_main:
 *      IMM     1715386493
 *      PUSH
 *      IMM     86400
 *      PUSH
 *      CALL    ic_sub
 *      PUSH
 *      EXIT
 */

int main(){
    vm vm;
    long long ic_sub[] = {
        vm::ENT ,
        1 ,
        vm::LEA ,
        3 ,
        vm::LI ,
        vm::PUSH ,
        vm::LEA ,
        2 ,
        vm::LI ,
        vm::MOD ,
        vm::ADJ ,
        1 ,
        vm::LEV
    };
    long long ic_main[] = {
        vm::IMM ,
        1715386493 ,
        vm::PUSH ,
        vm::IMM ,
        86400 ,
        vm::PUSH ,
        vm::CALL ,
        ( long long ) ic_sub ,
        vm::PUSH ,
        vm::EXIT
    };
    vm.load_IC( ic_main );
    return vm.run() == 893 ? 0 : 1;
}

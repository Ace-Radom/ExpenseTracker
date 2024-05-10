#include"etscript/vm.h"

using namespace rena::et::etscript;

/**
 * ic_jnz:
 *      IMM     1024
 *      PUSH
 *      EXIT
 *
 * ic_jz:
 *      IMM     1
 *      JNZ     ic_jnz
 *      IMM     2048
 *      PUSH
 *      EXIT
 *
 * ic_main:
 *      IMM     1
 *      JZ      ic_jz
 *      IMM     4096
 *      PUSH
 *      EXIT
 */

int main(){
    vm vm;
    long long ic_jnz[] = {
        vm::IMM ,
        1024 ,
        vm::PUSH ,
        vm::EXIT
    };
    long long ic_jz[] = {
        vm::IMM ,
        1 ,
        vm::JNZ ,
        ( long long ) ic_jnz ,
        vm::IMM ,
        2048 ,
        vm::PUSH ,
        vm::EXIT
    };
    long long ic_main[] = {
        vm::IMM ,
        1 ,
        vm::JZ ,
        ( long long ) ic_jz ,
        vm::IMM ,
        4096 ,
        vm::PUSH ,
        vm::EXIT
    };
    vm.load_IC( ic_main );
    return vm.run() == 2048 ? 0 : 1;
}

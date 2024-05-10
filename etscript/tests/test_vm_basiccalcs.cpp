#include"etscript/vm.h"

using namespace rena::et::etscript;

/**
 * IMM      64
 * PUSH
 * IMM      2
 * SHL              ; ax -> 256
 * PUSH
 * IMM      4
 * SHR              ; ax -> 16
 * PUSH
 * IMM      16
 * ADD              ; ax -> 32
 * PUSH
 * IMM      8
 * SUB              ; ax -> 24
 * PUSH
 * IMM      4
 * MUL              ; ax -> 96
 * PUSH
 * IMM      8
 * DIV              ; ax -> 12
 * PUSH
 * IMM      10
 * MOD              ; ax -> 2
 * PUSH
 * EXIT
*/

int main(){
    vm vm;
    long long ic[] = {
        vm::IMM ,
        64 ,
        vm::PUSH ,
        vm::IMM ,
        2 ,
        vm::SHL ,
        vm::PUSH ,
        vm::IMM ,
        4 ,
        vm::SHR ,
        vm::PUSH ,
        vm::IMM ,
        16 ,
        vm::ADD ,
        vm::PUSH ,
        vm::IMM ,
        8 ,
        vm::SUB ,
        vm::PUSH ,
        vm::IMM ,
        4 ,
        vm::MUL ,
        vm::PUSH ,
        vm::IMM ,
        8 ,
        vm::DIV ,
        vm::PUSH ,
        vm::IMM ,
        10 ,
        vm::MOD ,
        vm::PUSH ,
        vm::EXIT
    };
    vm.load_IC( ic );
    return vm.run() == 2 ? 0 : 1;
}

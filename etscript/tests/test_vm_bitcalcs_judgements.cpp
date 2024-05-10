#include"etscript/vm.h"

using namespace rena::et::etscript;

/**
 * IMM      16
 * PUSH
 * IMM      16
 * EQ               ; ax -> 1
 * PUSH
 * IMM      32
 * PUSH
 * IMM      32
 * NE               ; ax -> 0
 * PUSH
 * IMM      16
 * PUSH
 * IMM      32
 * LT               ; ax -> 1
 * PUSH
 * IMM      32
 * PUSH
 * IMM      32
 * LE               ; ax -> 1
 * PUSH
 * IMM      32
 * PUSH
 * IMM      64
 * GT               ; ax -> 0
 * PUSH
 * IMM      64
 * PUSH
 * IMM      64
 * GE               ; ax -> 1
 * XOR              ; 0 ^ 1 => ax -> 1
 * AND              ; 1 & 1 => ax -> 1
 * XOR              ; 1 ^ 1 => ax -> 0
 * OR               ; 0 | 0 => ax -> 0
 * OR               ; 1 | 0 => ax -> 1
 * PUSH
 * EXIT
 */

int main(){
    vm vm;
    long long ic[] = {
        vm::IMM ,
        16 ,
        vm::PUSH ,
        vm::IMM ,
        16 ,
        vm::EQ ,
        vm::PUSH ,
        vm::IMM ,
        32 ,
        vm::PUSH ,
        vm::IMM ,
        32 ,
        vm::NE ,
        vm::PUSH ,
        vm::IMM ,
        16 , 
        vm::PUSH ,
        vm::IMM ,
        32 , 
        vm::LT ,
        vm::PUSH ,
        vm::IMM ,
        32 ,
        vm::PUSH ,
        vm::IMM ,
        32 ,
        vm::LE ,
        vm::PUSH ,
        vm::IMM ,
        32 ,
        vm::PUSH ,
        vm::IMM ,
        64 ,
        vm::GT ,
        vm::PUSH ,
        vm::IMM ,
        64 ,
        vm::PUSH ,
        vm::IMM ,
        64 ,
        vm::GE ,
        vm::XOR ,
        vm::AND ,
        vm::XOR ,
        vm::OR ,
        vm::OR ,
        vm::PUSH ,
        vm::EXIT
    };
    vm.load_IC( ic );
    return vm.run() == 1 ? 0 : 1;
}

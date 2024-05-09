#ifndef _EXPENSETRACKER_ETSCRIPT_VM_H_
#define _EXPENSETRACKER_ETSCRIPT_VM_H_

#include<cstddef>
#include<typeindex>
#include<vector>

#include"core/core.h"

namespace rena::et::etscript {

    class vm {

        public:
            vm();
            ~vm();

            void init();
            void shutdown();

            void load_IC( long long* __p_ll_ic );
            void reset_IC();

            int run();

        public:
            typedef enum {
                IMM , LC , LI , SC , SI ,                       // MOV
                PUSH , POP ,                                    // stack ctrl
                JMP ,                                           // JMP
                JZ , JNZ ,                                      // JS, JNZ
                CALL , ENT , ADJ , LEV , LEA ,                  // CALL, RET
                OR , XOR , AND , EQ , NE , LT , LE , GT , GE ,
                SHL , SHR , ADD , SUB , MUL , DIV , MOD ,
                TSN , ICS , PRTF , EXIT ,                       // built-in
                ETCC ,                                          // ExpenseTracker Core Call
            } IS;  // Instruction Set

        private:
            typedef struct {
                void* memp;
                std::type_index type;
            } requested_mem_info_t;

        private:
            constexpr static const size_t _ll_stacksize = 64 * 1024;

            long long* _p_ll_stack;
            long long* _p_ll_pc;
            long long* _p_ll_sp;
            long long* _p_ll_bsp;
            long long _ll_ax;

            std::vector<requested_mem_info_t> _v_requested_mem_list;

    }; // class vm

} // namespace rena::et::etscript

#endif // _EXPENSETRACKER_ETSCRIPT_VM_H_

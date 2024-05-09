#include"vm.h"
#include"log.h"

#include<cassert>
#include<chrono>
#include<cstdlib>
#include<cstring>
#include<string>

namespace etscript = rena::et::etscript;

etscript::vm::vm(){
    this -> init();
    return;
}

etscript::vm::~vm(){
    this -> shutdown();
    return;
}

void etscript::vm::init(){
    this -> _p_ll_stack = new long long[this->_ll_stacksize];
    memset( this -> _p_ll_stack , 0 , this -> _ll_stacksize * sizeof( long long ) );
    this -> _p_ll_pc = nullptr;
    this -> _p_ll_bsp = this -> _p_ll_sp = ( long long* ) ( ( long long ) this -> _p_ll_stack + this -> _ll_stacksize * sizeof( long long ) );
    this -> _ll_ax = 0;
    this -> _v_requested_mem_list.clear();
    return;
}

void etscript::vm::shutdown(){
    delete[] this -> _p_ll_stack;
    this -> _p_ll_pc = nullptr;
    this -> _p_ll_sp = nullptr;
    this -> _p_ll_bsp = nullptr;
    this -> _ll_ax = 0;
    for ( const auto& it : this -> _v_requested_mem_list )
    {
        if ( it.type == typeid( char* ) )               delete[] ( char* ) it.memp;
        else if ( it.type == typeid( long long* ) )     delete ( long long* ) it.memp;
    }
    return;
}

void etscript::vm::load_IC( long long* __p_ll_ic ){
    this -> _p_ll_pc = __p_ll_ic;
    return;
}

void etscript::vm::reset_IC(){
    this -> _p_ll_pc = nullptr;
    return;
}

int etscript::vm::run(){
    assert( this -> _p_ll_pc );

    long long cycle = 0;
    long long op;
    std::string tempstr;
    char* temppc = nullptr;

    while ( 1 )
    {
        cycle++;
        op = *_p_ll_pc++;

        LOG_E( DEBUG ) << "vm cycle " << cycle << ": op: " << op 
                                               << " ax: " << this -> _ll_ax
                                               << " pc: " << this -> _p_ll_pc
                                               << " sp: " << this -> _p_ll_sp
                                               << " bsp: " << this -> _p_ll_bsp;

        switch ( static_cast<IS>( op ) )
        {
            case IMM:   _ll_ax = *_p_ll_pc++; break;                                                                                            // load immediate value to ax                                       IMM <value>;
            case LC:    _ll_ax = *( char* ) _ll_ax; break;                                                                                      // load char to ax, addr in ax                                      LC;
            case LI:    _ll_ax = *( long long* ) _ll_ax; break;                                                                                 // load int to ax, addr in ax                                       LI;
            case SC:    *( char* ) *_p_ll_sp++ = _ll_ax; break;                                                                                 // save char to addr, value in ax, addr on stack                    SC;
            case SI:    *( long long* ) *_p_ll_sp++ = _ll_ax; break;                                                                            // save int to addr, value in ax, addr on stack                     SI;
            case PUSH:  *--_p_ll_sp = _ll_ax; break;                                                                                            // push ax onto the stack                                           IMM <value>; PUSH;
            case POP:   _ll_ax = *_p_ll_sp++; break;                                                                                            // pop the last item on the stack to ax                             POP;
            case JMP:   _p_ll_pc = ( long long* ) *_p_ll_pc; break;                                                                             // jump to addr                                                     JMP <addr>;
            case JZ:    _p_ll_pc = _ll_ax ? ( long long* ) *_p_ll_pc : _p_ll_pc + 1; break;                                                     // jump to addr if ax is zero                                       JZ <addr>;
            case JNZ:   _p_ll_pc = _ll_ax ? _p_ll_pc + 1 : ( long long* ) *_p_ll_pc; break;                                                     // jump to addr if ax is not zero                                   JNZ <addr>;
            case CALL:  *--_p_ll_sp = ( long long ) ( _p_ll_pc + 1 ); _p_ll_pc = ( long long* ) *_p_ll_pc; break;                               // call subroutine                                                  CALL <addr>;
            case ENT:   *--_p_ll_sp = ( long long ) _p_ll_bsp; _p_ll_bsp = _p_ll_sp; _p_ll_sp = _p_ll_sp - *_p_ll_pc++; break;                  // enter new stack frame                                            ENT <size>;
            case ADJ:   _p_ll_sp = _p_ll_sp + *_p_ll_pc++; break;                                                                               // leave new stack frame                                            ADJ <size>;
            case LEV:   _p_ll_sp = _p_ll_bsp; _p_ll_bsp = ( long long* ) *_p_ll_sp++; _p_ll_pc = ( long long* ) *_p_ll_sp++; break;             // restore stack frame and return to parent-routine                 LEV;
            case LEA:   _ll_ax = ( long long ) ( _p_ll_bsp + *_p_ll_pc++ ); break;                                                              // load address for arguments for subroutine                        LEA <pos>;

            case OR:    _ll_ax = *_p_ll_sp++ | _ll_ax; break;                                                                                   // value on stack OR value in ax, result to ax                      IMM <value1>; PUSH; IMM <value2>; OR;
            case XOR:   _ll_ax = *_p_ll_sp++ ^ _ll_ax; break;                                                                                   // value on stack XOR value in ax, result to ax                     IMM <value1>; PUSH; IMM <value2>; XOR;
            case AND:   _ll_ax = *_p_ll_sp++ & _ll_ax; break;                                                                                   // value on stack AND value in ax, result to ax                     IMM <value1>; PUSH; IMM <value2>; AND;
            case EQ:    _ll_ax = *_p_ll_sp++ == _ll_ax; break;                                                                                  // value on stack EQUAL value in ax, result to ax                   IMM <value1>; PUSH; IMM <value2>; EQ;
            case NE:    _ll_ax = *_p_ll_sp++ != _ll_ax; break;                                                                                  // value on stack NOT EQUAL value in ax, result to ax               IMM <value1>; PUSH; IMM <value2>; NE;
            case LT:    _ll_ax = *_p_ll_sp++ < _ll_ax; break;                                                                                   // value on stack LESS THAN value in ax, result to ax               IMM <value1>; PUSH; IMM <value2>; LT;
            case LE:    _ll_ax = *_p_ll_sp++ <= _ll_ax; break;                                                                                  // value on stack LESS THAN / EQUAL value in ax, result to ax       IMM <value1>; PUSH; IMM <value2>; LE;
            case GT:    _ll_ax = *_p_ll_sp++ > _ll_ax; break;                                                                                   // value on stack GREATER THAN value in ax, result to ax            IMM <value1>; PUSH; IMM <value2>; GT;
            case GE:    _ll_ax = *_p_ll_sp++ >= _ll_ax; break;                                                                                  // value on stack GREATER THAN / EQUAL value in ax, result to ax    IMM <value1>; PUSH; IMM <value2>; GE;
            case SHL:   _ll_ax = *_p_ll_sp++ << _ll_ax; break;                                                                                  // value on stack SHIFT LEFT value in ax, result to ax              IMM <value1>; PUSH; IMM <value2>; SHL;
            case SHR:   _ll_ax = *_p_ll_sp++ >> _ll_ax; break;                                                                                  // value on stack SHIFT RIGHT value in ax, result to ax             IMM <value1>; PUSH; IMM <value2>; SHR;
            case ADD:   _ll_ax = *_p_ll_sp++ + _ll_ax; break;                                                                                   // value on stack ADD value in ax, result to ax                     IMM <value1>; PUSH; IMM <value2>; ADD;
            case SUB:   _ll_ax = *_p_ll_sp++ - _ll_ax; break;                                                                                   // value on stack SUBTRACT value in ax, result to ax                IMM <value1>; PUSH; IMM <value2>; SUB;
            case MUL:   _ll_ax = *_p_ll_sp++ * _ll_ax; break;                                                                                   // value on stack MULTIPLE value in ax, result to ax                IMM <value1>; PUSH; IMM <value2>; MUL;
            case DIV:   _ll_ax = *_p_ll_sp++ / _ll_ax; break;                                                                                   // value on stack DIVIDE value in ax, result to ax                  IMM <value1>; PUSH; IMM <value2>; DIV;
            case MOD:   _ll_ax = *_p_ll_sp++ % _ll_ax; break;                                                                                   // value on stack MODULO value in ax, result to ax                  IMM <value1>; PUSH; IMM <value2>; MOD;

            case TSN:                                                                                                                           // load current timestamp to ax                                     TSN;
                _ll_ax = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() ); break;
            case ICS:                                                                                                                           // convert int in ax to str, save in ax                             IMM <int_value>; ICS;
                tempstr = std::to_string( _ll_ax ); temppc = new char[tempstr.size()];
                _v_requested_mem_list.push_back( { ( void* ) temppc , typeid( char* ) } );
                strcpy( temppc , tempstr.c_str() ); _ll_ax = ( long long ) temppc; break;
            case PRTF:                                                                                                                          // print one str to terminal, str on stack                          IMM <str_addr>; PUSH; PRTF;
                _ll_ax = printf( ( char* ) *_p_ll_sp++ ); break;
            case EXIT:                                                                                                                          // exit vm cycle                                                    EXIT;
                LOG_E( DEBUG ) << "vm exits at cycle " << cycle << ": *sp: " << *_p_ll_sp << " ax: " << _ll_ax;
                return *_p_ll_sp; break;

            case ETCC:                                                                                                                          // ExpenseTracker core call, args on stack, rc to ax                IMM <args>; PUSH; ... ETCC <cmd_num>;
            
            default:
                LOG_E( WARNING ) << "vm crashes because of unknown instruction: op: " << op;
                return -1; break;
        }
    }

    return -1;
}

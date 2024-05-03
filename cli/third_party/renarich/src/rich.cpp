#include"rich.h"

namespace rich = rena::rich;

#ifdef RICH_FORCE_WINAPI

WORD _fgc;
WORD _bgc;
WORD _cflag;
WORD _oricflag;

HANDLE rich::putils::GetStdoutHandle(){
    return GetStdHandle( STD_OUTPUT_HANDLE );
}

void rich::putils::SetForegroundColor( rich::FColor __fc ){
    switch ( __fc ) {
        case FColor::BLACK:         _fgc = 0x0000; break;
        case FColor::RED:           _fgc = 0x0004; break;
        case FColor::GREEN:         _fgc = 0x0002; break;
        case FColor::YELLOW:        _fgc = 0x0006; break;
        case FColor::BLUE:          _fgc = 0x0001; break;
        case FColor::MAGENTA:       _fgc = 0x0005; break;
        case FColor::CYAN:          _fgc = 0x0003; break;
        case FColor::WHITE:         _fgc = 0x0007; break;
        case FColor::GRAY:          _fgc = 0x0008; break;
        case FColor::BRIGHTRED:     _fgc = 0x000C; break;
        case FColor::BRIGHTGREEN:   _fgc = 0x000A; break;
        case FColor::BRIGHTYELLOW:  _fgc = 0x000E; break;
        case FColor::BRIGHTBLUE:    _fgc = 0x0009; break;
        case FColor::BRIGHTMAGENTA: _fgc = 0x000D; break;
        case FColor::BRIGHTCYAN:    _fgc = 0x000B; break;
        case FColor::BRIGHTWHITE:   _fgc = 0x000F; break;
        default: return;
    };
    _cflag = _fgc | _bgc;
    return;
}

void rich::putils::SetBackgroundColor( rich::BColor __bc ){
    switch ( __bc ) {
        case BColor::BLACK:         _bgc = 0x0000; break;
        case BColor::RED:           _bgc = 0x0040; break;
        case BColor::GREEN:         _bgc = 0x0020; break;
        case BColor::YELLOW:        _bgc = 0x0060; break;
        case BColor::BLUE:          _bgc = 0x0010; break;
        case BColor::MAGENTA:       _bgc = 0x0050; break;
        case BColor::CYAN:          _bgc = 0x0030; break;
        case BColor::WHITE:         _bgc = 0x0070; break;
        case BColor::GRAY:          _bgc = 0x0080; break;
        case BColor::BRIGHTRED:     _bgc = 0x00C0; break;
        case BColor::BRIGHTGREEN:   _bgc = 0x00A0; break;
        case BColor::BRIGHTYELLOW:  _bgc = 0x00E0; break;
        case BColor::BRIGHTBLUE:    _bgc = 0x0090; break;
        case BColor::BRIGHTMAGENTA: _bgc = 0x00D0; break;
        case BColor::BRIGHTCYAN:    _bgc = 0x00B0; break;
        case BColor::BRIGHTWHITE:   _bgc = 0x00F0; break;
        default: return;
    };
    _cflag = _fgc | _bgc;
    return;
}

void rich::putils::ResetColor(){
    _cflag = _oricflag;
    _fgc = 0;
    _bgc = 0;
    return;
}

WORD rich::putils::GetColorFlag(){
    return _cflag;
}

#endif

void rich::init(){
#ifdef RICH_FORCE_WINAPI
    CONSOLE_SCREEN_BUFFER_INFO csbufinfo;
    GetConsoleScreenBufferInfo( putils::GetStdoutHandle() , &csbufinfo );
    _oricflag = csbufinfo.wAttributes;
#endif
    return;
}

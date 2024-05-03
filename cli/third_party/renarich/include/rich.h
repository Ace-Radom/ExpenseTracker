#ifndef _RENAMD_RICH_H_
#define _RENAMD_RICH_H_

#pragma once

#include<iostream>
#ifdef RICH_FORCE_WINAPI
#include<windows.h>
#include<tchar.h>
#endif

namespace rena {

    namespace rich {
        
        typedef enum class FColor : int {
            BLACK         = 30,
            RED           = 31,
            GREEN         = 32,
            YELLOW        = 33,
            BLUE          = 34,
            MAGENTA       = 35,
            CYAN          = 36,
            WHITE         = 37,
            GRAY          = 90,
            BRIGHTRED     = 91,
            BRIGHTGREEN   = 92,
            BRIGHTYELLOW  = 93,
            BRIGHTBLUE    = 94,
            BRIGHTMAGENTA = 95,
            BRIGHTCYAN    = 96,
            BRIGHTWHITE   = 97,
        } FColor;

        typedef enum class BColor : int {
            BLACK         = 40,
            RED           = 41,
            GREEN         = 42,
            YELLOW        = 43,
            BLUE          = 44,
            MAGENTA       = 45,
            CYAN          = 46,
            WHITE         = 47,
            GRAY          = 100,
            BRIGHTRED     = 101,
            BRIGHTGREEN   = 102,
            BRIGHTYELLOW  = 103,
            BRIGHTBLUE    = 104,
            BRIGHTMAGENTA = 105,
            BRIGHTCYAN    = 106,
            BRIGHTWHITE   = 107,
        } BColor;

        typedef enum class FStyle : int {
            BOLD          = 1,
            DIM           = 2,
            ITALIC        = 3,
            UNDERLINE     = 4,
        } FStyle;

#ifdef RICH_FORCE_WINAPI

        namespace putils {
            
            HANDLE GetStdoutHandle();
            void SetForegroundColor( FColor __fc );
            void SetBackgroundColor( BColor __bc );
            void ResetColor();
            WORD GetColorFlag();

        }; // namespace putils

#endif

        void init();

        template <class _Elem , class _Traits>
        std::basic_ostream<_Elem,_Traits>& operator<<( std::basic_ostream<_Elem,_Traits>& __os , FColor __fc ){
#ifdef RICH_FORCE_WINAPI
            putils::SetForegroundColor( __fc );
            SetConsoleTextAttribute( putils::GetStdoutHandle() , putils::GetColorFlag() );
#else
            __os << "\033[" << static_cast<unsigned int>( __fc ) << "m";
#endif
            return __os;
        }

        template <class _Elem , class _Traits>
        std::basic_ostream<_Elem,_Traits>& operator<<( std::basic_ostream<_Elem,_Traits>& __os , BColor __bc ){
#ifdef RICH_FORCE_WINAPI
            putils::SetBackgroundColor( __bc );
            SetConsoleTextAttribute( putils::GetStdoutHandle() , putils::GetColorFlag() );
#else
            __os << "\033[" << static_cast<unsigned int>( __bc ) << "m";
#endif
            return __os;
        }

        /**
         * @warning FStyle DOES NOT work when RICH_FORCE_WINAPI is set
        */
        template <class _Elem , class _Traits>
        std::basic_ostream<_Elem,_Traits>& operator<<( std::basic_ostream<_Elem,_Traits>& __os , FStyle __fs ){
#ifndef RICH_FORCE_WINAPI
            __os << "\033[" << static_cast<unsigned int>( __fs ) << "m";
#endif
            return __os;
        }

        template <class _Elem , class _Traits>
        std::basic_ostream<_Elem,_Traits>& style_reset( std::basic_ostream<_Elem,_Traits>& __os ){
#ifdef RICH_FORCE_WINAPI
            putils::ResetColor();
            SetConsoleTextAttribute( putils::GetStdoutHandle() , putils::GetColorFlag() );
#else
            __os << "\033[0m";
#endif
            return __os;
        }

        template <class _Elem , class _Traits>
        std::basic_ostream<_Elem,_Traits>& clear_line( std::basic_ostream<_Elem,_Traits>& __os ){
#ifdef RICH_FORCE_WINAPI
            CONSOLE_SCREEN_BUFFER_INFO csbufinfo;
            GetConsoleScreenBufferInfo( putils::GetStdoutHandle() , &csbufinfo );
            COORD cp;
            cp.X = 0;
            cp.Y = csbufinfo.dwCursorPosition.Y;
            SetConsoleCursorPosition( putils::GetStdoutHandle() , cp );
            DWORD cw;
            FillConsoleOutputCharacter( putils::GetStdoutHandle() , _T( ' ' ) , csbufinfo.dwSize.X , cp , &cw );
            SetConsoleCursorPosition( putils::GetStdoutHandle() , cp );
#else
            __os << "\033[2K\r";
#endif
            return __os;
        }


    }; // namespace rich

}; // namespace rena

#endif // _RENAMD_RICH_H_

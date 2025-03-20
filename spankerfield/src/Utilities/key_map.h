#pragma once
#include "../common.h"

std::map<int, std::string> key_map =
{
    // Mouse inputs
    { VK_LBUTTON, xorstr_("Left Mouse") },
    { VK_RBUTTON, xorstr_("Right Mouse") },
    { VK_MBUTTON, xorstr_("Middle Mouse") },
    { VK_XBUTTON1, xorstr_("Mouse 4") },
    { VK_XBUTTON2, xorstr_("Mouse 5") },

    // Movement keys
    { 0x57, xorstr_("W") },           // 'W'
    { 0x41, xorstr_("A") },           // 'A'
    { 0x53, xorstr_("S") },           // 'S' 
    { 0x44, xorstr_("D") },           // 'D'
    { VK_SPACE, xorstr_("Space") },
    { VK_SHIFT, xorstr_("Shift") },
    { VK_CONTROL, xorstr_("Ctrl") },
    { 0x43, xorstr_("C") },           // 'C'
    { VK_MENU, xorstr_("Alt") },

    // Action keys
    { 0x52, xorstr_("R") },           // 'R'
    { 0x45, xorstr_("E") },           // 'E'
    { 0x46, xorstr_("F") },           // 'F'
    { 0x47, xorstr_("G") },           // 'G'
    { 0x51, xorstr_("Q") },           // 'Q'
    { 0x58, xorstr_("X") },           // 'X'
    { 0x5A, xorstr_("Z") },           // 'Z'
    { 0x56, xorstr_("V") },           // 'V'
    { 0x31, xorstr_("1") },           // '1'
    { 0x32, xorstr_("2") },           // '2'
    { 0x33, xorstr_("3") },           // '3'
    { 0x34, xorstr_("4") },           // '4'
    { 0x35, xorstr_("5") },           // '5'

    // Basic navigation keys
    { VK_TAB, xorstr_("Tab") },
    { VK_INSERT, xorstr_("Insert") },
    { VK_HOME, xorstr_("Home") },
    { VK_PRIOR, xorstr_("Page Up") },
    { VK_ESCAPE, xorstr_("Esc") },
    { VK_F1, xorstr_("F1") },
    { VK_F2, xorstr_("F2") },
    { VK_F3, xorstr_("F3") },
    { VK_F4, xorstr_("F4") },

    // Additional useful keys
    { VK_RETURN, xorstr_("Enter") },
    { VK_DELETE, xorstr_("Delete") },
    { VK_END, xorstr_("End") },
    { VK_NEXT, xorstr_("Page Down") },
    { VK_F5, xorstr_("F5") },
    { VK_F6, xorstr_("F6") },
    { VK_F7, xorstr_("F7") },
    { VK_F8, xorstr_("F8") },
    { VK_F9, xorstr_("F9") },
    { VK_F10, xorstr_("F10") },
    { VK_F11, xorstr_("F11") },
    { VK_F12, xorstr_("F12") }
};

std::map<int, std::string> open_key_map =
{
    { VK_INSERT, xorstr_("Insert") },
    { VK_DELETE, xorstr_("Delete") },
    { VK_HOME, xorstr_("Home") },
    { VK_END, xorstr_("End") },
    { VK_PRIOR, xorstr_("Page Up") },
    { VK_NEXT, xorstr_("Page Down") },
    { VK_F1, xorstr_("F1") },
    { VK_F2, xorstr_("F2") },
    { VK_F3, xorstr_("F3") },
    { VK_F4, xorstr_("F4") },
    { VK_F5, xorstr_("F5") },
    { VK_F6, xorstr_("F6") },
    { VK_F7, xorstr_("F7") },
    { VK_F8, xorstr_("F8") },
    { VK_F9, xorstr_("F9") },
    { VK_F10, xorstr_("F10") },
    { VK_F11, xorstr_("F11") },
    { VK_F12, xorstr_("F12") }
};
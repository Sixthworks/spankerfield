#pragma once
#include "../../common.h"

std::map<int, std::string> key_map =
{
    { VK_LBUTTON, xorstr_("Left Mouse") },
    { VK_RBUTTON, xorstr_("Right Mouse") },
    { VK_MBUTTON, xorstr_("Middle Mouse") },
    { VK_XBUTTON1, xorstr_("Mouse 4") },
    { VK_XBUTTON2, xorstr_("Mouse 5") },
    { VK_CAPITAL, xorstr_("Caps Lock") },
    { VK_SHIFT, xorstr_("Shift") },
    { VK_CONTROL, xorstr_("Ctrl") },
    { VK_MENU, xorstr_("Alt") },
    { VK_SPACE, xorstr_("Space") },
    { VK_RETURN, xorstr_("Enter") },
    { VK_TAB, xorstr_("Tab") },
    { VK_ESCAPE, xorstr_("Esc") },
    { VK_INSERT, xorstr_("Insert") },
    { VK_DELETE, xorstr_("Delete") },
    { VK_HOME, xorstr_("Home") },
    { VK_END, xorstr_("End") },
    { VK_PRIOR, xorstr_("Page Up") },
    { VK_NEXT, xorstr_("Page Down") },
    { VK_UP, xorstr_("Up Arrow") },
    { VK_DOWN, xorstr_("Down Arrow") },
    { VK_LEFT, xorstr_("Left Arrow") },
    { VK_RIGHT, xorstr_("Right Arrow") },
    { VK_F1, xorstr_("F1") },
    { VK_F2, xorstr_("F2") },
    { VK_F3, xorstr_("F3") }
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
    { VK_F3, xorstr_("F3") }
};
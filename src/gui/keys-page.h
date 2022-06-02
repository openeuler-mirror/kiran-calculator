/**
* @Copyright (C) 2021 KylinSec Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; If not, see <http: //www.gnu.org/licenses/>.
*
* Author:     luoqing <luoqing@kylinos.com.cn>
*/
#ifndef KEYSPAGE_H
#define KEYSPAGE_H

enum Button
{
    Button_Key_0, Button_Key_1, Button_Key_2, Button_Key_3, Button_Key_4,
    Button_Key_5, Button_Key_6, Button_Key_7, Button_Key_8, Button_Key_9,
    Button_Key_A, Button_Key_B, Button_Key_C, Button_Key_D, Button_Key_E,Button_Key_F,

    Button_Key_Add, Button_Key_Sub, Button_Key_Mult, Button_Key_Divide,   //加、减、乘、除
    Button_Key_Point, Button_Key_Equal, Button_Key_Brackets, Button_Key_Percent,                     //"."   "=" "()"
    Button_Key_Sqrt, Button_Key_Square, Button_Key_Reciprocal, Button_Key_Backspace,
    Button_Key_Clear,Button_Key_ClearEntry,Button_Key_LeftBracket,Button_Key_RightBracket,

    Button_Key_Xn, Button_Key_Sin, Button_Key_Cos, Button_Key_Tan,
    Button_Key_10n, Button_Key_Log, Button_Key_Exp, Button_Key_Mod,

    Button_Key_Cube, Button_Key_Yroot, Button_Key_Arcsin,Button_Key_Arccos,
    Button_Key_Arctan,Button_Key_Index,Button_Key_Ln,

    Button_Key_Pi, Button_Key_Opposite, Button_Key_Factorials,

    Button_Key_And, Button_Key_Xor, Button_Key_Not, Button_Key_Or,
    Button_Key_Lsh, Button_Key_Rsh, Button_Key_RoR, Button_Key_RoL,

    Button_Key_Shift,Button_Key_AngleUnit,Button_Key_FE,Button_Key_HYP,
};

#endif // KEYSPAGE_H

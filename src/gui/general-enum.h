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
#ifndef GENERALENUM_H
#define GENERALENUM_H

enum CalculationMode{
    Calculation_Mode_Standard,Calculation_Mode_Science,Calculation_Mode_Programmer,
};

enum NumFormat{
    Num_Format_Hex,Num_Format_Dec,Num_Format_Oct,Num_Format_Bin
};

enum AngleUnit{
    Angle_Unit_Degree,Angle_Unit_Radian,Angle_Unit_Gradian,
};

enum ModeLabel{
    Mode_Label_Standard,Mode_Label_Science,Mode_Label_Programmer,Mode_Label_About
};

#endif // GENERALENUM_H

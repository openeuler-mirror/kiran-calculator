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
#ifndef SSELECTION_H
#define SSELECTION_H
#include <QString>

struct SSelection {
    QString oldText; //选中时的当前text
    QString selected; //被选中部分
    QString clearText;
    bool isChanged;
    int curpos = 0;  // fix for bug-14249选中的左侧光标所在位置
    SSelection() { isChanged = false; }
};

#endif // SSELECTION_H

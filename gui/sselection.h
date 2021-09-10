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

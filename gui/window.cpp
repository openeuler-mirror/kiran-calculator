#include "window.h"
#include "./ui_window.h"
//#include "ui_window.h"
#include "history-recoder.h"
#include "mode-switcher-page/mode-switcher-page.h"
#include "standard-mode-page/standard-keys-page.h"
#include "standard-mode-page/expr-calculator.h"
#include "programmer-mode-page/num-conversion.h"

#include "core/session.h"
#include "core/evaluator.h"
#include "core/settings.h"

#include <QMessageBox>
#include <QDebug>

Window::Window(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Window)
{
    ui->setupUi(this);
    //并不一定后初始化，在上层
    //m_drawer->raise(); 通过raise将图层放在上面
    //通过new得到的控件，默认显示在比它new的早的控件上面

//    m_settings = Settings::instance();

    m_drawer = new ModeSwitcherPage(this);

   //侧边栏按钮布局，待确定
    QPushButton *m_drawerButton = new QPushButton("模式",this);
    m_drawerButton->show();
    m_drawerButton->move(200,10);
    m_drawerButton->resize(25,25);

// Ui::CustomDrawer *ui 为私有，调用不到，同时不建议越层调用，可以用其他方法访问属性
//    connect(m_drawer->modeListWidget,SIGNAL(itemSelectionChanged()),this,
//            SLOT(on_modeLabelChanged()));

    connect(m_drawer,SIGNAL(clickList(int )),this,SLOT(on_modeLabelChanged(int )));
    connect(m_drawer,SIGNAL(clickList(int )),this,SLOT(on_switchKeyborad(int )));
    connect(m_drawerButton,SIGNAL(clicked()),this,SLOT(on_drawerButton()));

    ui->stackedWidget->setCurrentIndex(0);


}

Window::~Window()
{
    delete ui;
}

void Window::on_modeLabelChanged(int mode )
{
    qDebug() << mode;
    if(mode == 0) {ui->modeLabel->setText("标准");}

    if(mode == 1) {ui->modeLabel->setText("科学计算器");}

    if(mode == 2) {ui->modeLabel->setText("程序员计算器");}

    if(mode == 3) {ui->modeLabel->setText("关于");}
}

//不同模式下的键盘切换
void Window::on_switchKeyborad(int mode)
{
    ui->stackedWidget->setCurrentIndex(mode);
}

//缩小时，隐藏历史记录栏
//void Window::resizeEvent(QResizeEvent *event)
//{
//    qDebug() << this->frameGeometry().width();

//    int width = this->frameGeometry().width();
//    //宽度暂定,待确定
//    if(width > 581)
//    {
//        //ui->historyListWidget->setVisible(true);
//        ui->clearButton->setVisible(true);
//        ui->standardHistory->setVisible(true);
//    }
//    else
//    {
//        //ui->historyListWidget->setVisible(false);
//        ui->clearButton->setVisible(false);
//        ui->standardHistory->setVisible(false);
//    }
//}

//点击切换按钮
void Window::on_drawerButton()
{
    m_drawer->activateAnimation();
}




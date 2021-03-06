#include "mainwindow.h"
#include <QApplication>
#include <QtDebug>
#include <QDesktopWidget>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //设置编码为utf-8
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

    QSettings setting("config.ini", QSettings::IniFormat); //生成配置文件
    setting.beginGroup("config");                          //beginGroup与下面endGroup 相对应，“config”是标记
    if (setting.value("status_bar").isNull())
        setting.setValue("status_bar", QVariant("1"));
    if (setting.value("auto_go_line").isNull())
        setting.setValue("auto_go_line", QVariant("1"));
    setting.endGroup();

    MainWindow w;
    w.show();
    return a.exec();
}

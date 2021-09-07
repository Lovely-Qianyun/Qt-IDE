#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QMessageBox"
#include "QtDebug"
#include "QTranslator"
#include "QFileDialog"
#include "QFile"
#include "QDir"
#include "QTextStream"
#include <QPrinter>
#include <QPrintDialog>
#include <QTextCursor>
#include <QDateTime>
#include <QFontDialog>
#include <QFont>
#include <QLabel>
#include <QDesktopServices>
#include <QTextBlock>
#include <QMimeData>
#include <QSettings>
#include "aboutdialog.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "codeeditor.h"
#include <QDebug>
#include <color.h>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void replace(QString value, bool isChecked);
    Highlighter *highlighter;

private:
    Ui::MainWindow *ui;
    CodeEditor *editor;
    QKeyEvent *ev;
private slots:
    void newFileSolt();                                                                   //新文件槽函数
    void exitAppSlot();                                                                   //退出软件槽函数
    void openFileSlot();                                                                  //打开文件槽函数
    void textChangeSlot();                                                                //文本变化槽函数
    void saveFileSlot();                                                                  //保存文件槽函数
    void saveOtherFileSlot();                                                             //另存
    void editSolt();                                                                      //编辑
    void undoSolt();                                                                      //撤销
    void redoSolt();                                                                      //重做
    void cutSolt();                                                                       //剪切
    void copySolt();                                                                      //复制
    void pasteSolt();                                                                     //粘贴
    void deleteSolt();                                                                    //删除
    void selectAllSlot();                                                                 //全选
    void findSlot();                                                                      //查找
    void find(QString value, bool isChecked, bool isUp);                                  //查找
    void replaceSlot();                                                                   //替换
    void findForReplaceSlot(QString value, bool isChecked);                               //查找 - 替换
    void doReplaceSlot(QString target, QString value, bool isChecked, bool isReplaceAll); //替换或者全替换
    void zoomInSlot();                                                                    //放大
    void zoomOutSlot();                                                                   //缩小
    void autoLineSlot();                                                                  //自动换行
    void cursorChangeSlot();                                                              //光标变化
    void statusBarSlot();                                                                 //状态栏
    void helpSlot();                                                                      //帮助
    //连接编译器相关
    void precompileSlot(); //预编译
    void compileRunSlot(); //编译运行
    void runSlot();        //运行
    void compileSlot();    //编译

private:
    QString fileName;            //文件路径
    QString fileContent;         //读到的文件内容
    bool autoLine;               //自动换行
    bool statusBar;              //显示状态栏
    QLabel *change;              //行和列
    FindDialog findDialog;       //查找
    ReplaceDialog replaceDialog; //替换
    bool isFirstFind;
    int zoomInNum = 0;  //缩放参数
    QSettings *setting; //配置文件

    void saveTextToFile();                  //保存文本到文件
    void readFile();                        //读取文件
    void showMessage(QString);              //显示提示消息
    void init();                            //初始化一下参数
    void conn();                            //关联信号和槽
    void setShortcut();                     //设置快捷键
    void closeEvent(QCloseEvent *event);    //关闭事件
    void keyReleaseEvent(QKeyEvent *event); //括号补全
};
#endif // MAINWINDOW_H

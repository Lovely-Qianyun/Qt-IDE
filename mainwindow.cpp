#include "mainwindow.h"
#include "codeeditor.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <stdlib.h>
#include <QFile>
#include <QTextDocument>
#include <windows.h>
#include <color.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    //添加代码编辑器
    editor = new CodeEditor(ui->centralWidget);
    ui->centralWidget->layout()->addWidget(editor);
    //editor->setGeometry(0,0,800,500);

    this->init(); //进行初始化

    this->conn(); //连接信号与槽

    this->setShortcut(); //设置快捷键

    //是否传入了文件路径
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.length() > 1) {
        fileName = arguments[1];
        this->readFile();
    }
    highlighter = new Highlighter(editor->document());
}

MainWindow::~MainWindow() {
    delete ui;
}
//初始化一下参数
void MainWindow::init() {
    this->setWindowTitle("无标题 - idepad");
    this->setWindowIcon(QIcon(":/app.ico"));
    setting = new QSettings("config.ini", QSettings::IniFormat);
    //初始化默认值
    autoLine = true; //是否自动换行
    //***
    statusBar = true; //是否显示状态栏
    //**
    isFirstFind = true;            //是否第一次搜索 替换时用
    editor->setAcceptDrops(false); //设置文本输入框不接受鼠标放下
    this->setAcceptDrops(true);    //设置窗口接受鼠标放下

    setting->beginGroup("config"); //beginGroup与下面endGroup 相对应，“config”是标记
    //***
    statusBar = setting->value("status_bar").toInt() == 1 ? true : false; //获取存储的值
    //**
    autoLine = setting->value("auto_go_line").toInt() == 1 ? true : false;
    setting->endGroup();
    //***
    //设置是否显示状态栏
    ui->statusBar->setVisible(false);
    //是否显示显示状态栏勾中图标
    ui->actionStatusBar->setIconVisibleInMenu(statusBar);
    //**
    //设置Qeditor自动换行
    ui->actionAutoLine->setIconVisibleInMenu(autoLine);
    if (autoLine) {
        editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    } else {
        editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
    //设置状态栏
    ui->statusBar->setStyleSheet(QString("QStatusBar::item{border: 0px}")); // 设置不显示label的边框
    ui->statusBar->setSizeGripEnabled(false);                               //设置是否显示右边的大小控制点
    change = new QLabel("第 1 行，第 1 列", this);
    ui->statusBar->addPermanentWidget(change); //显示永久信息
    //查找和查找下一个不可用
    ui->actionFind->setEnabled(false);
    ui->actionFindNext->setEnabled(false);
    ui->actionRedo->setEnabled(false);
}
//关联信号和槽
void MainWindow::conn() {
    //添加处理 文件
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFileSolt()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exitAppSlot()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFileSlot()));
    connect(editor, SIGNAL(textChanged()), this, SLOT(textChangeSlot()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveFileSlot()));
    connect(ui->actionOtherSave, SIGNAL(triggered()), this, SLOT(saveOtherFileSlot()));

    //添加处理 编辑
    connect(ui->menuEdit, SIGNAL(aboutToShow()), this, SLOT(editSolt()));
    connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(redoSolt()));
    connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(undoSolt()));
    connect(ui->actionCut, SIGNAL(triggered()), this, SLOT(cutSolt()));
    connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(copySolt()));
    connect(ui->actionPaste, SIGNAL(triggered()), this, SLOT(pasteSolt()));
    connect(ui->actionDelete, SIGNAL(triggered()), this, SLOT(deleteSolt()));
    connect(ui->actionSelectAll, SIGNAL(triggered()), this, SLOT(selectAllSlot()));
    //添加处理 视图
    connect(ui->actionFind, SIGNAL(triggered()), this, SLOT(findSlot()));
    connect(&findDialog, SIGNAL(find(QString, bool, bool)), this, SLOT(find(QString, bool, bool)));
    connect(ui->actionReplace, SIGNAL(triggered()), this, SLOT(replaceSlot()));
    connect(&replaceDialog, SIGNAL(find(QString, bool)), this, SLOT(findForReplaceSlot(QString, bool)));
    connect(&replaceDialog, SIGNAL(replace(QString, QString, bool, bool)), this, SLOT(doReplaceSlot(QString, QString, bool, bool)));
    //格式
    connect(ui->actionAutoLine, SIGNAL(triggered()), this, SLOT(autoLineSlot()));
    connect(editor, SIGNAL(cursorPositionChanged()), this, SLOT(cursorChangeSlot()));
    //查看
    connect(ui->actionStatusBar, SIGNAL(triggered()), this, SLOT(statusBarSlot()));
    connect(ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomInSlot()));
    connect(ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOutSlot()));
    //帮助
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(helpSlot()));

    //工具栏

    // 编译运行
    connect(ui->actionCompile, SIGNAL(triggered()), this, SLOT(compileSlot()));
    connect(ui->actionCompileRun, SIGNAL(triggered()), this, SLOT(compileRunSlot()));
    connect(ui->actionRun, SIGNAL(triggered()), this, SLOT(runSlot()));
}
//设置快捷键
void MainWindow::setShortcut() {
    //快捷键
    ui->actionNew->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    ui->actionOpen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    ui->actionSave->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    ui->actionOtherSave->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));

    ui->actionRedo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    ui->actionUndo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y)); //here!
    ui->actionCut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    ui->actionCopy->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    ui->actionPaste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    ui->actionFind->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    ui->actionFindNext->setShortcut(QKeySequence(Qt::Key_F3));
    ui->actionReplace->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
    ui->actionDelete->setShortcut(QKeySequence(Qt::Key_Delete));
    ui->actionSelectAll->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    ui->actionTimeOrDate->setShortcut(QKeySequence(Qt::Key_F5));

    ui->actionCompile->setShortcut(QKeySequence(Qt::Key_F9));
    ui->actionRun->setShortcut(QKeySequence(Qt::Key_F10));
    ui->actionCompileRun->setShortcut(QKeySequence(Qt::Key_F11));
}

//新建文件槽函数
void MainWindow::newFileSolt() {
    //文档已经修改
    if (editor->document()->isModified() && !editor->document()->isEmpty() && fileContent != editor->document()->toPlainText()) {
        QMessageBox box(QMessageBox::Question, "idepad", "是否将更改保存到 无标题");
        box.setIcon(QMessageBox::NoIcon);
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Ignore | QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok, QString("保存"));
        box.setButtonText(QMessageBox::Ignore, QString("不保存"));
        box.setButtonText(QMessageBox::Cancel, QString("取消"));
        int result = box.exec();
        if (result == QMessageBox::Ok) {
            if (fileName.isEmpty()) {
                //新建
                //弹出保存文件对话框
                fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), QDir::homePath(), tr("c files (*.c);;cpp files (*.cpp);;any files (*.*)"));
                if (!fileName.isEmpty()) {
                    //保存文件
                    this->saveTextToFile();
                }
            } else {
                //读取的文本
                this->saveTextToFile();
            }
            editor->clear();
            setWindowTitle("无标题 - idepad");
        } else if (result == QMessageBox::Ignore) {
            //不保存
            editor->clear();
            setWindowTitle("无标题 - idepad");
            ui->statusBar->showMessage("");
        }
    } else {
        //文档未修改
        editor->clear();
        setWindowTitle("无标题 - idepad");
    }
    //路径置空
    fileName = "";
}

//退出应用槽函数
void MainWindow::exitAppSlot() {
    //文档已经修改
    if (editor->document()->isModified() && !editor->document()->isEmpty() && fileContent != editor->document()->toPlainText()) {
        QMessageBox box(QMessageBox::Question, "idepad", "是否将更改保存到 无标题");
        box.setIcon(QMessageBox::NoIcon);
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Ignore | QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok, QString("保存"));
        box.setButtonText(QMessageBox::Ignore, QString("不保存"));
        box.setButtonText(QMessageBox::Cancel, QString("取消"));
        int result = box.exec();
        if (result == QMessageBox::Ok) {
            if (fileName.isEmpty()) {
                //新建
                //弹出保存文件对话框
                fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), QDir::homePath(), tr("c files (*.c);;cpp files (*.cpp);;any files (*.*)"));
                if (!fileName.isEmpty()) {
                    //保存文件
                    this->saveTextToFile();
                }
            } else {
                //读取的文本
                this->saveTextToFile();
            }
        } else if (result == QMessageBox::Ignore) {
            //不保存 关闭
            this->close();
        }
    } else {
        this->close();
    }
}
//另存文件槽函数
void MainWindow::saveOtherFileSlot() {
    //弹出保存文件对话框
    fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), QDir::homePath(), tr("c files (*.c);;cpp files (*.cpp);;any files (*.*)"));
    if (!fileName.isEmpty()) {
        //保存文件
        this->saveTextToFile();
    }
}
//保存文件槽函数
void MainWindow::saveFileSlot() {
    //判断是新建还是读取的文本
    if (fileName.isEmpty()) {
        //新建
        //弹出保存文件对话框
        fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), QDir::homePath(), tr("c files (*.c);;cpp files (*.cpp);;any files (*.*)"));
        if (!fileName.isEmpty()) {
            //保存文件
            this->saveTextToFile();
        }
    } else {
        //读取的文本
        this->saveTextToFile();
    }
}
//保存文件
void MainWindow::saveTextToFile() {
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << editor->document()->toPlainText();
        file.close();
        this->setWindowTitle(fileName.mid(fileName.lastIndexOf('/') + 1) + " - idepad");
        fileContent = editor->document()->toPlainText();
        ui->statusBar->showMessage("已保存", 3000);
    } else {
        QMessageBox box(QMessageBox::Question, "提示", "保存文件失败！");
        box.setIcon(QMessageBox::Warning);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok, QString("确定"));
        box.exec();
    }
}
//打开文件槽函数
void MainWindow::openFileSlot() {
    //文档已经修改
    if (editor->document()->isModified() && !editor->document()->isEmpty() && fileContent != editor->document()->toPlainText()) {
        QMessageBox box(QMessageBox::Question, "idepad", "是否将更改保存到 无标题");
        box.setIcon(QMessageBox::NoIcon);
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Ignore | QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok, QString("保存"));
        box.setButtonText(QMessageBox::Ignore, QString("不保存"));
        box.setButtonText(QMessageBox::Cancel, QString("取消"));
        int result = box.exec();
        if (result == QMessageBox::Ok) {
            //保存文件
            this->saveTextToFile();
        } else if (result == QMessageBox::Ignore) {
            //不保存
            //打开文件
            fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), QDir::homePath(), tr("c files (*.c);;cpp files (*.cpp);;any files (*.*)"));
            this->readFile();
        }
    } else {
        fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), QDir::homePath(), tr("c files (*.c);;cpp files (*.cpp);;any files (*.*)"));
        this->readFile();
    }
}
//真正打开文件
void MainWindow::readFile() {

    //得到路径不为空
    if (!fileName.isEmpty()) {
        QFile *file = new QFile;
        file->setFileName(fileName);

        bool isOpen = file->open(QIODevice::ReadOnly);
        if (isOpen) {
            editor->clear();
            QByteArray array ;
            while(!file->atEnd()) { //判断是否到文件末尾
                //读一行
                array += file->readLine();
            }
            editor->appendPlainText(array);
            fileContent = editor->document()->toPlainText();

            if (fileName.lastIndexOf("\\") != -1) {
                //设置标题
                this->setWindowTitle(fileName.mid(fileName.lastIndexOf('\\') + 1) + " - idepad");
            } else {
                //设置标题
                this->setWindowTitle(fileName.mid(fileName.lastIndexOf('/') + 1) + " - idepad");
            }

            file->close();
            ui->statusBar->showMessage("");
        } else {
            QMessageBox box(QMessageBox::Question, "提示", "打开文件失败！");
            box.setIcon(QMessageBox::Warning);
            box.setStandardButtons(QMessageBox::Ok);
            box.setButtonText(QMessageBox::Ok, QString("确定"));
            box.exec();
        }
    }
}
//文本 变化槽函数
void MainWindow::textChangeSlot() {
    ui->statusBar->showMessage("");
    if (editor->document()->isEmpty()) {
        if (fileName.isEmpty()) {
            //没有保存到文件
            this->setWindowTitle("无标题 - idepad");
        } else {
            //有文件
            this->setWindowTitle(fileName.mid(fileName.lastIndexOf('/') + 1) + " - idepad *");
            if (fileContent != editor->document()->toPlainText()) {
                ui->statusBar->showMessage("未保存");
            }
        }
    } else if (editor->document()->isModified()) {
        if (fileName.isEmpty()) {
            //没有保存到文件
            this->setWindowTitle("无标题 - idepad *");
            if (fileContent != editor->document()->toPlainText()) {
                ui->statusBar->showMessage("未保存");
            }
        } else {
            //有文件
            this->setWindowTitle(fileName.mid(fileName.lastIndexOf('/') + 1) + " - idepad *");
            if (fileContent != editor->document()->toPlainText()) {
                ui->statusBar->showMessage("未保存");
            }
        }
        if (fileContent == editor->document()->toPlainText()) {
            this->setWindowTitle(fileName.mid(fileName.lastIndexOf('/') + 1) + " - idepad");
        }
    }
    if (editor->document()->isEmpty()) {
        ui->actionFind->setEnabled(false);
        ui->actionFindNext->setEnabled(false);
    } else {
        ui->actionFind->setEnabled(true);
        ui->actionFindNext->setEnabled(true);
    }
    //撤销可用
    ui->actionRedo->setEnabled(true);
}

// 撤销槽函数
void MainWindow::editSolt() {
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) {
        //剪切复制删除可用
        ui->actionCut->setEnabled(true);
        ui->actionCopy->setEnabled(true);
        ui->actionDelete->setEnabled(true);
    } else {
        //剪切复制删除不可用
        ui->actionCut->setEnabled(false);
        ui->actionCopy->setEnabled(false);
        ui->actionDelete->setEnabled(false);
    }
}
// 撤销槽函数
void MainWindow::undoSolt() {
    editor->undo();
}
//重做
void MainWindow::redoSolt() {
    editor->redo();
}
//剪切
void MainWindow::cutSolt() {
    editor->cut();
}
//复制
void MainWindow::copySolt() {
    editor->copy();

}
//粘贴
void MainWindow::pasteSolt() {
    editor->paste();
}
//删除
void MainWindow::deleteSolt() {
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) {
        cursor.deleteChar();
        editor->reInit();
    }
}
//插入括号
void MainWindow::Insertbrackets() {
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) {
        cursor.insertText("(" + cursor.selectedText() + ")");
    }
}
//全选
void MainWindow::selectAllSlot() {
    editor->selectAll();
}
//自动换行
void MainWindow::autoLineSlot() {
    if (autoLine) {
        editor->setLineWrapMode(QPlainTextEdit::NoWrap);
        autoLine = false;
    } else {
        editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        autoLine = true;
    }
    ui->actionAutoLine->setIconVisibleInMenu(autoLine);

    setting->beginGroup("config"); //beginGroup与下面endGroup 相对应，“config”是标记
    setting->setValue("auto_go_line", QVariant(autoLine ? "1" : "0"));
    setting->endGroup();
}
//光标变化
void MainWindow::cursorChangeSlot() {
    QTextCursor cursor = editor->textCursor();
    int column = cursor.columnNumber() + 1;
    int block = cursor.blockNumber() + 1;
    change->setText("第 " + QString::number(block) + " 行，第 " + QString::number(column) + " 列 s");
}
//补全括号
void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    QWidget::keyPressEvent(event);
}
//补全括号
void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
}
//状态栏
void MainWindow::statusBarSlot() {
    // statusBar = !statusBar;
    // ui->actionStatusBar->setIconVisibleInMenu(statusBar);
    // ui->statusBar->setVisible(statusBar);

    // setting->beginGroup("config"); //beginGroup与下面endGroup 相对应，“config”是标记
    // setting->setValue("status_bar", QVariant(statusBar ? "1" : "0"));
    // setting->endGroup();
}

//帮助
void MainWindow::helpSlot() {
    //todo
}
//查找槽函数
void MainWindow::findSlot() {
    editor->moveCursor(QTextCursor::Start);
    findDialog.exec();
}
//查找
void MainWindow::find(QString value, bool isChecked, bool isUp) {
    if (isUp) {
        if (!isChecked) {
            if (!editor->find(value, QTextDocument::FindBackward)) {
                showMessage("找不到\"" + value + "\"");
            }
        } else {
            if (!editor->find(value, QTextDocument::FindBackward | QTextDocument::FindCaseSensitively)) {
                showMessage("找不到\"" + value + "\"");
            }
        }
    } else {
        if (!isChecked) {
            if (!editor->find(value)) {
                showMessage("找不到\"" + value + "\"");
            }
        } else {
            if (!editor->find(value, QTextDocument::FindCaseSensitively)) {
                showMessage("找不到\"" + value + "\"");
            }
        }
    }
    QPalette palette = editor->palette();
    palette.setColor(QPalette::Highlight, palette.color(QPalette::Active, QPalette::Highlight));
    editor->setPalette(palette);
}
//替换
void MainWindow::replaceSlot() {
    isFirstFind = true;
    replaceDialog.exec();
}
//进行替换
void MainWindow::replace(QString value, bool isChecked) {
    QTextCursor cursor = editor->textCursor();
    //替换单个值
    cursor.insertText(value);
    //光标移动到前一位
    editor->moveCursor(cursor.PreviousCharacter);
    //是否区分大小写 查找替换后的值高亮
    if (!isChecked) {
        editor->find(value);
    } else {
        editor->find(value, QTextDocument::FindCaseSensitively);
    }
}

void MainWindow::doReplaceSlot(QString target, QString value, bool isChecked, bool isReplaceAll) {
    if (isFirstFind) {
        editor->moveCursor(QTextCursor::Start);
        isFirstFind = false;
    }
    if (!isChecked) {
        if (!editor->find(target)) {
            showMessage("找不到\"" + target + "\"");
            return;
        }
    } else {
        if (!editor->find(target, QTextDocument::FindCaseSensitively)) {
            showMessage("找不到\"" + target + "\"");
            return;
        }
    }
    //选中高亮
    QPalette palette = editor->palette();
    palette.setColor(QPalette::Highlight, palette.color(QPalette::Active, QPalette::Highlight));
    editor->setPalette(palette);
    //替换
    if (isReplaceAll) {
        if (!editor->textCursor().atEnd()) {
            replace(value, isChecked);
            doReplaceSlot(target, value, isChecked, isReplaceAll);
        }
    } else {
        replace(value, isChecked);
    }
}
//查找
void MainWindow::findForReplaceSlot(QString value, bool isChecked) {
    if (isFirstFind) {
        editor->moveCursor(QTextCursor::Start);
        isFirstFind = false;
    }

    this->find(value, isChecked, false);
}
void MainWindow::showMessage(QString title) {
    QMessageBox box(QMessageBox::Question, "idepad - 查找", title);
    box.setIcon(QMessageBox::NoIcon);
    box.setStandardButtons(QMessageBox::Ok);
    box.setButtonText(QMessageBox::Ok, QString("确定"));
    box.setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
    box.exec();
}

//关闭事件
void MainWindow::closeEvent(QCloseEvent *event) {
    //文档已经修改
    if (editor->document()->isModified() && !editor->document()->isEmpty() && fileContent != editor->document()->toPlainText()) {
        QMessageBox box(QMessageBox::Question, "idepad", "是否将更改保存到 无标题");
        box.setIcon(QMessageBox::NoIcon);
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Ignore | QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok, QString("保存"));
        box.setButtonText(QMessageBox::Ignore, QString("不保存"));
        box.setButtonText(QMessageBox::Cancel, QString("取消"));
        int result = box.exec();
        if (result == QMessageBox::Ok) {
            //保存文件
            if (fileName.isEmpty()) {
                //新建
                //弹出保存文件对话框
                fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), QDir::homePath(), tr("c files (*.c);;cpp files (*.cpp);;any files (*.*)"));
                if (!fileName.isEmpty()) {
                    //保存文件
                    this->saveTextToFile();
                }
            } else {
                //读取的文本
                this->saveTextToFile();
            }
            event->accept();
        } else if (result == QMessageBox::Ignore) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}
//放大
void MainWindow::zoomInSlot() {
    zoomInNum++;
    if (zoomInNum <= 10) {
        //最大放大次数为10
        editor->zoomIn(2); //参数为2缩放比例正常
    }
}
//缩小
void MainWindow::zoomOutSlot() {
    zoomInNum--;
    if (zoomInNum >= -5) {
        //最大缩小次数为5
        editor->zoomIn(-2);
    }
}

//连接编译器相关
//预编译
//void MainWindow::precompileSlot() {
//    FILE *p = fopen(fileName.toStdString().data(), "r");
//    if (p == NULL) {
//        return;
//    }
//    QString cmd = fileName;
//    FILE *p1 = fopen(cmd.toStdString().data(), "w");
//    if (p1 == NULL) {
//        return;
//    }
//    QString str;
//    while (!feof(p)) {
//        char buf[1024] = {0};
//        fgets(buf, sizeof(buf), p);
//        str += buf;
//    }
//    fputs(str.toStdString().data(), p1);
//    fclose(p);
//    fclose(p1);
//}

//编译运行
void MainWindow::compileRunSlot() {
    compileSlot();
    runSlot();
}
//运行
void MainWindow::runSlot() {
    QFile file(fileName.mid(0, fileName.indexOf(".")) + ".exe");
    if(file.exists()) {
        system(QString(QCoreApplication::applicationDirPath() + "//ConsolePauser.exe" + " " + fileName.mid(0, fileName.indexOf(".")) + ".exe").toStdString().c_str());
    } else {
        compileSlot();
    }
}

//编译
void MainWindow::compileSlot() {
    saveFileSlot();
    if (fileName.endsWith("cpp")) {
        system(QString(QCoreApplication::applicationDirPath() + "//mingw64//bin//g++.exe" + " " + fileName + " -o " + fileName.mid(0, fileName.indexOf(".")) + ".exe").toStdString().c_str());
    }
}

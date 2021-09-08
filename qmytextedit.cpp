#include "qmytextedit.h"
#include <QTextCursor>

QMyTextEdit::QMyTextEdit(QWidget *parent) : QPlainTextEdit(parent)
{
}
//括号补全
void QMyTextEdit::keyReleaseEvent(QKeyEvent *event)
{
    //    qDebug()<<"QMyTextEdit::keyReleaseEvent";
    //    qDebug()<<"key value:"<< event->key();
    QTextCursor cursor = this->textCursor();
    if (event->key() == Qt::Key_ParenLeft)
    {
        //        qDebug()<<"Qt::Key_ParenLeft";
        cursor.insertText(s + tr(")"));
        reInit();
        cursor.setPosition(cursor.position() - 1); //让光标回到括号里
        setTextCursor(cursor);
        ensureCursorVisible();
    }
    else if (event->key() == Qt::Key_Less)
    {
        //        qDebug()<<"Qt::Key_Less";
        cursor.insertText(s + tr(">"));
        reInit();
        cursor.setPosition(cursor.position() - 1);
        setTextCursor(cursor);
        ensureCursorVisible();
    }
    else if (event->key() == Qt::Key_BraceLeft)
    {
        //        qDebug()<<"Qt::Key_BraceLeft";
        cursor.insertText(s + tr("}"));
        reInit();
        cursor.setPosition(cursor.position() - 1);
        setTextCursor(cursor);
        ensureCursorVisible();
    }
    else if (event->key() == Qt::Key_BracketLeft)
    {
        //        qDebug()<<"Qt::Key_BracketLeft";
        cursor.insertText(s + tr("]"));
        reInit();
        cursor.setPosition(cursor.position() - 1);
        setTextCursor(cursor);
        ensureCursorVisible();
    }
    else if (event->key() == 34) //双引号
    {
        //        qDebug()<<"Key_QuoteLeft";
        cursor.insertText(s + tr("\""));
        reInit();
        cursor.setPosition(cursor.position() - 1);
        setTextCursor(cursor);
        ensureCursorVisible();
    }
    else if (event->key() == 39) //单引号
    {
        //        qDebug()<<"Key_QuoteLeft";
        cursor.insertText(s + tr("\'"));
        reInit();
        cursor.setPosition(cursor.position() - 1);
        setTextCursor(cursor);
        ensureCursorVisible();
    }
    else if (event->key() == Qt::Key_Backspace) //删除
    {
        reInit();
    }
    else if (event->key() == Qt::Key_Delete) //删除
    {
        reInit();
    }
    event->ignore(); //忽略当前事件
}
void QMyTextEdit::mouseReleaseEvent(QMouseEvent *event)
{
    QTextCursor cursor = this->textCursor();
    s = cursor.selectedText();
    qDebug() << s;
}

void QMyTextEdit::reInit()
{
    s = "";
    qDebug() << s;
}

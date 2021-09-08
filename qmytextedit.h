#ifndef QMYTEXTEDIT_H
#define QMYTEXTEDIT_H

#include <QPlainTextEdit>
#include <QtGui>

class QMyTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit QMyTextEdit(QWidget *parent = 0);
    QString s;
    void keyReleaseEvent(QKeyEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void reInit();
};

#endif // QMYTEXTEDIT_H

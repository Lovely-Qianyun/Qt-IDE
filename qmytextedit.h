#ifndef QMYTEXTEDIT_H
#define QMYTEXTEDIT_H

#include <QPlainTextEdit>
#include <QtGui>

class QMyTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit QMyTextEdit(QWidget *parent = 0);

    void keyReleaseEvent(QKeyEvent *event);
};

#endif // QMYTEXTEDIT_H

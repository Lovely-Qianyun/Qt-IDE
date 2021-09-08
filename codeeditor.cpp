#include "codeeditor.h"
#include "qmytextedit.h"
#include <QPainter>
#include <QTextBlock>
#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QSyntaxHighlighter>
#include <QTextCursor>

CodeEditor::CodeEditor(QWidget *parent) : QMyTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    initcompleter();
}

int CodeEditor::lineNumberAreaWidth()
{
    int space = 80;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

//代码高亮
void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    //extraAreaPaintEvent_0
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    //extraAreaPaintEvent_1
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    //extraAreaPaintEvent_2
    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::setCompleter(QCompleter *completer)
{
    //    qDebug()<<"3";
    m_completer = completer;
    if (m_completer)
    {
        m_completer->setWidget(this);
        connect(m_completer, SIGNAL(activated(QString)),
                this, SLOT(onCompleterActivated(QString)));
    }
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    if (m_completer)
    {
        if (m_completer->popup()->isVisible())
        {
            switch (e->key())
            {
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Tab:
                qDebug() << '1' << e->key();
                e->ignore();
                return;
            default:
                qDebug() << '2' << e->key();
                break;
            }
        }
        QPlainTextEdit::keyPressEvent(e);

        if ((e->key() >= 48 && e->key() <= 57) || (e->key() >= 65 && e->key() <= 90) || (e->key() >= 97 && e->key() <= 122) || e->key() == Qt::Key_Backspace)
        {
            //            qDebug()<<"2";
            QString completerPrefix = this->wordUnderCursor();
            if (completerPrefix.length())
            {
                //                qDebug()<<completerPrefix;
                QTextCursor cursor = this->textCursor();
                //                QTextBlock *pLayout = cursor.block();
                m_completer->setCompletionPrefix(completerPrefix);
                //                qDebug()<<this->cursorRect()
                //                qDebug()<<cursor.Left;
                m_completer->complete(QRect(this->cursorRect().left() + 42, this->cursorRect().top(), 100, 15));
            }
            else
                m_completer->popup()->setVisible(false);
        }
        else
        {
            m_completer->popup()->setVisible(false);
            updatecompleter();
        }
    }
    //    qDebug()<<"1";
}

void CodeEditor::updatecompleter()
{
    //    qDebug()<<"4";
    QStringList templist = stringList;
    QRegularExpression pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9]+(?=\\()"));
    QRegularExpressionMatchIterator matchIterator = pattern.globalMatch(this->toPlainText());
    QTextCursor cursor = this->textCursor();
    while (matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();
        QString tmp = match.captured();
        templist << tmp;
        //        qDebug()<<tmp<<pattern;
    }
    templist = templist.toSet().toList(); //去重
                                          //    qDebug()<<stringList;
                                          //    qDebug()<<templist;
    QCompleter *completer = new QCompleter(templist, this);
    this->setCompleter(completer);
}

void CodeEditor::onCompleterActivated(const QString &completion)
{
    QString completionPrefix = wordUnderCursor(),
            shouldInertText = completion;
    QTextCursor cursor = this->textCursor();
    if (!completion.contains(completionPrefix))
    {
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, completionPrefix.size());
        cursor.clearSelection();
    }
    else
    { // 补全字符
        shouldInertText = shouldInertText.replace(
            shouldInertText.indexOf(completionPrefix), completionPrefix.size(), "");
    }
    cursor.insertText(shouldInertText);
}

QString CodeEditor::wordUnderCursor()
{
    QTextCursor cursor = this->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    return cursor.selectedText();
}

void CodeEditor::initcompleter()
{
    stringList << "auto"
               << "break"
               << "case"
               << "char"
               << "const"
               << "continue"
               << "default"
               << "do"
               << "double"
               << "else"
               << "enum"
               << "extern"
               << "float"
               << "for"
               << "goto"
               << "if"
               << "int"
               << "long"
               << "register"
               << "return"
               << "short"
               << "signed"
               << "sizeof"
               << "static"
               << "struct"
               << "switch"
               << "typedef"
               << "union"
               << "unsigned"
               << "void"
               << "volatile"
               << "while";
    QCompleter *completer = new QCompleter(stringList, this);
    this->setCompleter(completer);
}
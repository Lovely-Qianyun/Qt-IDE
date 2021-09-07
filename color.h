#ifndef COLOR_H
#define COLOR_H

#include <QSyntaxHighlighter>

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = nullptr);
    void init();

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    struct HighlightingRule //高亮规则
    {
        QRegExp pattern;        //正则表达式
        QTextCharFormat format; //文本格式
    };
    QList<HighlightingRule> colorrules;
};

#endif

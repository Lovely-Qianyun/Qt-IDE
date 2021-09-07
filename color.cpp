#include "color.h"

#include <QDebug>
#include <QSettings>

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    init();
}

void Highlighter::highlightBlock(const QString &text) //参数是编辑框中输入的内容
{
    foreach (HighlightingRule colorrule, colorrules)
    {
        int index = colorrule.pattern.indexIn(text); //从0开始匹配字符，返回匹配成功的索引
        while (index >= 0)
        {
            int length = colorrule.pattern.matchedLength();
            setFormat(index, length, colorrule.format);              //用高亮规则对应的文本格式设置匹配到的内容
            index = colorrule.pattern.indexIn(text, index + length); //从index + length开始接着匹配当前规则
        }
    }
}

void Highlighter::init()
{
    QSettings settings("config.ini", QSettings::IniFormat);

    //    int size = settings.beginReadArray("colors");
    //    for (int i = 0; i < size; ++i) {
    //        settings.setArrayIndex(i);
    //        HighlightingRule rule;
    //        rule.pattern = settings.value("pattern").value<QRegExp>();
    //        rule.format = settings.value("format").value<QTextFormat>().toCharFormat();
    //        colorrules.append(rule);
    //        qDebug()<<rule.format.font()<<rule.format.foreground();
    //    }
    //    settings.endArray();

    HighlightingRule rule;
    HighlightingRule rule2;
    HighlightingRule rule3;
    QTextCharFormat classFormat;
    QTextCharFormat classFormat2;
    QTextCharFormat classFormat3;

    classFormat.setForeground(QColor("#008000"));
    //    rule.pattern = QRegExp("(?<!\\)//.*"); //不支持零宽断言
    rule.pattern = QRegExp("//.*");
    qDebug() << rule.pattern;
    rule.format = classFormat;

    classFormat2.setForeground(QColor("#008000"));
    rule2.pattern = QRegExp("/\\*.*\\*/");
    rule2.format = classFormat2;
    qDebug() << rule2.pattern;

    rule3.pattern = QRegExp("\\b(auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)\\b");

    classFormat3.setForeground(QColor("#c13980"));
    rule3.format = classFormat3;

    colorrules.append(rule3);
    colorrules.append(rule);
    colorrules.append(rule2);

    //写入配置文件
    settings.beginWriteArray("colors");
    for (int i = 0; i < colorrules.length(); ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("pattern", colorrules.at(i).pattern);
        settings.setValue("format", colorrules.at(i).format);
    }
    settings.endArray();
}

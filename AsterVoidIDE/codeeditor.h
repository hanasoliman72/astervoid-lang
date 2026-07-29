#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

// Syntax Highlighter for AsterVoid
class AsterVoidHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    AsterVoidHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat functionFormat;
};

// Custom Code Editor Widget
class CodeEditor : public QTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    AsterVoidHighlighter *highlighter;
};

#endif // CODEEDITOR_H

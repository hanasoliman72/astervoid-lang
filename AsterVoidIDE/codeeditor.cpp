#include "codeeditor.h"
#include <QKeyEvent>

// AsterVoid Syntax Highlighter Implementation
AsterVoidHighlighter::AsterVoidHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // Keywords format (cyan/blue)
    keywordFormat.setForeground(QColor("#00d4ff"));
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywordPatterns = {
        "\\blaunch\\b", "\\bmass\\b", "\\bflux\\b", "\\bquantum\\b",
        "\\bnebula\\b", "\\bstar\\b", "\\btruth\\b", "\\bvacuum\\b",
        "\\bblackHole\\b", "\\bdarkMatter\\b", "\\bdarkmatter\\b", "\\bwarp\\b",
        "\\brotate\\b", "\\borbit\\b", "\\bphase\\b", "\\beclipse\\b",
        "\\bsupernova\\b", "\\bstarPath\\b", "\\bblackVoid\\b",
        "\\bmoon\\b", "\\bshine\\b", "\\bgalaxy\\b", "\\bearth\\b",
        "\\bconstellation\\b", "\\bconstruct\\b", "\\bdeorbit\\b",
        "\\bshield\\b", "\\brecover\\b", "\\beject\\b",
        "\\bopen\\b", "\\bcore\\b", "\\borbitshield\\b",
        "\\bstarlight\\b", "\\bvoidness\\b"
    };

    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // String format (green)
    stringFormat.setForeground(QColor("#00ff88"));
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = stringFormat;
    highlightingRules.append(rule);

    // Single line comment format (purple)
    commentFormat.setForeground(QColor("#6b4fb5"));
    commentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression("\\*\\*[^\n]*");
    rule.format = commentFormat;
    highlightingRules.append(rule);

    // Multi-line comment format
    rule.pattern = QRegularExpression("\\*\\*\\*.*\\*\\*\\*");
    rule.format = commentFormat;
    highlightingRules.append(rule);

    // Number format (orange)
    numberFormat.setForeground(QColor("#ff6b35"));
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    // Function format (yellow/orange)
    functionFormat.setForeground(QColor("#f77f00"));
    rule.pattern = QRegularExpression("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);
}

void AsterVoidHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

// Code Editor Implementation
CodeEditor::CodeEditor(QWidget *parent)
    : QTextEdit(parent)
{
    // Setup syntax highlighter
    highlighter = new AsterVoidHighlighter(document());

    // Set monospace font
    QFont font("Courier New");
    font.setPointSize(11);
    setFont(font);

    // Tab settings
    setTabStopDistance(40);
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    // Auto-indent on Enter
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        QTextEdit::keyPressEvent(e);

        // Get previous line
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::Up);
        cursor.select(QTextCursor::LineUnderCursor);
        QString previousLine = cursor.selectedText();

        // Count leading spaces/tabs
        int indent = 0;
        for (QChar c : previousLine) {
            if (c == ' ' || c == '\t') {
                indent++;
            } else {
                break;
            }
        }

        // Add extra indent if previous line ends with {
        if (previousLine.trimmed().endsWith('{')) {
            indent += 4;
        }

        // Insert indentation
        cursor = textCursor();
        cursor.insertText(QString(indent, ' '));

        return;
    }

    // Auto-close brackets
    if (e->text() == "{") {
        QTextEdit::keyPressEvent(e);
        insertPlainText("}");
        moveCursor(QTextCursor::Left);
        return;
    }

    if (e->text() == "(") {
        QTextEdit::keyPressEvent(e);
        insertPlainText(")");
        moveCursor(QTextCursor::Left);
        return;
    }

    if (e->text() == "\"") {
        QTextEdit::keyPressEvent(e);
        insertPlainText("\"");
        moveCursor(QTextCursor::Left);
        return;
    }

    QTextEdit::keyPressEvent(e);
}

#include "sqlhighlighter.h"

SQLHighlighter::SQLHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{

}

void SQLHighlighter::setFormatByRegExp(const QRegularExpression &re, const QString &text, const QColor &color)
{
    QRegularExpressionMatchIterator i = re.globalMatch(text);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), color);
    }
}

void SQLHighlighter::highlightBlock(const QString &text)
{
    const QRegularExpression commandsRegexp("\\b(?:select|from|where|and|case|when|then|else|distinct|all|null|"
								 "is|like|between|not|group|by|having|order|inner|outer|right|left|alter|with|isnull|cast|create|replace|function|"
                                 "returns|language|volatile|cost|table|view|or|returning|"
								 "asc|desc|"
                                 "join|on|using|union|exists|in|as|intersect|except|coalesce|insert|into|update)\\b",
                                 QRegularExpression::CaseInsensitiveOption);
    setFormatByRegExp(commandsRegexp, text, QColor(12,70,200));

    const QRegularExpression functionRegexp("\\b[A-Za-z0-9_]+(?=\\()", QRegularExpression::CaseInsensitiveOption);
    setFormatByRegExp(functionRegexp, text, QColor(3,103,128));

    const QRegularExpression numbersRegexp("\\b((\\d+)(\\.)?)",QRegularExpression::CaseInsensitiveOption);
    setFormatByRegExp(numbersRegexp, text, QColor(0,7,194));

    const QRegularExpression commentRegexp("--[^\\n]*");
    setFormatByRegExp(commentRegexp, text, QColor(23,139,23));

    setCurrentBlockState(2);
    QRegularExpression strStartExpression("'.*");
    QRegularExpression strEndExpression ("([^']'|'')");
    int startInd = 0;
    if (previousBlockState() != 3){
        startInd = text.indexOf(strStartExpression);
    }
    while (startInd >= 0) {
        QRegularExpressionMatch match = strEndExpression.match(text, startInd);
        int endIndex = match.capturedStart();
        int strLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(3);
            strLength = text.length() - startInd;
        } else {
            strLength = endIndex - startInd + match.capturedLength();
        }
        setFormat(startInd, strLength, QColor(198,59,133));
        startInd = text.indexOf(strStartExpression, startInd + strLength);
    }

    QRegularExpression commentStartExpression("/\\*");
    QRegularExpression commentEndExpression ("\\*/");
    int startIndex = 0;
    if (previousBlockState() != 1){
        startIndex =text.indexOf(commentStartExpression);
    }
    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, QColor(23,139,23));
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}


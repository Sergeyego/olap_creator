#include "sqlhighlighter.h"

SQLHighlighter::SQLHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{

}

void SQLHighlighter::setFormatByRegExp(const QRegExp &re, const QString &text, const QColor &color)
{
	for (int i = re.indexIn(text, 0); i != -1; i = re.indexIn(text, i + re.matchedLength())) {
		setFormat(i, re.matchedLength(), color);
	}
}

void SQLHighlighter::highlightBlock(const QString &text)
{
	const QRegExp commandsRegexp("\\b(?:select|from|where|and|case|when|then|else|distinct|all|null|"
								 "is|like|between|not|group|by|having|order|inner|outer|right|left|alter|with|isnull|cast|create|replace|function|"
                                 "returns|language|volatile|cost|table|view|or|returning|"
								 "asc|desc|"
								 "join|on|using|union|exists|in|as|intersect|except|coalesce|insert|into|update)\\b",
								 Qt::CaseInsensitive);
    setFormatByRegExp(commandsRegexp, text, QColor(12,70,200));

    const QRegExp functionRegexp("\\b[A-Za-z0-9_]+(?=\\()", Qt::CaseInsensitive);
    setFormatByRegExp(functionRegexp, text, QColor(3,103,128));

    const QRegExp numbersRegexp("\\b((\\d+)(\\.)?)",Qt::CaseInsensitive);
    setFormatByRegExp(numbersRegexp, text, QColor(0,7,194));

    const QRegExp stringsRegexp("'[^']+'",Qt::CaseInsensitive);
    setFormatByRegExp(stringsRegexp, text, QColor(198,59,133));

    const QRegExp commentRegexp("^--[^\n]*");
    setFormatByRegExp(commentRegexp, text, QColor(23,139,23));

    setCurrentBlockState(0);
    QRegularExpression commentStartExpression("/\\*");
    QRegularExpression commentEndExpression ("\\*/");
    int startIndex = 0;
    if (previousBlockState() != 1){
        startIndex = text.indexOf(commentStartExpression);
    }
    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                    + match.capturedLength();
        }
        setFormat(startIndex, commentLength, QColor(23,139,23));
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}


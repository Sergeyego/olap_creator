#ifndef SQLHIGHLIGHTER_H
#define SQLHIGHLIGHTER_H

#include <QtGui/QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QDebug>

class QTextDocument;

class SQLHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	SQLHighlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text);

private:
	Q_DISABLE_COPY(SQLHighlighter)

    void setFormatByRegExp(const QRegularExpression &re, const QString &text, const QColor& color);
};

#endif

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QObject>
#include <QTextBlockUserData>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void setErrRow(int row);
    void createBracketsSelection(int position);

protected:
    void resizeEvent(QResizeEvent *event) override;
    inline QChar character(const QTextCursor &c);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &, int);
    void matchBrackets();

private:
    QWidget *lineNumberArea;
    int errRow;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditor *codeEditor;
};

#endif

#include <QtWidgets>

#include "codeeditor.h"


CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);
    errRow=-1;

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()),this, SLOT(matchBrackets()));

    updateLineNumberAreaWidth(0);
}


int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 15 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::setErrRow(int row)
{
    errRow=row;
    update();
}

void CodeEditor::matchBrackets()
{
    QList <QTextEdit::ExtraSelection> selections;
    setExtraSelections(selections);

    const QChar leftBrack='(';
    const QChar rightBrack=')';
    const QChar app='\'';

    if (character(textCursor())==leftBrack){
        int numberbra=1;
        bool ignorestr=false;
        bool ignorecomment=false;
        bool ignoresingcomment=false;
        createBracketsSelection(textCursor().position());
        QTextCursor cursor = textCursor();
        while (!cursor.atEnd()){
            cursor.movePosition(QTextCursor::NextCharacter);
            QTextCursor nextCursor=cursor;
            nextCursor.movePosition(QTextCursor::NextCharacter);

            if (character(cursor)==app){
                ignorestr=!ignorestr;
            }
            if (character(cursor)=='/'&& character(nextCursor)=='*' && cursor.blockNumber()==nextCursor.blockNumber()){
                ignorecomment=true;
            }
            if (character(cursor)=='*' && character(nextCursor)=='/'&& cursor.blockNumber()==nextCursor.blockNumber()){
                ignorecomment=false;
            }
            if (character(cursor)=='-' && character(nextCursor)=='-'&& cursor.blockNumber()==nextCursor.blockNumber()){
                ignoresingcomment=true;
            }
            if (cursor.blockNumber()!=nextCursor.blockNumber() && ignoresingcomment){
                ignoresingcomment=false;
            }

            if (!ignorestr && !ignorecomment && !ignoresingcomment){
                if (character(cursor)==leftBrack){
                    numberbra++;
                } else if (character(cursor)==rightBrack){
                    numberbra--;
                    if (numberbra==0){
                        createBracketsSelection(cursor.position());
                        break;
                    }
                }
            }
        }
    } else {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::PreviousCharacter);

        if (character(cursor)==rightBrack){
            int numberbra=1;
            bool ignorestr=false;
            bool ignorecomment=false;
            bool ignoresingcomment=false;
            createBracketsSelection(cursor.position());
            while (!cursor.atStart()){
                int oldNum=cursor.blockNumber();
                cursor.movePosition(QTextCursor::PreviousCharacter);
                QTextCursor previousCursor = cursor;
                previousCursor.movePosition(QTextCursor::PreviousCharacter);

                if (character(cursor)==app){
                    ignorestr=!ignorestr;
                }
                if (character(cursor)=='/' && character(previousCursor)=='*' && cursor.blockNumber()==previousCursor.blockNumber()){
                    ignorecomment=true;
                }
                if (character(cursor)=='*' && character(previousCursor)=='/'&& cursor.blockNumber()==previousCursor.blockNumber()){
                    ignorecomment=false;
                }
                if (cursor.blockNumber()!=oldNum){
                    QTextCursor c=cursor;
                    while (!c.atBlockStart()){
                        if (character(c)=='-'){
                            c.movePosition(QTextCursor::PreviousCharacter);
                            if (character(c)=='-'){
                                ignoresingcomment=true;
                                break;
                            }
                        }
                        c.movePosition(QTextCursor::PreviousCharacter);
                    }
                }
                if (character(cursor)=='-' && character(previousCursor)=='-'&& cursor.blockNumber()==previousCursor.blockNumber() && ignoresingcomment){
                    ignoresingcomment=false;
                }

                if (!ignorestr && !ignorecomment && !ignoresingcomment){
                    if (character(cursor)==rightBrack){
                        numberbra++;
                    } else if (character(cursor)==leftBrack){
                        numberbra--;
                        if (numberbra==0){
                            createBracketsSelection(cursor.position());
                            break;
                        }
                    }
                }
            }
        }
    }
}

void CodeEditor::createBracketsSelection(int position)
{
    QList <QTextEdit::ExtraSelection> listSelections = extraSelections();

    QTextEdit::ExtraSelection selection;

    QTextCharFormat format = selection.format;
    format.setForeground(Qt::red);
    format.setBackground(QColor(169,224,169));
    selection.format = format;

    QTextCursor cursor = textCursor();
    cursor.setPosition(position);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

    selection.cursor = cursor;

    listSelections.append(selection);

    setExtraSelections(listSelections);
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

QChar CodeEditor::character(const QTextCursor &c)
{
    QChar ch;
    QTextBlock textBlock = c.block();
    QString text=textBlock.text();
    int pos=c.position() - textBlock.position();
    if (pos>=0 && pos<text.size()){
        ch=text.at(pos);
    }
    return ch;
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), palette().window().color());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(palette().windowText().color());
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),Qt::AlignRight, number);
            if (errRow==blockNumber+1){
                painter.setBrush(Qt::white);
                painter.drawEllipse(0,top+2,fontMetrics().height()-5,fontMetrics().height()-5);
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}



LineNumberArea::LineNumberArea(CodeEditor *editor) : QWidget(editor)
{
    codeEditor = editor;
}

QSize LineNumberArea::sizeHint() const
{
    return QSize(codeEditor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    codeEditor->lineNumberAreaPaintEvent(event);
}

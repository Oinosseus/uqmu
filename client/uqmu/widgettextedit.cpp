#include "widgettextedit.h"
#include <QColor>

WidgetTextEdit::WidgetTextEdit(QWidget *parent) : QTextEdit(parent)
{
    this->setReadOnly(true);
    this->setWordWrapMode(QTextOption::NoWrap);
}

void WidgetTextEdit::appendFileInfo(const QString &msg)
{
    int weigth_tmp = this->fontWeight();
    QColor color_tmp = this->textColor();

    this->setFontWeight(QFont::Black);
    this->setTextColor(QColor(100,100,255));
    this->append(msg);
    this->setFontWeight(weigth_tmp);
    this->setTextColor(color_tmp);
}

void WidgetTextEdit::appendError(const QString &msg)
{
    int weigth_tmp = this->fontWeight();
    QColor color_tmp = this->textColor();

    this->setFontWeight(QFont::Black);
    this->setTextColor(QColor(255,0,0));
    this->append("ERROR: ");
    this->setFontWeight(weigth_tmp);
    this->append(msg);
    this->setTextColor(color_tmp);
}

void WidgetTextEdit::appendDownload(const QString &msg)
{
    QColor color_tmp = this->textColor();

    this->setTextColor(QColor(150,150,0));
    this->append(msg);
    this->setTextColor(color_tmp);
}

void WidgetTextEdit::appendInstall(const QString &msg)
{
    QColor color_tmp = this->textColor();

    this->setTextColor(QColor(0,150,150));
    this->append(msg);
    this->setTextColor(color_tmp);
}

void WidgetTextEdit::appendSuccess(const QString &msg)
{
    int weigth_tmp = this->fontWeight();
    QColor color_tmp = this->textColor();

    this->setFontWeight(QFont::Black);
    this->setTextColor(QColor(0,200,0));
    this->append(msg);
    this->setFontWeight(weigth_tmp);
    this->setTextColor(color_tmp);
}


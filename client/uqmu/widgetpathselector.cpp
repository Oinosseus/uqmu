#include "widgetpathselector.h"
#include <QHBoxLayout>
#include <QFileDialog>

WidgetPathSelector::WidgetPathSelector(const QString &path, QWidget *parent) : QWidget(parent)
{
    lineedit = new QLineEdit(path, this);
    connect(lineedit, SIGNAL(textChanged(QString)), this, SLOT(slotPathEdit(QString)));

    toolbtn = new QToolButton(this);
    toolbtn->setText("...");
    connect(toolbtn, SIGNAL(clicked(bool)), this, SLOT(slotPathButton(bool)));

    QHBoxLayout *layout = new QHBoxLayout();
    this->setLayout(layout);
    layout->addWidget(lineedit, 1);
    layout->addWidget(toolbtn, 0);
}

QString WidgetPathSelector::getPath() const
{
    return lineedit->text();
}

void WidgetPathSelector::setPath(const QString &path)
{
    lineedit->setText(path);
}

void WidgetPathSelector::setEnabled(bool enabled)
{
    this->lineedit->setEnabled(enabled);
    this->toolbtn->setEnabled(enabled);
}

void WidgetPathSelector::setDisabled(bool disabled)
{
    this->lineedit->setDisabled(disabled);
    this->toolbtn->setDisabled(disabled);
}

void WidgetPathSelector::slotPathButton(bool)
{
    QString path = QFileDialog::getExistingDirectory(this, tr("select directory"), lineedit->text());
    if (path.length() > 0) {
        if (lineedit->text() != path) {
            lineedit->setText(path);
            emit signalPathChanged(lineedit->text());
        }
    }
}

void WidgetPathSelector::slotPathEdit(QString path)
{
    if (lineedit->text() != path) {
        lineedit->setText(path);
        emit signalPathChanged(lineedit->text());
    }
}


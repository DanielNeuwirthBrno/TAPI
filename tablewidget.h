#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QKeySequence>
#include <QTableWidget>

class TableWidget: public QTableWidget {

    using QTableWidget::QTableWidget;

    void keyPressEvent(QKeyEvent * event) {

        if (event->matches(QKeySequence::Copy)) {

            const QString cellContents = this->currentItem()->text();
            QApplication::clipboard()->setText(cellContents);
            return;
        }

        if (event->matches(QKeySequence::Paste)) {

            const QString clipboardContents = QApplication::clipboard()->text();
            this->currentItem()->setText(clipboardContents);
            return;
        }

        if (event->key() == Qt::Key_Delete) {

            this->currentItem()->setText(QString());
            return;
        }

        QTableWidget::keyPressEvent(event);
        return;
    }
};

#endif // TABLEWIDGET_H

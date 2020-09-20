/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef TABLEWIDGETITEM_H
#define TABLEWIDGETITEM_H

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QKeySequence>
#include <QTableWidgetItem>

class TableWidgetItem: public QTableWidgetItem {

    public:
        bool eventFilter(QObject * obj, QKeyEvent * event) {

            if (event->matches(QKeySequence::Copy)) {

                QApplication::clipboard()->setText(this->text());
            }

            if (event->matches(QKeySequence::Paste)) {

                this->setText(QApplication::clipboard()->text());
            }

           return QObject::eventFilter(obj, event);
        }


};

#endif // TABLEWIDGETITEM_H

/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef UI_LOGWINDOW_H
#define UI_LOGWINDOW_H

// user interface for LogWindow class

#include <QDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QVector>
#include "request.h"

class Ui_LogWindow {

    public:
        const QStringList headers =
            { QStringLiteral("ID"), QStringLiteral("Datum a čas"), QStringLiteral("HTTP metoda"),
              QStringLiteral("URL adresa"), QStringLiteral("Status")};

        QIcon * logWindowIcon;

        QTableWidget * listOfCommunicationTable;

        QHBoxLayout * buttonsLayout;
        QPushButton * displayRequestButton;
        QPushButton * displayResponseButton;
        QPushButton * closeButton;

        QVBoxLayout * windowLayout;

        void setupUi(QDialog * LogWindow, const QVector<Communication> & communication) {

            const int16_t noOfRows = communication.size();
            const uint8_t noOfColumns = 5;

            // properties of main window
            logWindowIcon = new QIcon(QStringLiteral(":/icons/icons/system-switch-user.png"));
            LogWindow->setWindowIcon(*logWindowIcon);
            LogWindow->resize(0,600);
            const QString title = QStringLiteral("Historie komunikace (celkem záznamů: ") +
                                  QString::number(noOfRows) + QStringLiteral(")");
            LogWindow->setWindowTitle(title);

            // table
            listOfCommunicationTable = new QTableWidget(noOfRows, noOfColumns, LogWindow);
            listOfCommunicationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
            listOfCommunicationTable->setHorizontalHeaderLabels(headers);
            listOfCommunicationTable->verticalHeader()->hide();

            // fill rows
            uint16_t row = 0;
            for (auto it: communication) {

                const QString date = it.createDate().toString("dd.MM.yyyy hh:mm:ss.zzz");
                const QString method = http::convertEnumValueToText(it.request().httpMethod());
                const QString url = it.request().request().url().toDisplayString();
                const QString status =
                    QString::number(static_cast<int>(it.response().statusCode())) + " " +
                    it.response().statusDescription();

                const QStringList description =
                    { QString::number(it.ID()), date, method, url, status };

                for (int i = 0; i < noOfColumns; ++i) {

                    QTableWidgetItem * column = new QTableWidgetItem;
                    column->setText(description.at(i));
                    column->setFlags(column->flags() & ~Qt::ItemIsEditable);

                    if (i == 0)
                        column->setBackground(QBrush(http::httpMethods[method]._color));
                    else
                        switch (it.response().statusCode()) {
                            case TEST: break;
                            case OK: column->setBackground(QBrush(QColor(210,255,166))); break;
                            default: column->setBackground(QBrush(QColor(255,210,210)));
                        }

                    listOfCommunicationTable->setItem(row, i, column);
                }
                ++row;
            }

            for (int i = 0; i < noOfColumns; ++i) {

                listOfCommunicationTable->horizontalHeaderItem(i)->setTextAlignment(Qt::AlignLeft);
                listOfCommunicationTable->resizeColumnToContents(i);
            }

            // buttons
            buttonsLayout = new QHBoxLayout();
            displayRequestButton = new QPushButton(QStringLiteral(" Zobrazit request "));
            displayResponseButton = new QPushButton(QStringLiteral(" Zobrazit response "));
            if (noOfRows == 0) {
                displayRequestButton->setEnabled(false);
                displayResponseButton->setEnabled(false);
            }
            closeButton = new QPushButton(QIcon(QStringLiteral(":/icons/icons/edit-delete.png")),
                                          QStringLiteral("Zavřít"));

            buttonsLayout->addStretch();
            buttonsLayout->addWidget(displayRequestButton);
            buttonsLayout->addWidget(displayResponseButton);
            buttonsLayout->addWidget(closeButton);

            windowLayout = new QVBoxLayout(LogWindow);
            windowLayout->addWidget(listOfCommunicationTable);
            windowLayout->addLayout(buttonsLayout);

            listOfCommunicationTable->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
            LogWindow->adjustSize();

            QMetaObject::connectSlotsByName(LogWindow);
        }
};

#endif // UI_LOGWINDOW_H

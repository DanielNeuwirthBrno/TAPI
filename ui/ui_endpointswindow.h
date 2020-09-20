/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef UI_ENDPOINTSWINDOW_H
#define UI_ENDPOINTSWINDOW_H

// user interface for EndpointsWindow class

#include <QBrush>
#include <QDialog>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSize>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QVector>
#include "endpoint.h"
#include "methods.h"

class Ui_EndpointsWindow {

    public:
        const QStringList headers =
            { QString(), QStringLiteral("Http metoda"),
              QStringLiteral("Endpoint"), QStringLiteral("DTO objekt") };

        QIcon * endpointsWindowIcon;

        QTableWidget * endpointsListTable;
        QVector<QTableWidgetItem *> endpointsListTableItems;
        QHeaderView * header;

        QPlainTextEdit * descriptionTextEdit;

        QHBoxLayout * buttonsLayout;
        QLineEdit * searchBox;
        QPushButton * searchButton;
        QPushButton * inputParamsPathButton;
        QPushButton * inputParamsBodyButton;
        QPushButton * buildRequestButton;
        QPushButton * acceptRequestButton;
        QPushButton * cancelButton;

        QVBoxLayout * windowLayout;

        void setupUi(QDialog * EndpointsWindow, QVector<Endpoint> * const listOfEndpoints) {

            const int16_t noOfRows = listOfEndpoints->size();
            const uint8_t noOfColumns = 4;

            // properties of main window
            endpointsWindowIcon = new QIcon(QStringLiteral(":/icons/icons/system-switch-user.png"));
            EndpointsWindow->setWindowIcon(*endpointsWindowIcon);
            EndpointsWindow->resize(0,600);
            const QString title = QStringLiteral("Přehled dostupných metod API (") +
                                  QString::number(noOfRows) + QStringLiteral(")");
            EndpointsWindow->setWindowTitle(title);

            // table
            endpointsListTable = new QTableWidget(noOfRows, noOfColumns, EndpointsWindow);
            endpointsListTable->setSelectionBehavior(QAbstractItemView::SelectRows);
            endpointsListTable->setColumnWidth(0,15);
            endpointsListTable->setHorizontalHeaderLabels(headers);
            endpointsListTable->verticalHeader()->hide();

            // fill rows
            uint16_t row = 0;
            QTableWidgetItem * currentItem = nullptr;
            bool dtoColumnNotEmpty = false;

            for (auto it: *listOfEndpoints) {

                const QStringList description = { QString(), it.method(), it.path(), it.dtoLabel() };

                for (int i = 0; i < noOfColumns; ++i) {

                    QTableWidgetItem * column = new QTableWidgetItem;
                    column->setText(description.at(i));

                    if (i == 0) {

                        column->setBackground(QBrush(http::httpMethods[it.method().toUpper()]._color));
                        // set cursor to previous position
                        if (it.currentEndpoint() != nullptr && it == *(it.currentEndpoint()))
                            currentItem = column;
                    }

                    if (i == 3 && !dtoColumnNotEmpty && !column->text().isEmpty())
                        dtoColumnNotEmpty = true;

                    if (it.itemNotEligible()) {

                        if (i != 0)
                            column->setBackground(QBrush(QColor(Qt::lightGray)));
                        column->setFlags(Qt::NoItemFlags);
                    }
                    else
                        column->setFlags(column->flags() & ~Qt::ItemIsEditable);

                    endpointsListTable->setItem(row, i, column);
                    endpointsListTableItems.push_back(column);
                }
                ++row;
            }

            for (int i = 1; i <= 3; ++i) {

                endpointsListTable->horizontalHeaderItem(i)->setTextAlignment(Qt::AlignLeft);
                if (i !=3 || dtoColumnNotEmpty)
                    endpointsListTable->resizeColumnToContents(i);
            }

            if (currentItem != nullptr)
                endpointsListTable->setCurrentItem(currentItem);
            header = endpointsListTable->horizontalHeader();
            header->setSectionResizeMode(0, QHeaderView::Fixed);

            // description
            descriptionTextEdit = new QPlainTextEdit;
            descriptionTextEdit->setPlaceholderText(
                QStringLiteral("<zde se po označení řádku zobrazí bližší popis "
                               "vybrané metody (je-li k dispozici)>"));
            descriptionTextEdit->setReadOnly(true);
            QFontMetrics fontMetrics(descriptionTextEdit->font());
            descriptionTextEdit->setFixedHeight(6 * fontMetrics.height());
            descriptionTextEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

            // buttons
            buttonsLayout = new QHBoxLayout();
            searchBox = new QLineEdit;
            searchButton =
                new QPushButton(QIcon(QStringLiteral(":/icons/icons/document-preview.png")), QString());
            inputParamsPathButton = new QPushButton(QStringLiteral(" Parametry (path) "));
            inputParamsPathButton->setEnabled(false);
            inputParamsBodyButton = new QPushButton(QStringLiteral(" Parametry (body) "));
            inputParamsBodyButton->setEnabled(false);
            buildRequestButton = new QPushButton(QStringLiteral(" Sestavit request "));
            acceptRequestButton = new QPushButton(QStringLiteral(" Převzít request "));
            acceptRequestButton->setEnabled(false);
            cancelButton = new QPushButton(QIcon(QStringLiteral(":/icons/icons/edit-delete.png")),
                                           QStringLiteral("Zrušit"));

            buttonsLayout->addWidget(searchBox);
            buttonsLayout->addWidget(searchButton);
            buttonsLayout->addWidget(inputParamsPathButton);
            buttonsLayout->addWidget(inputParamsBodyButton);
            buttonsLayout->addWidget(buildRequestButton);
            buttonsLayout->addWidget(acceptRequestButton);
            buttonsLayout->addWidget(cancelButton);
            buttonsLayout->insertStretch(2);

            windowLayout = new QVBoxLayout(EndpointsWindow);
            windowLayout->addWidget(endpointsListTable);
            windowLayout->addWidget(descriptionTextEdit);
            windowLayout->addLayout(buttonsLayout);

            endpointsListTable->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
            EndpointsWindow->adjustSize();

            QMetaObject::connectSlotsByName(EndpointsWindow);
        }
};

#endif // UI_ENDPOINTSWINDOW_H

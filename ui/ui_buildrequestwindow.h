/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef UI_BUILDREQUESTWINDOW_H
#define UI_BUILDREQUESTWINDOW_H

// user interface for BuildRequestWindow class

#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QMap>
#include <QPair>
#include <QPixmap>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include "endpoint.h"
#include "tablewidget.h"

class Ui_BuildRequestWindow {

    public:
        const QStringList headerNamesParamsTable =
            { QStringLiteral("Parametr"), QStringLiteral("Datový typ"),
              QStringLiteral("Hodnota"), QStringLiteral("Povinný") };
        const QStringList headerNamesAttribsTable =
            { QStringLiteral("Atribut"), QStringLiteral("Datový typ"),
              QStringLiteral("Hodnota"), QStringLiteral("Do dotazu") };

        QIcon * buildRequestWindowIcon;
        QLabel * endpointNameLabel;
        QTableWidgetItem * currentTableWidgetItem;

        // parameters
        QGroupBox * parametersGroupBox;
        QVBoxLayout * parametersLayout;
        TableWidget * parametersListTable;
        QHBoxLayout * parametersButtonLayout;
        QMap<Parameters::paramsState, QPixmap> paramsStateIcons;
        QPushButton * generateParametersButton;
        QPushButton * clearParametersButton;

        // attributes
        QGroupBox * attributesGroupBox;
        QVBoxLayout * attributesLayout;
        TableWidget * attributesListTable;
        QHBoxLayout * attributesButtonLayout;
        QPair<QPixmap, uint8_t> * useInRequestIcon;
        QPair<QPixmap, uint8_t> * notUseInRequestIcon;
        QPushButton * generateAttributesButton;
        QPushButton * clearAttributesButton;
        QPushButton * selectAllButton;
        QPushButton * deselectAllButton;

        QHBoxLayout * buttonsLayout;
        QPushButton * generateButton;
        QPushButton * clearButton;
        QPushButton * confirmButton;

        QVBoxLayout * windowLayout;

        void setNoContent(TableWidget * tableWidget, const uint8_t noOfColumns) {

            for (int i = 0; i < noOfColumns; ++i) {

                currentTableWidgetItem = new QTableWidgetItem;
                currentTableWidgetItem->setBackground(QBrush(QColor(Qt::lightGray)));
                currentTableWidgetItem->setFlags(Qt::NoItemFlags);

                tableWidget->setItem(0, i, currentTableWidgetItem);
            }
            return;
        }

        void setupUi(QDialog * BuildRequestWindow, const Endpoint * const currentEndpoint) {

            // properties of main window
            buildRequestWindowIcon = new QIcon(QStringLiteral(":/icons/icons/system-switch-user.png"));
            BuildRequestWindow->setWindowIcon(*buildRequestWindowIcon);
            // BuildRequestWindow->resize(0,0);
            BuildRequestWindow->setWindowTitle(QStringLiteral("Sestavení requestu"));

            // endpoint name
            endpointNameLabel = new QLabel(currentEndpoint->completePath());
            endpointNameLabel->setTextFormat(Qt::RichText);
            endpointNameLabel->setStyleSheet("font-weight:bold; font-size:16px; color:darkblue;");
            const http::dataFlow flow =
                http::httpMethods[currentEndpoint->method().toUpper()]._dtoObjectType;

            // parameters section (tablewidget)
            parametersGroupBox =
                new QGroupBox(QStringLiteral("Parametry (path)"), BuildRequestWindow);
            parametersGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            const int8_t noOfParams = (currentEndpoint->hasPathParams()) ?
                                      currentEndpoint->parameters()->size() : 1;
            const int8_t noOfColumnsInParamsTable = headerNamesParamsTable.size();
            parametersListTable =
                new TableWidget(noOfParams, noOfColumnsInParamsTable, BuildRequestWindow);
            parametersListTable->setHorizontalHeaderLabels(headerNamesParamsTable);
            parametersListTable->verticalHeader()->hide();

            uint16_t row = 0;
            if (currentEndpoint->hasPathParams()) {

                // icons (Supplied = green, Required = red, NotRequired = blue)
                const QString basicFileName =
                    QStringLiteral(":/icons/icons/preferences-desktop-notification");
                paramsStateIcons.insert(Parameters::SUPPLIED,
                                        QPixmap(basicFileName + QStringLiteral("-green-small.png")));
                paramsStateIcons.insert(Parameters::REQUIRED_AND_NOT_SUPPLIED,
                                        QPixmap(basicFileName + QStringLiteral("-red-small.png")));
                paramsStateIcons.insert(Parameters::NOT_REQUIRED_AND_NOT_SUPPLIED,
                                        QPixmap(basicFileName + QStringLiteral("-blue-small.png")));

                // fill rows
                for (auto it: *(currentEndpoint->parameters())) {

                    const QStringList description =
                        { it.name(), it.type(), it.value().toString(), QStringLiteral("icon") };

                    for (int i = 0; i < noOfColumnsInParamsTable; ++i) {

                        QTableWidgetItem * item = new QTableWidgetItem;
                        item->setText(description.at(i));

                        if (i != 2)
                            item->setFlags(item->flags() & ~Qt::ItemIsEditable);

                        if (description.at(i) == "icon") {

                            QLabel * labelForParamsIcon = new QLabel;
                            const QPixmap * const requiredParamIcon = (!it.value().isNull())
                                ? &paramsStateIcons[Parameters::SUPPLIED]
                                : (it.required()) ? &paramsStateIcons[Parameters::REQUIRED_AND_NOT_SUPPLIED]
                                                  : &paramsStateIcons[Parameters::NOT_REQUIRED_AND_NOT_SUPPLIED];
                            labelForParamsIcon->setPixmap(*requiredParamIcon);
                            labelForParamsIcon->setAlignment(Qt::AlignCenter);

                            parametersListTable->setCellWidget(row, i, labelForParamsIcon);
                        }
                        else

                        parametersListTable->setItem(row, i, item);
                    }
                    ++row;
                }
                // resize columns
                for (int i = 0; i < noOfColumnsInParamsTable - 2 +
                     static_cast<int8_t>(currentEndpoint->isAtLeastOnePathParameterSupplied()); ++i)
                    parametersListTable->resizeColumnToContents(i);
            }
            else
                setNoContent(parametersListTable, noOfColumnsInParamsTable);

            // buttons
            parametersButtonLayout = new QHBoxLayout;
            generateParametersButton = new QPushButton(QStringLiteral("Generovat"));
            clearParametersButton = new QPushButton(QStringLiteral("Vymazat"));
            if (!currentEndpoint->hasPathParams()) {

                generateParametersButton->setHidden(true);
                clearParametersButton->setHidden(true);
            }
            parametersButtonLayout->addStretch();
            parametersButtonLayout->addWidget(generateParametersButton);
            parametersButtonLayout->addWidget(clearParametersButton);

            // layout
            parametersLayout = new QVBoxLayout;
            parametersLayout->addWidget(parametersListTable);
            parametersLayout->addLayout(parametersButtonLayout);
            parametersGroupBox->setLayout(parametersLayout);

            // attributes section (tablewidget)
            attributesGroupBox =
                new QGroupBox(QStringLiteral("Atributy (body)"), BuildRequestWindow);
            const int16_t noOfAttributes = (currentEndpoint->hasBodyAttributes()) ?
                                           currentEndpoint->attributes()->size() : 1;
            const int8_t noOfColumnsInAttribsTable = headerNamesAttribsTable.size();
            attributesListTable =
                new TableWidget(noOfAttributes, noOfColumnsInAttribsTable, BuildRequestWindow);
            attributesListTable->setHorizontalHeaderLabels(headerNamesAttribsTable);
            attributesListTable->verticalHeader()->hide();

            row = 0;
            if (currentEndpoint->hasBodyAttributes()) {

                useInRequestIcon = new QPair<QPixmap, uint8_t>
                    { QPixmap(QStringLiteral(":/icons/icons/dialog-ok-apply.png")), 24 };
                notUseInRequestIcon = new QPair<QPixmap, uint8_t>
                    { QPixmap(QStringLiteral(":/icons/icons/dialog-cancel.png")), 20 };

                // fill rows
                for (auto it: *(currentEndpoint->attributes())) {

                    const QStringList description =
                        { it.name(), it.type(), it.value().toString(), QStringLiteral("icon") };

                    for (int i = 0; i < noOfColumnsInAttribsTable; ++i) {

                        currentTableWidgetItem = new QTableWidgetItem;
                        currentTableWidgetItem->setText(description.at(i));

                        if (i != 2)
                            currentTableWidgetItem->setFlags(currentTableWidgetItem->flags()
                                                             & ~Qt::ItemIsEditable);

                        if (!(flow == http::INPUT) && i == 2) {

                            currentTableWidgetItem->setBackground(QBrush(QColor(Qt::lightGray)));
                            currentTableWidgetItem->setFlags(Qt::NoItemFlags);
                        }

                        if (description.at(i) == "icon") {

                            QLabel * labelForAttribsIcon = new QLabel;
                            const QPair<QPixmap, uint8_t> * requestIcon =
                                (it.useInRequest()) ? useInRequestIcon : notUseInRequestIcon;
                            labelForAttribsIcon->setPixmap
                                (requestIcon->first.scaled(requestIcon->second, requestIcon->second,
                                 Qt::KeepAspectRatio));
                            labelForAttribsIcon->setAlignment(Qt::AlignCenter);

                            attributesListTable->setCellWidget(row, i, labelForAttribsIcon);
                        }
                        else
                            attributesListTable->setItem(row, i, currentTableWidgetItem);
                    }
                    ++row;
                }
                // resize columns
                for (int i = 0; i < noOfColumnsInAttribsTable - 2 +
                     static_cast<int8_t>(currentEndpoint->isAtLeastOneBodyAttributeSupplied()); ++i)
                    attributesListTable->resizeColumnToContents(i);
            }
            else
                setNoContent(attributesListTable, noOfColumnsInAttribsTable);

            // buttons
            attributesButtonLayout = new QHBoxLayout;
            selectAllButton = new QPushButton(QStringLiteral("Označit vše"));
            deselectAllButton = new QPushButton(QStringLiteral("Odznačit vše"));
            generateAttributesButton = new QPushButton(QStringLiteral("Generovat"));
            clearAttributesButton = new QPushButton(QStringLiteral("Vymazat"));
            if (!currentEndpoint->hasBodyAttributes()) {

                selectAllButton->setHidden(true);
                deselectAllButton->setHidden(true);
            }
            if (!currentEndpoint->hasBodyAttributes() || flow == http::OUTPUT) {

                generateAttributesButton->setHidden(true);
                clearAttributesButton->setHidden(true);
            }

            attributesButtonLayout->addStretch();
            attributesButtonLayout->addWidget(selectAllButton);
            attributesButtonLayout->addWidget(deselectAllButton);
            attributesButtonLayout->addWidget(generateAttributesButton);
            attributesButtonLayout->addWidget(clearAttributesButton);

            // unify columns' width
            int parColWidth, attrColWidth;
            for (int i = 0; i < noOfColumnsInAttribsTable; ++i) {

                if ((parColWidth = parametersListTable->columnWidth(i)) >
                    (attrColWidth = attributesListTable->columnWidth(i)))
                    attributesListTable->setColumnWidth(i, parColWidth);
                else
                    parametersListTable->setColumnWidth(i, attrColWidth);
            }

            // layout
            attributesLayout = new QVBoxLayout;
            attributesLayout->addWidget(attributesListTable);
            attributesLayout->addLayout(attributesButtonLayout);
            attributesGroupBox->setLayout(attributesLayout);

            // buttons
            buttonsLayout = new QHBoxLayout();
            generateButton = new QPushButton(QStringLiteral(" Generovat vše "));
            clearButton = new QPushButton(QStringLiteral(" Vymazat vše "));
            if (!currentEndpoint->hasPathParams() ||
                !currentEndpoint->hasBodyAttributes() || flow == http::OUTPUT) {

                generateButton->setHidden(true);
                clearButton->setHidden(true);
            }
            confirmButton = new QPushButton(QIcon(QStringLiteral(":/icons/icons/dialog-ok-apply.png")),
                                            QStringLiteral("Hotovo"));

            buttonsLayout->addStretch();
            buttonsLayout->addWidget(generateButton);
            buttonsLayout->addWidget(clearButton);
            buttonsLayout->addWidget(confirmButton);

            windowLayout = new QVBoxLayout(BuildRequestWindow);
            windowLayout->addWidget(endpointNameLabel);
            windowLayout->addWidget(parametersGroupBox);
            windowLayout->addWidget(attributesGroupBox);
            windowLayout->addLayout(buttonsLayout);

            parametersListTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            parametersListTable->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
            attributesListTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            attributesListTable->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
            BuildRequestWindow->resize(BuildRequestWindow->width(), BuildRequestWindow->parentWidget()->height());

            QMetaObject::connectSlotsByName(BuildRequestWindow);
        }
};

#endif // UI_BUILDREQUESTWINDOW_H

/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef UI_PATHWINDOW_H
#define UI_PATHWINDOW_H

// user interface for PathWindow class

#include <QDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include "endpoint.h"

class Ui_PathWindow {

    public:
        QIcon * pathWindowIcon;

        QLabel * endpointNameLabel;
        QGridLayout * parametersLayout;

        QHBoxLayout * buttonsLayout;
        QPushButton * confirmButton;
        QPushButton * cancelButton;

        QVBoxLayout * windowLayout;

        void setupUi(QDialog * PathWindow, const Endpoint & currentEndpoint) {

            // properties of main window
            pathWindowIcon = new QIcon(QStringLiteral(":/icons/icons/system-switch-user.png"));
            PathWindow->setWindowIcon(*pathWindowIcon);
            // PathWindow->resize(0,0);
            PathWindow->setWindowTitle(QStringLiteral("Seznam parametrů (path)"));

            // endpoint name
            endpointNameLabel = new QLabel(currentEndpoint.completePath());
            endpointNameLabel->setTextFormat(Qt::RichText);
            endpointNameLabel->setStyleSheet("font-weight:bold; font-size:16px; color:darkblue;");

            // parameters
            parametersLayout = new QGridLayout();
            const int noOfParams = currentEndpoint.parameters()->size();

            for (int i = 0; i < noOfParams; ++i) {

                const Parameters currentParameter = currentEndpoint.parameters()->at(i);

                const QString name =
                    QStringLiteral("<b>") + currentParameter.name() + QStringLiteral("</b>");
                QLabel * nameLabel = new QLabel(name);
                nameLabel->setTextFormat(Qt::RichText);
                const QString type = currentParameter.type();
                QLabel * typeLabel = new QLabel(type);
                const QString value =
                    (!currentParameter.value().toString().isEmpty()) ?
                    currentParameter.value().toString() : QStringLiteral("<prázdná hodnota>");
                QLineEdit * valueLineEdit = new QLineEdit(value);
                valueLineEdit->setReadOnly(true);
                const QString required =
                    currentParameter.required() ? QStringLiteral("*") : QString();
                QLabel * requiredLabel = new QLabel(required);
                requiredLabel->setTextFormat(Qt::RichText);
                requiredLabel->setStyleSheet("color: red;");

                parametersLayout->addWidget(nameLabel, i, 0);
                parametersLayout->addWidget(typeLabel, i, 1);
                parametersLayout->addWidget(valueLineEdit, i, 2);
                parametersLayout->addWidget(requiredLabel, i, 3);
            }

            // buttons
            buttonsLayout = new QHBoxLayout();
            confirmButton = new QPushButton(QStringLiteral("Potvrdit"));
            cancelButton = new QPushButton(QIcon(QStringLiteral(":/icons/icons/edit-delete.png")),
                                           QStringLiteral("Zrušit"));

            buttonsLayout->addStretch();
            buttonsLayout->addWidget(confirmButton);
            buttonsLayout->addWidget(cancelButton);

            windowLayout = new QVBoxLayout(PathWindow);
            windowLayout->addWidget(endpointNameLabel);
            windowLayout->addLayout(parametersLayout);
            windowLayout->addLayout(buttonsLayout);

            QMetaObject::connectSlotsByName(PathWindow);
        }
};

#endif // UI_PATHWINDOW_H

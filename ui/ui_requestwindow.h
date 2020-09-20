/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef UI_REQUESTWINDOW_H
#define UI_REQUESTWINDOW_H

// user interface for RequestWindow class

#include <QDialog>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSize>
#include <QTextEdit>
#include <QVariant>
#include <QVBoxLayout>
#include "request.h"

class Ui_RequestWindow {

    public:
        QIcon * requestWindowIcon;

        QLabel * endpointNameLabel;

        QHBoxLayout * priorityLayout;
        QLabel * priorityDescriptionLabel;
        QLabel * priorityValueLabel;
        QLabel * redirectsLabel;
        QLabel * redirectsValueLabel;
        QPushButton * httpMethodButton;

        QLineEdit * requestQueryLineEdit;
        QTextEdit * requestHeaderTextEdit;
        QTextEdit * requestBodyTextEdit;

        QHBoxLayout * buttonsLayout;
        QPushButton * closeButton;

        QVBoxLayout * windowLayout;

        void setupUi(QDialog * RequestWindow, const Request & request, const QVariant ID) {

            // properties of main window
            requestWindowIcon = new QIcon(QStringLiteral(":/icons/icons/system-switch-user.png"));
            RequestWindow->setWindowIcon(*requestWindowIcon);
            RequestWindow->resize(600,0);
            QString baseTitle = QStringLiteral("Zaslaný dotaz");
            const QString title = (ID.isNull()) ? baseTitle : baseTitle + " (" + ID.toString() + ")";
            RequestWindow->setWindowTitle(title);

            // endpoint name
            const QString url = request.request().url().toDisplayString(QUrl::RemoveQuery);
            endpointNameLabel = new QLabel(url);
            endpointNameLabel->setTextFormat(Qt::RichText);
            endpointNameLabel->setStyleSheet("font-weight:bold; font-size:16px; color:darkblue;");
            endpointNameLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

            // priority, redirects
            priorityLayout = new QHBoxLayout;
            priorityDescriptionLabel = new QLabel("Priorita: ");
            QString priorityText;
            priorityValueLabel = new QLabel(priorityText.setNum(request.request().priority()));
            redirectsLabel = new QLabel(" Přesměrování (max.): ");
            QString redirectsText;
            redirectsValueLabel =
                new QLabel(redirectsText.setNum(request.request().maximumRedirectsAllowed()));

            const QString methodButtonLabel = http::convertEnumValueToText(request.httpMethod());
            httpMethodButton = new QPushButton(methodButtonLabel);
            const QColor backgroundColor = http::httpMethods[methodButtonLabel]._color;
            const QString background = QStringLiteral(" font-weight: bold; background-color: %1;").
                                                      arg(backgroundColor.name());
            httpMethodButton->setStyleSheet(background);
            const QString iconPath = QStringLiteral(
                 ":/icons/icons/") + http::httpMethods[methodButtonLabel]._iconPath;
            httpMethodButton->setIcon(QIcon(iconPath));
            httpMethodButton->setDisabled(true);

            priorityLayout->addWidget(priorityDescriptionLabel);
            priorityLayout->addWidget(priorityValueLabel);
            priorityLayout->addWidget(redirectsLabel);
            priorityLayout->addWidget(redirectsValueLabel);
            priorityLayout->addStretch();
            priorityLayout->addWidget(httpMethodButton);

            // request query contents (following ?)
            const QString query = request.request().url().query(QUrl::PrettyDecoded);
            requestQueryLineEdit = new QLineEdit(query);
            requestQueryLineEdit->setReadOnly(true);
            requestQueryLineEdit->home(false);
            requestQueryLineEdit->setStyleSheet("background-color:lightgray;");
            requestQueryLineEdit->setVisible(!query.isEmpty());

            // request header contents
            requestHeaderTextEdit = new QTextEdit;
            requestHeaderTextEdit->setReadOnly(true);
            requestHeaderTextEdit->setPlaceholderText(QStringLiteral("prázdné"));
            for (auto it_header: request.request().rawHeaderList()) {

                const QString headerContent = request.request().rawHeader(it_header);
                const QString completeHeader = QString::fromLocal8Bit(it_header) +
                                               QStringLiteral(": ") + headerContent;
                requestHeaderTextEdit->append(completeHeader);
            }
            QFontMetrics fontMetrics = requestHeaderTextEdit->fontMetrics();
            QSize size = requestHeaderTextEdit->size();
            uint16_t height = fontMetrics.height() * request.request().rawHeaderList().count();
            requestHeaderTextEdit->resize(size.width(), height);
            requestHeaderTextEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

            // request body contents
            requestBodyTextEdit = new QTextEdit;
            requestBodyTextEdit->setReadOnly(true);
            requestBodyTextEdit->setWordWrapMode(QTextOption::WrapAnywhere);
            requestBodyTextEdit->setPlaceholderText(QStringLiteral("prázdné"));
            const QString bodyContents(request.body());
            if (!bodyContents.isEmpty())
                requestBodyTextEdit->setPlainText(bodyContents);
            fontMetrics = requestBodyTextEdit->fontMetrics();
            size = requestBodyTextEdit->size();
            height = fontMetrics.height() * 10;
            if (height > size.height())
                requestBodyTextEdit->resize(size.width(), height);
            requestBodyTextEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

            // buttons
            buttonsLayout = new QHBoxLayout;
            closeButton = new QPushButton(QIcon(QStringLiteral(":/icons/icons/edit-delete.png")),
                                          QStringLiteral("Zavřít"));
            buttonsLayout->addStretch();
            buttonsLayout->addWidget(closeButton);

            windowLayout = new QVBoxLayout(RequestWindow);
            windowLayout->addWidget(endpointNameLabel);
            windowLayout->addLayout(priorityLayout);
            windowLayout->addWidget(requestQueryLineEdit);
            windowLayout->addWidget(requestHeaderTextEdit);
            windowLayout->addWidget(requestBodyTextEdit);
            windowLayout->addLayout(buttonsLayout);

            QMetaObject::connectSlotsByName(RequestWindow);
        }
};

#endif // UI_REQUESTWINDOW_H

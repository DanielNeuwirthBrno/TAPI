/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef UI_RESPONSEWINDOW_H
#define UI_RESPONSEWINDOW_H

// user interface for ResponseWindow class

#include <QDialog>
#include <QFontMetrics>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSize>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QVector>
#include "request.h"

class Ui_ResponseWindow {

    public:
        QIcon * responseWindowIcon;

        QLabel * endpointNameLabel;

        QHBoxLayout * statusLayout;
        QLabel * statusCodeLabel;
        QLabel * statusDescriptionLabel;
        QPushButton * httpMethodButton;

        QLineEdit * responseQueryLineEdit;
        QTextEdit * responseHeaderTextEdit;
        QTextEdit * responseBodyTextEdit;

        QGridLayout * stateAttributesLayout;
        QLabel * pageCountLabel;
        QLabel * pageCountValueLabel;
        QLabel * rowCountLabel;
        QLabel * rowCountValueLabel;
        QLabel * statusLabel;
        QLabel * statusValueLabel;
        QLabel * messageLabel;
        QLineEdit * messageValueLineEdit;
        QLabel * stackTraceLabel;
        QLineEdit * stackTraceValueLineEdit;

        QHBoxLayout * buttonsLayout;
        QLineEdit * tableNameLineEdit;
        QPushButton * verifyButton;
        QPushButton * closeButton;

        QVBoxLayout * windowLayout;

        void setupUi(QDialog * ResponseWindow, const QNetworkReply * const reply,
                     const QVariant & ID, QList<QString> & recordIDs, Communication * const comm) {

            const QList<QNetworkReply::RawHeaderPair> headers = reply->rawHeaderPairs();
            Response * const currentResponse = new Response;
            *currentResponse = comm->response();

            // properties of main window
            responseWindowIcon = new QIcon(QStringLiteral(":/icons/icons/system-switch-user.png"));
            ResponseWindow->setWindowIcon(*responseWindowIcon);
            ResponseWindow->resize(600,0);
            QString baseTitle = QStringLiteral("Odpověd na zaslaný dotaz");
            const QString title = (ID.isNull()) ? baseTitle : baseTitle + " (" + ID.toString() + ")";
            ResponseWindow->setWindowTitle(title);

            // endpoint name
            const QString url = reply->request().url().toDisplayString(QUrl::RemoveQuery);
            endpointNameLabel = new QLabel(url);
            endpointNameLabel->setTextFormat(Qt::RichText);
            endpointNameLabel->setStyleSheet("font-weight:bold; font-size:16px; color:darkblue;");
            endpointNameLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

            // status
            statusLayout = new QHBoxLayout;
            const QString style = QStringLiteral("font-weight:bold; font-size:12px;");
            statusCodeLabel = new QLabel(QString::number(currentResponse->statusCode()));
            statusCodeLabel->setTextFormat(Qt::RichText);
            const QString textColour = (currentResponse->statusCode() == OK) ?
                QStringLiteral(" color:green;") : QStringLiteral(" color:red;");
            statusCodeLabel->setStyleSheet(style+textColour);
            statusCodeLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

            statusDescriptionLabel = new QLabel(currentResponse->statusDescription());
            statusDescriptionLabel->setTextFormat(Qt::RichText);
            statusDescriptionLabel->setStyleSheet(style);
            statusDescriptionLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

            // http method button (coloured)
            const QString methodButtonLabel =
                http::convertEnumValueToText(comm->request().httpMethod());
            httpMethodButton = new QPushButton(methodButtonLabel);

            if (!methodButtonLabel.isNull()) {

                const QColor backgroundColor = http::httpMethods[methodButtonLabel]._color;
                const QString background = QStringLiteral(
                     " font-weight: bold; background-color: %1;").arg(backgroundColor.name());
                httpMethodButton->setStyleSheet(background);

                const QString iconPath = QStringLiteral(
                     ":/icons/icons/") + http::httpMethods[methodButtonLabel]._iconPath;
                httpMethodButton->setIcon(QIcon(iconPath));
            }
            else
                httpMethodButton->setHidden(true);

            statusLayout->addWidget(statusCodeLabel);
            statusLayout->addWidget(statusDescriptionLabel);
            statusLayout->addStretch();
            statusLayout->addWidget(httpMethodButton);
            statusLayout->setSizeConstraint(QLayout::SetFixedSize);

            // response query contents (following ?)
            const QString query = reply->request().url().query(QUrl::PrettyDecoded);
            responseQueryLineEdit = new QLineEdit(query);
            responseQueryLineEdit->setReadOnly(true);
            responseQueryLineEdit->home(false);
            responseQueryLineEdit->setStyleSheet("background-color:lightgray;");
            responseQueryLineEdit->setVisible(!query.isEmpty());

            // response header contents
            responseHeaderTextEdit = new QTextEdit;
            responseHeaderTextEdit->setReadOnly(true);
            responseHeaderTextEdit->setPlaceholderText(QStringLiteral("prázdné"));
            for (auto it_header: currentResponse->headers())
               responseHeaderTextEdit->append(QString::fromLocal8Bit(it_header.first) + " " +
                                              QString::fromLocal8Bit(it_header.second));
            QFontMetrics fontMetrics = responseHeaderTextEdit->fontMetrics();
            QSize size = responseHeaderTextEdit->size();
            uint16_t height = fontMetrics.height() * headers.count();
            responseHeaderTextEdit->resize(size.width(), height);
            responseHeaderTextEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

            // response body contents
            responseBodyTextEdit = new QTextEdit;
            responseBodyTextEdit->setReadOnly(true);
            responseBodyTextEdit->setWordWrapMode(QTextOption::WrapAnywhere);
            responseBodyTextEdit->setPlaceholderText(QStringLiteral("prázdné"));
            QString bodyContents = currentResponse->dataFromBody(recordIDs);
            if (bodyContents.isNull() && currentResponse->stateAttributes().status.isNull())
                bodyContents = currentResponse->response();
            responseBodyTextEdit->setPlainText(bodyContents);
            fontMetrics = responseBodyTextEdit->fontMetrics();
            size = responseBodyTextEdit->size();
            height = fontMetrics.height() * 10;
            if (height > size.height())
              responseBodyTextEdit->resize(size.width(), height);
            responseBodyTextEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

            // response state attributes
            uint8_t rowNo = 0;
            stateAttributesLayout = new QGridLayout;

            if (!currentResponse->stateAttributes().pageCount.isNull()) {

                pageCountLabel = new QLabel(QStringLiteral("PageCount"));
                pageCountValueLabel = new QLabel(currentResponse->stateAttributes().pageCount);
                stateAttributesLayout->addWidget(pageCountLabel, rowNo, 0);
                stateAttributesLayout->addWidget(pageCountValueLabel, rowNo++, 1);
            }
            if (!currentResponse->stateAttributes().rowCount.isNull()) {

                rowCountLabel = new QLabel(QStringLiteral("RowCount"));
                rowCountValueLabel = new QLabel(currentResponse->stateAttributes().rowCount);
                stateAttributesLayout->addWidget(rowCountLabel, rowNo, 0);
                stateAttributesLayout->addWidget(rowCountValueLabel, rowNo++, 1);
            }
            if (!currentResponse->stateAttributes().status.isNull()) {

                statusLabel = new QLabel(QStringLiteral("Status"));
                statusValueLabel = new QLabel(currentResponse->stateAttributes().status);
                stateAttributesLayout->addWidget(statusLabel, rowNo, 0);
                stateAttributesLayout->addWidget(statusValueLabel, rowNo++, 1);
            }
            if (!currentResponse->stateAttributes().message.isNull()) {

                messageLabel = new QLabel(QStringLiteral("Message"));
                messageValueLineEdit = new QLineEdit(currentResponse->stateAttributes().message);
                stateAttributesLayout->addWidget(messageLabel, rowNo, 0);
                stateAttributesLayout->addWidget(messageValueLineEdit, rowNo++, 1);
            }
            if (!currentResponse->stateAttributes().stackTrace.isNull()) {

                stackTraceLabel = new QLabel(QStringLiteral("StackTrace"));
                stackTraceValueLineEdit = new QLineEdit(currentResponse->stateAttributes().stackTrace);
                stateAttributesLayout->addWidget(stackTraceLabel, rowNo, 0);
               stateAttributesLayout->addWidget(stackTraceValueLineEdit, rowNo++, 1);
            }

            // buttons
            buttonsLayout = new QHBoxLayout;
            tableNameLineEdit = new QLineEdit;
            tableNameLineEdit->setHidden(true);
            verifyButton = new QPushButton(QIcon(QStringLiteral(
                ":/icons/icons/preferences-desktop-notification.png")), QStringLiteral(" Neověřeno "));
            if (recordIDs.isEmpty())
                verifyButton->setHidden(true);
            closeButton = new QPushButton(QIcon(QStringLiteral(":/icons/icons/edit-delete.png")),
                                          QStringLiteral("Zavřít"));
            buttonsLayout->addStretch();
            buttonsLayout->addWidget(tableNameLineEdit);
            buttonsLayout->addWidget(verifyButton);
            buttonsLayout->addWidget(closeButton);

            windowLayout = new QVBoxLayout(ResponseWindow);
            windowLayout->addWidget(endpointNameLabel);
            windowLayout->addLayout(statusLayout);
            windowLayout->addWidget(responseQueryLineEdit);
            windowLayout->addWidget(responseHeaderTextEdit);
            windowLayout->addWidget(responseBodyTextEdit);
            windowLayout->addLayout(stateAttributesLayout);
            windowLayout->addLayout(buttonsLayout);

            QMetaObject::connectSlotsByName(ResponseWindow);
        }
};

#endif // UI_RESPONSEWINDOW_H

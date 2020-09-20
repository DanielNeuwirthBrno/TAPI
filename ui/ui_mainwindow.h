/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

// user interface for MainWindow class

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFontMetrics>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPixmap>
#include <QPushButton>
#include <QRegularExpression>
#include <QSize>
#include <QValidator>
#include <QVBoxLayout>
#include "connection.h"
#include "methods.h"

class Ui_MainWindow {

    public:
        QIcon * mainIcon;
        QVBoxLayout * windowLayout;

        // authorization
        QGroupBox * authGroupBox;
        QVBoxLayout * authLayout;

        QHBoxLayout * configFileLayout;
        QLabel * selectConfigFileLabel;
        QLineEdit * selectConfigFileLineEdit;
        QPushButton * selectConfigFileButton;

        // sql server
        QHBoxLayout * sqlServerLayout;
        QGroupBox * sqlConnectionGroupBox;
        QVBoxLayout * sqlConnectionLayout;
        QHBoxLayout * sqlConnectionFirstRowLayout;
        QHBoxLayout * sqlConnectionSecondRowLayout;

        QLabel * serverLabel;
        QLineEdit * serverLineEdit;
        QLabel * userLabel;
        QLineEdit * userLineEdit;
        QLabel * passwordLabel;
        QLineEdit * passwordLineEdit;
        QPushButton * connectToServerButton;

        QLabel * agendaDbLabel;
        QLineEdit * agendaDbLineEdit;
        QLabel * systemDbLabel;
        QLineEdit * systemDbLineEdit;
        QLabel * userS5Label;
        QLineEdit * userS5LineEdit;

        QHBoxLayout * clientCredentialsLayout;
        QLabel * clientIDLabel;
        QLineEdit * clientIDLineEdit;
        QLabel * clientSecretLabel;
        QLineEdit * clientSecretLineEdit;

        // api
        QGroupBox * apiGroupBox;
        QHBoxLayout * apiLayout;
        QLabel * apiAddressLabel;
        QComboBox * apiProtocolComboBox;
        QLineEdit * apiHostNameLineEdit;
        QValidator * apiHostNameValidator;
        QLineEdit * apiPortLineEdit;
        QPushButton * apiTestConnectionButton;
        QLabel * apiTestResultIcon;
        QPushButton * apiGetEndpointsButton;

        // token
        QGroupBox * tokenGroupBox;
        QVBoxLayout * tokenLayout;

        QHBoxLayout * tokenValueLayout;
        QLabel * tokenLabel;
        QLineEdit * tokenTypeLineEdit;
        QLineEdit * tokenLineEdit;

        QHBoxLayout * tokenParamsLayout;
        QLabel * timeOfExpiryFromLabel;
        QLineEdit * timeOfExpiryFromLineEdit;
        QLabel * timeOfExpiryToLabel;
        QLineEdit * timeOfExpiryToLineEdit;
        QPushButton * generateTokenButton;
        QPushButton * viewTokenButton;

        // documentation
        QGroupBox * docGroupBox;
        QHBoxLayout * docLayout;
        QLabel * swaggerLabel;
        QLineEdit * swaggerWebLocationLineEdit;
        QLineEdit * swaggerFileLocationLineEdit;
        QPushButton * changeSwaggerLocationButton;
        QPushButton * swaggerFromWebButton;
        QPushButton * swaggerFromFileButton;

        // request
        QGroupBox * requestGroupBox;
        QVBoxLayout * requestLayout;

        QHBoxLayout * requestEndpointLayout;
        QLabel * requestLabel;
        QComboBox * requestMethodComboBox;
        QLineEdit * requestSelectedEndpointLineEdit;
        QPushButton * requestSelectEndpointButton;
        QComboBox * requestAcceptFormatComboBox;
        QPushButton * requestSendButton;

        QWidget * requestSelectAndFilterWidget; // allows disabling/hiding
        QGridLayout * requestSelectAndFilterLayout;
        QLabel * selectLabel;
        QLineEdit * selectConditionLineEdit;
        QCheckBox * useOwnSelectConditionCheckBox;
        QLabel * filterLabel;
        QLineEdit * filterConditionLineEdit;
        QCheckBox * useOwnFilterConditionCheckBox;

        // buttons
        QHBoxLayout * buttonsLayout;
        QCheckBox * useProxyCheckBox;
        QLabel * useProxyLabel;
        QCheckBox * testModeCheckBox;
        QLabel * testModeLabel;
        QPushButton * logButton;
        QPushButton * quitButton;

        void resizeLineWidget(QLineEdit * const lineEdit, const QString & time) {

            lineEdit->setText(time);
            const QFontMetrics fontMetrics = lineEdit->fontMetrics();
            const QSize size = lineEdit->size();
            const uint16_t width = fontMetrics.horizontalAdvance(time)*1.15;
            lineEdit->resize(width, size.height());

            return;
        }

        void setupUi(QDialog * MainWindow) {

            // properties of main window
            mainIcon = new QIcon(QStringLiteral(":/icons/icons/system-switch-user.png"));
            MainWindow->setWindowIcon(*mainIcon);
            MainWindow->setWindowFlags(MainWindow->windowFlags() | Qt::WindowMinimizeButtonHint);
            MainWindow->setWindowTitle(QStringLiteral("Test S5API"));
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
            MainWindow->resize(700,0);

            // authorization section
            authGroupBox = new QGroupBox(QStringLiteral("Přihlašovací údaje"), MainWindow);
            authLayout = new QVBoxLayout;
            // first row
            selectConfigFileLabel = new QLabel(QStringLiteral("Config (umístění)"));
            selectConfigFileLineEdit = new QLineEdit;
            selectConfigFileButton = new QPushButton
                (QIcon(QStringLiteral(":/icons/icons/folder-development.png")),
                 QStringLiteral("Načíst ze souboru"));
            // layout
            configFileLayout = new QHBoxLayout;
            configFileLayout->addWidget(selectConfigFileLabel);
            configFileLayout->addWidget(selectConfigFileLineEdit,3);
            configFileLayout->addWidget(selectConfigFileButton,1);
            // second row
            // sql connection section
            sqlConnectionGroupBox = new QGroupBox(QStringLiteral("SQL server"), MainWindow);
            sqlConnectionLayout = new QVBoxLayout;
            // first row (child)
            serverLabel = new QLabel(QStringLiteral("Server"));
            serverLineEdit = new QLineEdit;
            userLabel = new QLabel(QStringLiteral("Uživatel"));
            userLineEdit = new QLineEdit;
            passwordLabel = new QLabel(QStringLiteral("Heslo"));
            passwordLineEdit = new QLineEdit;
            passwordLineEdit->setEchoMode(QLineEdit::Password);
            connectToServerButton = new QPushButton
                (QIcon(QStringLiteral(":/icons/icons/call-start.png")),
                 QStringLiteral("Připojit"));
            connectToServerButton->setEnabled(false);
            // layout
            sqlConnectionFirstRowLayout = new QHBoxLayout;
            sqlConnectionFirstRowLayout->addWidget(serverLabel);
            sqlConnectionFirstRowLayout->addWidget(serverLineEdit);
            sqlConnectionFirstRowLayout->addWidget(userLabel);
            sqlConnectionFirstRowLayout->addWidget(userLineEdit);
            sqlConnectionFirstRowLayout->addWidget(passwordLabel);
            sqlConnectionFirstRowLayout->addWidget(passwordLineEdit);
            sqlConnectionFirstRowLayout->addWidget(connectToServerButton);
            // second row (child)
            agendaDbLabel = new QLabel(QStringLiteral("Agenda DB"));
            agendaDbLineEdit = new QLineEdit;
            systemDbLabel = new QLabel(QStringLiteral("Systém DB"));
            systemDbLineEdit = new QLineEdit;
            userS5Label = new QLabel(QStringLiteral("Uživatel S5"));
            userS5LineEdit = new QLineEdit;
            // layout
            sqlConnectionSecondRowLayout = new QHBoxLayout;
            sqlConnectionSecondRowLayout->addWidget(agendaDbLabel);
            sqlConnectionSecondRowLayout->addWidget(agendaDbLineEdit);
            sqlConnectionSecondRowLayout->addWidget(systemDbLabel);
            sqlConnectionSecondRowLayout->addWidget(systemDbLineEdit);
            sqlConnectionSecondRowLayout->addWidget(userS5Label);
            sqlConnectionSecondRowLayout->addWidget(userS5LineEdit);
            // sql connection section layout
            sqlConnectionLayout->addLayout(sqlConnectionFirstRowLayout);
            sqlConnectionLayout->addLayout(sqlConnectionSecondRowLayout);
            // layout
            sqlConnectionGroupBox->setLayout(sqlConnectionLayout);
            sqlServerLayout = new QHBoxLayout;
            sqlServerLayout->addWidget(sqlConnectionGroupBox);
            // third row
            clientIDLabel = new QLabel(QStringLiteral("Client ID"));
            clientIDLineEdit = new QLineEdit;
            clientSecretLabel = new QLabel(QStringLiteral("Client Secret"));
            clientSecretLineEdit = new QLineEdit;
            // layout
            clientCredentialsLayout = new QHBoxLayout;
            clientCredentialsLayout->addWidget(clientIDLabel);
            clientCredentialsLayout->addWidget(clientIDLineEdit);
            clientCredentialsLayout->addWidget(clientSecretLabel);
            clientCredentialsLayout->addWidget(clientSecretLineEdit);
            // group box layout
            authLayout->addLayout(configFileLayout);
            authLayout->addLayout(sqlServerLayout);
            authLayout->addLayout(clientCredentialsLayout);
            authGroupBox->setLayout(authLayout);

            // api section
            apiGroupBox = new QGroupBox(QStringLiteral("API server"), MainWindow);
            apiLayout = new QHBoxLayout;
            // first row
            apiAddressLabel = new QLabel(QStringLiteral("Adresa"));
            apiProtocolComboBox = new QComboBox;
            apiProtocolComboBox->addItems(protocols);
            apiHostNameLineEdit = new QLineEdit;
            const QRegularExpression condition(hostnameValidationRegex.join('|'));
            apiHostNameValidator = new QRegularExpressionValidator(condition, MainWindow);
            apiHostNameLineEdit->setValidator(apiHostNameValidator);
            apiPortLineEdit = new QLineEdit;
            QString portNumber;
            portNumber.setNum(defaultPort);
            apiPortLineEdit->setText(portNumber);
            apiTestConnectionButton = new QPushButton(QStringLiteral("Test spojení"));
            apiTestConnectionButton->setEnabled(false);
            apiTestResultIcon = new QLabel;
            const QPixmap icon = QPixmap(QStringLiteral(":/icons/icons/list-remove-blue.png"));
            apiTestResultIcon->setPixmap(icon.scaled(16,16));
            apiGetEndpointsButton = new QPushButton
                (QIcon(QStringLiteral(":/icons/icons/download.png")), QString());
            apiGetEndpointsButton->setEnabled(false);
            // layout
            apiLayout->addWidget(apiAddressLabel,0);
            apiLayout->addWidget(apiProtocolComboBox,2);
            apiLayout->addWidget(apiHostNameLineEdit,12);
            apiLayout->addWidget(apiPortLineEdit,2);
            apiLayout->addWidget(apiTestConnectionButton,4);
            apiLayout->addWidget(apiTestResultIcon,1);
            apiLayout->addWidget(apiGetEndpointsButton,1);
            apiGroupBox->setLayout(apiLayout);

            // token section
            tokenGroupBox = new QGroupBox(QStringLiteral("Autorizace"), MainWindow);
            tokenLayout = new QVBoxLayout;
            // first row
            tokenLabel = new QLabel(QStringLiteral("Token"));
            tokenTypeLineEdit = new QLineEdit;
            tokenTypeLineEdit->setAlignment(Qt::AlignCenter);
            tokenLineEdit = new QLineEdit;
            // layout
            tokenValueLayout = new QHBoxLayout;
            tokenValueLayout->addWidget(tokenLabel,0);
            tokenValueLayout->addWidget(tokenTypeLineEdit,1);
            tokenValueLayout->addWidget(tokenLineEdit,8);
            // secord row
            timeOfExpiryFromLabel = new QLabel(QStringLiteral("Platný od"));
            timeOfExpiryFromLineEdit = new QLineEdit(QStringLiteral("N/A"));
            timeOfExpiryFromLineEdit->setEnabled(false);
            timeOfExpiryFromLineEdit->setAlignment(Qt::AlignHCenter);
            timeOfExpiryToLabel = new QLabel(QStringLiteral("Platný do"));
            timeOfExpiryToLineEdit = new QLineEdit(QStringLiteral("N/A"));
            timeOfExpiryToLineEdit->setEnabled(false);
            timeOfExpiryToLineEdit->setAlignment(Qt::AlignHCenter);
            generateTokenButton = new QPushButton
                (QIcon(QStringLiteral(":/icons/icons/media-playlist-shuffle.png")),
                 QStringLiteral("Generovat"));
            generateTokenButton->setEnabled(false);
            viewTokenButton = new QPushButton
                (QIcon(QStringLiteral(":/icons/icons/video-display.png")),
                 QStringLiteral("Zobrazit"));
            viewTokenButton->setEnabled(false);
            // layout
            tokenParamsLayout = new QHBoxLayout;
            tokenParamsLayout->addWidget(timeOfExpiryFromLabel,0);
            tokenParamsLayout->addWidget(timeOfExpiryFromLineEdit,2);
            tokenParamsLayout->addWidget(timeOfExpiryToLabel,0);
            tokenParamsLayout->addWidget(timeOfExpiryToLineEdit,2);
            tokenParamsLayout->addWidget(generateTokenButton,1);
            tokenParamsLayout->addWidget(viewTokenButton,1);
            tokenParamsLayout->insertStretch(2,1);
            tokenParamsLayout->insertStretch(5,1);
            // group box layout
            tokenLayout->addLayout(tokenValueLayout);
            tokenLayout->addLayout(tokenParamsLayout);
            tokenGroupBox->setLayout(tokenLayout);

            // documentation section
            docGroupBox = new QGroupBox(QStringLiteral("Dokumentace"), MainWindow);
            docLayout = new QHBoxLayout;
            // first row
            swaggerLabel = new QLabel(QStringLiteral("Swagger (umístění)"));
            swaggerWebLocationLineEdit = new QLineEdit;
            swaggerFileLocationLineEdit = new QLineEdit;
            swaggerFileLocationLineEdit->setHidden(true);
            changeSwaggerLocationButton = new QPushButton
                (QIcon(QStringLiteral(":/icons/icons/emblem-symbolic-link.png")),
                 QStringLiteral("Zdroj"));
            swaggerFromWebButton = new QPushButton
                (QIcon(QStringLiteral(":/icons/icons/folder-remote.png")),
                 QStringLiteral("http://"));
            swaggerFromFileButton = new QPushButton
                (QIcon(QStringLiteral(":/icons/icons/folder-open.png")),
                 QStringLiteral("file://"));
            swaggerFromFileButton->setHidden(true);
            // layout
            docLayout->addWidget(swaggerLabel);
            docLayout->addWidget(swaggerWebLocationLineEdit);
            docLayout->addWidget(swaggerFileLocationLineEdit);
            docLayout->addWidget(changeSwaggerLocationButton);
            docLayout->addWidget(swaggerFromWebButton);
            docLayout->addWidget(swaggerFromFileButton);
            docGroupBox->setLayout(docLayout);

            // request section
            requestGroupBox = new QGroupBox(QStringLiteral("Request"), MainWindow);
            requestLayout = new QVBoxLayout;
            // first row
            requestLabel = new QLabel(QStringLiteral("Endpoint"));
            requestMethodComboBox = new QComboBox;
            QList<QString> methods = http::httpMethods.keys();
            methods.move(0,methods.size()-1);
            requestMethodComboBox->addItems(methods);
            requestSelectedEndpointLineEdit = new QLineEdit;
            requestSelectEndpointButton = new QPushButton
                (QIcon(QStringLiteral(":/icons/icons/document-open-remote.png")), QString());
            requestSelectEndpointButton->setEnabled(false);
            requestAcceptFormatComboBox = new QComboBox;
            const QStringList acceptFormats =
                { QStringLiteral("výchozí"), QStringLiteral("JSON"), QStringLiteral("XML") };
            requestAcceptFormatComboBox->addItems(acceptFormats);
            requestSendButton = new QPushButton
                (QIcon(QStringLiteral(":/icons/icons/go-up.png")), QStringLiteral("Odeslat"));
            requestSendButton->setEnabled(false);
            // layout
            requestEndpointLayout = new QHBoxLayout;
            requestEndpointLayout->addWidget(requestLabel);
            requestEndpointLayout->addWidget(requestMethodComboBox);
            requestEndpointLayout->addWidget(requestSelectedEndpointLineEdit);
            requestEndpointLayout->addWidget(requestSelectEndpointButton);
            requestEndpointLayout->addWidget(requestAcceptFormatComboBox);
            requestEndpointLayout->addWidget(requestSendButton);
            requestEndpointLayout->setStretchFactor(requestLabel,2);
            requestEndpointLayout->setStretchFactor(requestMethodComboBox,2);
            requestEndpointLayout->setStretchFactor(requestSelectedEndpointLineEdit,10);
            requestEndpointLayout->setStretchFactor(requestSelectEndpointButton,1);
            requestEndpointLayout->setStretchFactor(requestAcceptFormatComboBox,2);
            requestEndpointLayout->setStretchFactor(requestSendButton,3);
            // second and third row
            requestSelectAndFilterWidget = new QWidget;
            QSizePolicy currentPolicy = requestSelectAndFilterWidget->sizePolicy();
            currentPolicy.setRetainSizeWhenHidden(true);
            requestSelectAndFilterWidget->setSizePolicy(currentPolicy);
            selectLabel = new QLabel(QStringLiteral("Select"));
            selectConditionLineEdit = new QLineEdit;
            useOwnSelectConditionCheckBox = new QCheckBox;
            filterLabel = new QLabel(QStringLiteral("Filter"));
            filterConditionLineEdit = new QLineEdit;
            useOwnFilterConditionCheckBox = new QCheckBox;
            if (http::httpMethods[requestMethodComboBox->currentText()]._dtoObjectType != http::OUTPUT)
                requestSelectAndFilterWidget->setEnabled(false);
            // layout
            requestSelectAndFilterLayout = new QGridLayout(requestSelectAndFilterWidget);
            requestSelectAndFilterLayout->addWidget(selectLabel, 0, 0);
            requestSelectAndFilterLayout->addWidget(selectConditionLineEdit, 0, 1);
            requestSelectAndFilterLayout->addWidget(useOwnSelectConditionCheckBox, 0, 2);
            requestSelectAndFilterLayout->addWidget(filterLabel, 1, 0);
            requestSelectAndFilterLayout->addWidget(filterConditionLineEdit, 1, 1);
            requestSelectAndFilterLayout->addWidget(useOwnFilterConditionCheckBox, 1, 2);
            // group box layout
            requestLayout->addLayout(requestEndpointLayout);
            requestLayout->addWidget(requestSelectAndFilterWidget);
            requestGroupBox->setLayout(requestLayout);

            // buttons
            useProxyCheckBox = new QCheckBox;
            useProxyLabel = new QLabel(QStringLiteral("Použít proxy"));
            testModeCheckBox = new QCheckBox;
            testModeLabel = new QLabel(QStringLiteral("Testovací režim"));
            logButton = new QPushButton
                (QIcon(QStringLiteral(":/icons/icons/address-book-new.png")), QStringLiteral("Log"));
            quitButton = new QPushButton(QStringLiteral("Ukončit"));
            // layout
            buttonsLayout = new QHBoxLayout;
            buttonsLayout->addWidget(useProxyCheckBox);
            buttonsLayout->addWidget(useProxyLabel);
            buttonsLayout->addWidget(testModeCheckBox);
            buttonsLayout->addWidget(testModeLabel);
            buttonsLayout->addStretch();
            buttonsLayout->addWidget(logButton);
            buttonsLayout->addWidget(quitButton);

            windowLayout = new QVBoxLayout(MainWindow);
            windowLayout->addWidget(authGroupBox);
            windowLayout->addWidget(apiGroupBox);
            windowLayout->addWidget(tokenGroupBox);
            windowLayout->addWidget(docGroupBox);
            windowLayout->addWidget(requestGroupBox);
            windowLayout->addLayout(buttonsLayout);

            QMetaObject::connectSlotsByName(MainWindow);

            return;
        }
};

#endif // UI_MAINWINDOW_H

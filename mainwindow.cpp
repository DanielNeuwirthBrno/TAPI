/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <QApplication>
#include <QFileDevice>
#include <QFileDialog>
#include <QGridLayout>
#include <QInputDialog>
#include <QList>
#include <QMessageBox>
#include <QPair>
#include "endpointswindow.h"
#include "logwindow.h"
#include "mainwindow.h"
#include "methods.h"
#include "responsewindow.h"
#include "tokenwindow.h"
#include "ui/ui_mainwindow.h"

MainWindow::MainWindow(QWidget * parent): QDialog(parent), ui(new Ui_MainWindow),
    _delaySwaggerProcessing(false), _currentSession(new Session) {

    ui->setupUi(this);

    connect(ui->selectConfigFileButton, &QPushButton::clicked,
            this, &MainWindow::selectConfigFile);
    connect(ui->selectConfigFileLineEdit, &QLineEdit::editingFinished,
            this, &MainWindow::parseConfigFile);

    connect(ui->serverLineEdit, &QLineEdit::textChanged, this, [this]() -> void
            { this->_currentSession->connectionSettings()->setServer(ui->serverLineEdit->text()); } );
    connect(ui->serverLineEdit, &QLineEdit::textChanged,
            this, [this]() -> void { this->enableConnectButton(); } );

    connect(ui->userLineEdit, &QLineEdit::textChanged, this, [this]() -> void
            { this->_currentSession->connectionSettings()->setUser(ui->userLineEdit->text()); } );
    connect(ui->userLineEdit, &QLineEdit::textChanged,
            this, [this]() -> void { this->enableConnectButton(); } );

    connect(ui->passwordLineEdit, &QLineEdit::textEdited, this, [this]() -> void
            { this->_currentSession->connectionSettings()->setPassword(ui->passwordLineEdit->text()); } );
    connect(ui->passwordLineEdit, &QLineEdit::textEdited,
            this, [this]() -> void { this->enableConnectButton(); } );

    connect(ui->agendaDbLineEdit, &QLineEdit::textChanged, this, [this]() -> void
            { this->_currentSession->connectionSettings()->setAgenda(ui->agendaDbLineEdit->text()); } );
    connect(ui->agendaDbLineEdit, &QLineEdit::textChanged,
            this, [this]() -> void { this->enableConnectButton(); } );

    connect(ui->systemDbLineEdit, &QLineEdit::textChanged, this, [this]() -> void
            { this->_currentSession->connectionSettings()->setSystem(ui->systemDbLineEdit->text()); } );
    connect(ui->systemDbLineEdit, &QLineEdit::textChanged,
            this, [this]() -> void { this->enableConnectButton(); } );

    connect(ui->userS5LineEdit, &QLineEdit::textChanged, this, [this]() -> void
            { this->_currentSession->connectionSettings()->setUserS5(ui->userS5LineEdit->text()); } );
    connect(ui->userS5LineEdit, &QLineEdit::textChanged,
            this, [this]() -> void { this->enableConnectButton(); } );

    connect(ui->clientIDLineEdit, &QLineEdit::textChanged, this, [this]() -> void
            { this->_currentSession->credentials()->setClientID(ui->clientIDLineEdit->text()); } );
    connect(ui->clientIDLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableGenerateButton);
    connect(ui->clientSecretLineEdit, &QLineEdit::textChanged, this, [this]() -> void
            { this->_currentSession->credentials()->setClientSecret(ui->clientSecretLineEdit->text()); } );
    connect(ui->clientSecretLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableGenerateButton);

    connect(ui->apiProtocolComboBox, static_cast<void(QComboBox::*)(int)>
            (&QComboBox::currentIndexChanged), this, &MainWindow::setApiServerAddress);
    connect(ui->apiHostNameLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::setApiServerAddress);
    connect(ui->apiHostNameLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableGenerateButton);
    connect(ui->apiHostNameLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableGetEndpointsButton);
    connect(ui->apiHostNameLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableSendRequestButton);
    connect(ui->apiPortLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::setApiServerAddress);
    connect(ui->apiPortLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableGenerateButton);
    connect(ui->apiPortLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableGetEndpointsButton);
    connect(ui->apiPortLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableSendRequestButton);
    connect(ui->apiTestConnectionButton, &QPushButton::clicked,
            this, &MainWindow::testApiConnection);
    connect(ui->apiGetEndpointsButton, &QPushButton::clicked,
            this, &MainWindow::getListOfEndpoints);

    connect(ui->tokenTypeLineEdit, &QLineEdit::textChanged, this, [this]() -> void
            { this->_currentSession->token()->setType(ui->tokenTypeLineEdit->text()); } );
    connect(ui->tokenLineEdit, &QLineEdit::editingFinished,
            this, &MainWindow::setUserAssignedToken);
    connect(ui->tokenLineEdit, &QLineEdit::textChanged, this, [this]() -> void
            { ui->viewTokenButton->setEnabled(!ui->tokenLineEdit->text().isEmpty()); } );
    connect(ui->viewTokenButton, &QPushButton::clicked,
            this, &MainWindow::displayTokenWindow);

    connect(ui->changeSwaggerLocationButton, &QPushButton::clicked,
            this, &MainWindow::changeSwaggerLocation);
    connect(ui->swaggerFromWebButton, &QPushButton::clicked,
            this, &MainWindow::selectSwaggerWebSource);
    connect(ui->swaggerFromFileButton, &QPushButton::clicked,
            this, &MainWindow::selectSwaggerFile);
    connect(ui->swaggerWebLocationLineEdit, &QLineEdit::editingFinished, this, [this]() -> void
            { this->_currentSession->setWebSourceUrl(ui->swaggerWebLocationLineEdit->text()); } );
    connect(ui->swaggerWebLocationLineEdit, &QLineEdit::editingFinished, this, [this]() -> void
            { if (this->_currentSession->sourceChanged()) selectSwaggerWebSource(true); } );
    connect(ui->swaggerFileLocationLineEdit, &QLineEdit::editingFinished, this, [this]() -> void
            { this->_currentSession->setFileName(ui->swaggerFileLocationLineEdit->text()); } );
    connect(ui->swaggerFileLocationLineEdit, &QLineEdit::editingFinished, this, [this]() -> void
            { if (this->_currentSession->sourceChanged()) selectSwaggerFile(true); return; } );

    connect(ui->requestMethodComboBox, static_cast<void(QComboBox::*)(int)>
            (&QComboBox::currentIndexChanged), this, &MainWindow::hideSelectAndFilterWidget);
    connect(ui->requestSelectEndpointButton, &QPushButton::clicked,
            this, &MainWindow::displayEndpointsWindow);
    connect(ui->requestSelectEndpointButton, &QPushButton::clicked,
            this, &MainWindow::setCurrentEndpoint);

    connect(ui->requestSelectedEndpointLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::enableSendRequestButton);

    connect(this->_currentSession->_networkManager, &QNetworkAccessManager::finished,
            this, &MainWindow::processReceivedReply);
    connect(this, &MainWindow::processingOfGeneralRequestFinished,
            this, &MainWindow::displayResponseWindow);

    connect(ui->useProxyCheckBox, &QCheckBox::stateChanged, this, [this]() -> void
            { _currentSession->setAndApplyProxy(ui->useProxyCheckBox->isChecked()); } );
    connect(ui->testModeCheckBox, &QCheckBox::stateChanged, this, [this]() -> void
            { _currentSession->setTestMode(ui->testModeCheckBox->isChecked()); } );
    connect(ui->testModeCheckBox, &QCheckBox::stateChanged,
            this, &MainWindow::enableGenerateButton);
    connect(ui->testModeCheckBox, &QCheckBox::stateChanged,
            this, &MainWindow::enableGetEndpointsButton);
    connect(ui->logButton, &QPushButton::clicked,
            this, &MainWindow::displayLogWindow);

    connect(ui->generateTokenButton, &QPushButton::clicked, this, &MainWindow::generateToken);
    connect(ui->connectToServerButton, &QPushButton::clicked, this, &MainWindow::loadClientParams);
    connect(ui->requestSendButton, &QPushButton::clicked, this, &MainWindow::sendRequest);
    connect(ui->quitButton, &QPushButton::clicked, this, &QApplication::quit);
}

MainWindow::~MainWindow()
{
    delete _currentSession;
    delete ui;
}

void MainWindow::showFileErrorBox(const QString & fileName, const QString & infoText,
    const err::fileError & error, const QString & detail) const {

    QMessageBox * messageBox = new QMessageBox;
    const QIcon * icon = new QIcon(QStringLiteral(":/icons/icons/dialog-error.png"));
    messageBox->setWindowIcon(*icon);
    delete icon;

    messageBox->setWindowTitle(fileName);
    messageBox->setTextFormat(Qt::RichText);
    const QString text = QStringLiteral("<b>[") + QString::number(error) + QStringLiteral("] ") +
                         err::fileOpenErrors[error] + QStringLiteral("</b>");
    messageBox->setText(text);
    messageBox->setInformativeText(infoText);
    messageBox->setDetailedText(detail);
    messageBox->setIcon(QMessageBox::Warning);
    messageBox->setStandardButtons(QMessageBox::Ok);

    QGridLayout * layout = static_cast<QGridLayout *>(messageBox->layout());
    layout->setColumnMinimumWidth(2, 300);

    messageBox->exec();

    delete messageBox;
    return;
}

void MainWindow::showSwaggerErrorBox(const err::swaggerError error) const {

    QMessageBox * messageBox = new QMessageBox;
    const QIcon * icon = new QIcon(QStringLiteral(":/icons/icons/dialog-error.png"));
    messageBox->setWindowIcon(*icon);
    delete icon;

    messageBox->setWindowTitle(QStringLiteral("Swagger: chyba při zpracování"));
    messageBox->setTextFormat(Qt::RichText);
    const QString text =
        QStringLiteral("<b>Chybový stav: [") + QString::number(error) + QStringLiteral("]</b>");
    messageBox->setText(text);
    messageBox->setInformativeText(err::swaggerErrors[error]);
    messageBox->setIcon(QMessageBox::Warning);
    messageBox->setStandardButtons(QMessageBox::Ok);

    QGridLayout * layout = static_cast<QGridLayout *>(messageBox->layout());
    layout->setColumnMinimumWidth(2, 300);

    messageBox->exec();

    delete messageBox;
    return;
}

void MainWindow::cutText(QLineEdit * const element, const int maxLength) const {

    if (element->text().length() > maxLength)
        element->setText(element->text().left(maxLength) + QStringLiteral("..."));
}

// [slot]
void MainWindow::selectConfigFile() {

    const QString fileFilter = QStringLiteral("konfigurace API (*.") +
                               this->_currentSession->jsonFileType + QStringLiteral(")");

    const QString selectedFile =
        QFileDialog::getOpenFileName(this, QStringLiteral("Výběr souboru"),
                                     this->_currentSession->configFileLastDir(), fileFilter);
    if (!selectedFile.isNull())
        ui->selectConfigFileLineEdit->setText(selectedFile);

    parseConfigFile();

    return;
}

// [slot]
void MainWindow::parseConfigFile() const {

    this->_currentSession->setFileName(ui->selectConfigFileLineEdit->text());

    if (this->_currentSession->fileName().isEmpty())
        return;

    err::fileError error = this->_currentSession->openFile(_currentSession);
    if (static_cast<int>(error)) {

        const QString text = QStringLiteral("Chyba čtení souboru.");
        showFileErrorBox(this->_currentSession->fileName(), text, error);
        return;
    }

    const bool fileParsed = this->_currentSession->parseConfigFile();
    if (!fileParsed) {

        const QString text = QStringLiteral("Chyba při zpracování souboru.");
        error = err::NOT_PARSED;
        showFileErrorBox(this->_currentSession->fileName(), text, error);
        return;
    }

    ui->serverLineEdit->setText(_currentSession->connectionSettings()->serverName());
    ui->userLineEdit->setText(_currentSession->connectionSettings()->userName());
    ui->agendaDbLineEdit->setText(_currentSession->connectionSettings()->agendaDbName());
    ui->systemDbLineEdit->setText(_currentSession->connectionSettings()->systemDbName());
    ui->userS5LineEdit->setText(_currentSession->connectionSettings()->s5UserName());

    return;
}

// [slot]
void MainWindow::setApiServerAddress() {

    this->_currentSession->apiServer()->setValues(
        static_cast<ConnectionApi::Protocol>(ui->apiProtocolComboBox->currentIndex()),
        ui->apiHostNameLineEdit->text(), ui->apiPortLineEdit->text().toInt());

    ui->apiTestConnectionButton->setEnabled(this->_currentSession->apiServer()->port() != 0 &&
                                            ui->apiHostNameLineEdit->hasAcceptableInput());
    return;
}

// [slot]
void MainWindow::loadClientParams() const {

    if (this->_currentSession->allValuesSet()) {

        QSqlError error;
        const bool valuesLoaded =_currentSession->loadCredentials(error);

        if (valuesLoaded) {

            ui->clientIDLineEdit->setText(*(_currentSession->credentials()->clientID()));
            ui->clientSecretLineEdit->setText(*(_currentSession->credentials()->clientSecret()));
            ui->generateTokenButton->setFocus();
        }
        else {

            const QString title = QStringLiteral("Chyba spojení");
            const QString infoText = error.driverText();
            const QString detailText = error.databaseText();
            _currentSession->db()->showDbErrorBox(title, infoText, detailText, error.type());

            ui->clientIDLineEdit->clear();
            ui->clientSecretLineEdit->clear();
        }
    }

    return;
}

// [slot]
void MainWindow::enableGenerateButton() const {

    const bool buttonState =
        (!(this->_currentSession->credentials()->clientID()->isEmpty()) &&
         !(this->_currentSession->credentials()->clientSecret()->isEmpty()) &&
         (this->_currentSession->apiServer()->port() != 0) &&
         (ui->apiHostNameLineEdit->hasAcceptableInput())) || _currentSession->inTestMode();

    ui->generateTokenButton->setEnabled(buttonState);

    return;
}

// [slot]
void MainWindow::setUserAssignedToken() const {

    this->_currentSession->token()->setToken(ui->tokenLineEdit->text());
    this->cutText(ui->tokenLineEdit, 80);

    return;
}

// [slot]
void MainWindow::enableGetEndpointsButton() const {

    const bool buttonState =
        (ui->apiHostNameLineEdit->hasAcceptableInput() &&
         (this->_currentSession->apiServer()->port() != 0)) || _currentSession->inTestMode();

    ui->apiGetEndpointsButton->setEnabled(buttonState);

    return;
}

// [slot]
void MainWindow::selectSwaggerWebSource(const bool fromLineEdit) {

    _delaySwaggerProcessing = false;

    // if user clears SwaggerWebLocation => all processing is skipped
    const bool webSourceUrlIsEmpty = _currentSession->webSourceUrl().isEmpty() && !fromLineEdit;
    const QString defaultText =
        (webSourceUrlIsEmpty) ? swagger.endpoint : _currentSession->webSourceUrl();

    const QString selectedWebSource = (fromLineEdit && !webSourceUrlIsEmpty) ? defaultText :
        QInputDialog::getText(this, QStringLiteral("Výběr URL"),
                              QStringLiteral("Zdrojová adresa Swagger dokumentace:"),
                              QLineEdit::Normal, defaultText);

    // if web source has been identified
    if (!selectedWebSource.isEmpty()) {

        ui->changeSwaggerLocationButton->setEnabled(false);
        ui->swaggerWebLocationLineEdit->setText(selectedWebSource);

        const bool downloadEndpoints = _currentSession->downloadListOfEndpoints();

        // endpoints will be downloaded (either for the first time or because of user selection)
        if (downloadEndpoints) {

            _delaySwaggerProcessing = true;
            getEndpoints();

            // if in test mode, reply has not been sent => we must proceed with processing
            // (otherwise processSwaggerWebSource() would not be called because it is
            // called only after &QNetworkAccessManager::finished signal has been emitted)
            if (!(_currentSession->inTestMode()))
                return;
        }
        processSwaggerWebSource();
    }
    return;
}

// [private member function]
void MainWindow::processSwaggerWebSource() const {

    this->_currentSession->setWebSourceUrl(ui->swaggerWebLocationLineEdit->text());

    // download swagger docs' index page
    const bool requestPrepared = this->_currentSession->prepareSwaggerDocsRequest();

    if (requestPrepared) {

        if (_currentSession->inTestMode()) {

            // _currentID, in the meantime, has been incremented
            processSwaggerDocsReply(TEST, Communication::_currentID-1);
            ui->changeSwaggerLocationButton->setEnabled(true);
        }
        else
            this->_currentSession->sendGetRequestAndWaitForReply();
    }

    return;
}

// [slot]
void MainWindow::selectSwaggerFile(const bool fromLineEdit) {

    _delaySwaggerProcessing = false;

    const QString fileFilter = QStringLiteral("Swagger dokumentace (*.") +
                               this->_currentSession->jsonFileType + QStringLiteral(")");

    const QString selectedFile = (fromLineEdit) ? ui->swaggerFileLocationLineEdit->text() :
        QFileDialog::getOpenFileName(this, QStringLiteral("Výběr souboru"),
                                     this->_currentSession->swaggerFileLastDir(), fileFilter);

    // if file dialog was not cancelled by user
    if (!selectedFile.isEmpty()) {

        ui->changeSwaggerLocationButton->setEnabled(false);
        ui->swaggerFileLocationLineEdit->setText(selectedFile);

        const bool downloadEndpoints = _currentSession->downloadListOfEndpoints();

        // endpoints will be downloaded (either for the first time or because of user selection)
        if (downloadEndpoints) {

            _delaySwaggerProcessing = true;
            getEndpoints();

            // if in test mode, reply has not been sent => we must proceed with processing
            // (otherwise processSwaggerFile() would not be called because it is called
            // only after &QNetworkAccessManager::finished signal has been emitted)
            if (!(_currentSession->inTestMode()))
                return;
        }
        processSwaggerFile();
    }
    return;
}

// [private member function]
void MainWindow::processSwaggerFile() const {

    this->_currentSession->setFileName(ui->swaggerFileLocationLineEdit->text());

    err::fileError error = this->_currentSession->openFile(_currentSession);
    if (static_cast<int>(error)) {

        const QString text = QStringLiteral("Chyba čtení souboru.");
        showFileErrorBox(this->_currentSession->fileName(), text, error);
        return;
    }
    else {

        const bool fileParsed = this->_currentSession->parseSwaggerFile();
        if (!fileParsed) {

            const QString text = QStringLiteral("Chyba při zpracování souboru.");
            error = err::NOT_PARSED;
            const QString detailText =
                QStringLiteral("Nepodařilo se správně zpracovat soubor se Swagger dokumentací. "
                               "Z tohoto důvodu nebude pravděpodobně možné použít metody API "
                               "se vstupními parametry.");
            showFileErrorBox(this->_currentSession->fileName(), text, error, detailText);
        }
    }

    ui->changeSwaggerLocationButton->setEnabled(true);
    return;
}

// [slot]
void MainWindow::hideSelectAndFilterWidget() const {

    // select and filter clauses (their fields) are enabled only when output (get) methods are used
    ui->requestSelectAndFilterWidget->
        setEnabled(isOutputMethod(ui->requestMethodComboBox->currentText()));
    ui->selectConditionLineEdit->clear();

    return;
}

// [slot]
void MainWindow::setCurrentEndpoint() const {

    const Endpoint * const & currentEndpoint = Endpoint::currentEndpoint();

    if (currentEndpoint != nullptr) {

        const QString httpMethod = currentEndpoint->method().toUpper();
        const int newIndex = ui->requestMethodComboBox->findText(httpMethod);
        ui->requestMethodComboBox->setCurrentIndex(newIndex);

        // set path incl. parameters
        const QString endpointName = currentEndpoint->pathWithParameters();
        ui->requestSelectedEndpointLineEdit->setText(endpointName);

        // set select clause
        if (isOutputMethod(ui->requestMethodComboBox->currentText())) {

            const QString selectClause = currentEndpoint->buildSelectClause();
            ui->selectConditionLineEdit->setText(selectClause);
            ui->useOwnSelectConditionCheckBox->setChecked(false);
        }
    }
    return;
}

// [slot]
void MainWindow::enableSendRequestButton() const {

    bool stateOfSendRequestButton = false;

    if (!ui->requestSelectedEndpointLineEdit->text().isEmpty() &&
        ui->apiHostNameLineEdit->hasAcceptableInput() &&
        this->_currentSession->apiServer()->port() != 0)
        stateOfSendRequestButton = true;

    ui->requestSendButton->setEnabled(stateOfSendRequestButton);
    return;
}

/* section: connection test */

// [slot]
void MainWindow::testApiConnection() const {

    const QPixmap icon = QPixmap(QStringLiteral(":/icons/icons/download-later.png"));
    ui->apiTestResultIcon->setPixmap(icon.scaled(16,16));

    const bool requestPrepared = this->_currentSession->prepareTestConnectionRequest();

    if (requestPrepared)
        this->_currentSession->sendGetRequestAndWaitForReply();

    return;
}

void MainWindow::changeIconAccordingToTestConnectionResult(const StatusCode & status) const {

    QString iconPath;

    switch (status) {

        case OK: iconPath = QStringLiteral(":/icons/icons/dialog-ok-apply.png"); break;
        case NO_REPLY: iconPath = QStringLiteral(":/icons/icons/dialog-cancel.png"); break;
        default: iconPath = QStringLiteral(":/icons/icons/preferences-desktop-notification.png");
    }

    const QPixmap icon = QPixmap(iconPath);
    ui->apiTestResultIcon->setPixmap(icon.scaled(16,16));

    return;
}

/* section: get token */

// [slot]
void MainWindow::generateToken() const {

    ui->generateTokenButton->setEnabled(false);

    const bool requestPrepared = this->_currentSession->prepareGetTokenRequest();

    if (requestPrepared) {

        if (_currentSession->inTestMode())
            // _currentID, in the meantime, has been incremented
            processTokenReply(TEST, Communication::_currentID-1);
        else
            this->_currentSession->sendPostRequestAndWaitForReply();
    }
    return;
}

void MainWindow::processTokenReply(const StatusCode & status, uint16_t ID) const {

    if (status == OK || _currentSession->inTestMode()) {

        const bool valuesLoaded = this->_currentSession->parseTokenReply(ID);

        if (valuesLoaded) {

            ui->tokenTypeLineEdit->setText(_currentSession->token()->type());
            ui->tokenLineEdit->setText(_currentSession->token()->token());
            this->cutText(ui->tokenLineEdit, 72);

            // set time period (validity)
            const QString dateFormat = QStringLiteral("dd.MM.yyyy hh:mm:ss.zzz");
            const QString from = _currentSession->token()->from().toString(dateFormat);
            ui->resizeLineWidget(ui->timeOfExpiryFromLineEdit, from);
            const QString to = _currentSession->token()->to().toString(dateFormat);
            ui->resizeLineWidget(ui->timeOfExpiryToLineEdit, to);
        }
    }

    enableGenerateButton();
    return;
}

/* section: get endpoints */

// [slot]
void MainWindow::getListOfEndpoints() {

    _delaySwaggerProcessing = false;
    getEndpoints();
}

// [slot]
void MainWindow::changeSwaggerLocation() const {

    ui->swaggerWebLocationLineEdit->setVisible(ui->swaggerWebLocationLineEdit->isHidden());
    ui->swaggerFileLocationLineEdit->setVisible(ui->swaggerFileLocationLineEdit->isHidden());
    ui->swaggerFromWebButton->setVisible(ui->swaggerFromWebButton->isHidden());
    ui->swaggerFromFileButton->setVisible(ui->swaggerFromFileButton->isHidden());

    return;
}

void MainWindow::getEndpoints() const {

    const bool requestPrepared = this->_currentSession->prepareGetEndpointsRequest();

    if (requestPrepared) {

        if (_currentSession->inTestMode()) {

            // _currentID, in the meantime, has been incremented
            processEndpointsReply(TEST, Communication::_currentID-1);
        }
        else
            this->_currentSession->sendGetRequestAndWaitForReply();
    }
    return;
}

void MainWindow::processEndpointsReply(const StatusCode & status, uint16_t ID) const {

    if (status == OK || _currentSession->inTestMode()) {

        const bool valuesLoaded = this->_currentSession->parseEndpointsReply(ID);

        if (valuesLoaded)
            ui->requestSelectEndpointButton->setEnabled(valuesLoaded);
    }
}

/* section: swagger */

// [slot]
void MainWindow::processSwaggerDocsReply(const StatusCode & status, uint16_t ID) const {

    err::swaggerError error = err::SWAGGER_OK;

    if (status == OK || _currentSession->inTestMode()) {

        QStringList swaggerDocs;
        const bool valuesLoaded = this->_currentSession->parseSwaggerDocsReply(ID, swaggerDocs);

        // do not change conditions' sequence (top-to-bottom)
        if (swaggerDocs.isEmpty())
            error = err::FILE_NAMES_NOT_EXTRACTED;
        if (!valuesLoaded)
            error = err::SOURCE_NOT_PARSED;

        if (error == err::SWAGGER_OK) {

            // do not change conditions' sequence (left-to-right)
            if (_currentSession->inTestMode() ||
                !(_currentSession->downloadSwaggerFromWeb(swaggerDocs)))
                error = err::FILE_NOT_DOWNLOADED;

            if (error == err::SWAGGER_OK && !(_currentSession->parseSwaggerFile()))
                error = err::FILE_NOT_PARSED;
        }
    }
    else
        error = err::SOURCE_NOT_AVAILABLE;

    if (error != err::SWAGGER_OK)
        showSwaggerErrorBox(error);

    return;
}

/* section: general request */

// [slot]
void MainWindow::sendRequest() const {

    const QString selectedMethod = ui->requestMethodComboBox->currentText();
    const QString selectedPath =  ui->requestSelectedEndpointLineEdit->text();
    const ContentType accept =
        static_cast<ContentType>(ui->requestAcceptFormatComboBox->currentIndex()-1);

    const bool handMadeRequest = (Endpoint::currentEndpoint() == nullptr ||
        Endpoint::currentEndpoint()->pathWithParameters() != selectedPath) ? true : false;
    QString path = (handMadeRequest) ? selectedPath
                                     : Endpoint::currentEndpoint()->pathWithParameters();
    if (!path.startsWith('/')) path.insert(0, '/');

    if (selectedMethod == "GET") {

        const QPair<bool, QString> ownSelectClause =
            { ui->useOwnSelectConditionCheckBox->isChecked(), ui->selectConditionLineEdit->text()};

        const bool requestPrepared =
            this->_currentSession->prepareGeneralGetRequest(path, accept, http::GET, ownSelectClause);
        if (requestPrepared)
            this->_currentSession->sendGetRequestAndWaitForReply();
    }
    if (selectedMethod == "POST") {

        const bool requestPrepared =
            this->_currentSession->prepareGeneralPostRequest(path, accept);
        if (requestPrepared)
            this->_currentSession->sendPostRequestAndWaitForReply();
    }
    if (selectedMethod == "PUT") {

        const bool requestPrepared =
            this->_currentSession->prepareGeneralPutRequest(path, accept);
        if (requestPrepared)
            this->_currentSession->sendPutRequestAndWaitForReply();
    }
    if (selectedMethod == "DELETE") {

        const bool requestPrepared =
            this->_currentSession->prepareGeneralDeleteRequest(path, accept);
        if (requestPrepared)
            this->_currentSession->sendDeleteRequestAndWaitForReply();
    }
    return;
}

void MainWindow::processGeneralRequestReply(const StatusCode & status, uint16_t ID,
    const QNetworkAccessManager::Operation httpMethod) const {

    if (status == OK || _currentSession->inTestMode()) {

        const bool valuesLoaded = this->_currentSession->parseReplyToGeneralRequest(ID, httpMethod);

        if (valuesLoaded) {

        }
    }
    return;
}

// [slot]
void MainWindow::processReceivedReply(const QNetworkReply * const reply) const {

    const QNetworkRequest & originalRequest = reply->request();

    const RequestType requestType =
        static_cast<RequestType>(originalRequest.attribute(QNetworkRequest::User).toInt());
    const uint16_t ID = originalRequest.attribute(Request::userAttribute(1)).toInt();
    const QNetworkAccessManager::Operation httpMethod = reply->operation();

    switch (requestType) {

        case API: {

            changeIconAccordingToTestConnectionResult(_currentSession->getStatus(reply));
            break;
        }
        case TOKEN: {
            processTokenReply(_currentSession->getStatus(reply), ID);
            break;
        }
        case ENDPOINTS: {
            processEndpointsReply(_currentSession->getStatus(reply), ID);
            if (_delaySwaggerProcessing) {

                if (ui->swaggerFileLocationLineEdit->isHidden()) processSwaggerWebSource();
                if (ui->swaggerWebLocationLineEdit->isHidden()) processSwaggerFile();
            }
            break;
        }
        case SWAGGER: {
            processSwaggerDocsReply(_currentSession->getStatus(reply), ID);
            ui->changeSwaggerLocationButton->setEnabled(true);
            break;
        }
        case OTHER: {
            processGeneralRequestReply(_currentSession->getStatus(reply), ID, httpMethod);
            emit processingOfGeneralRequestFinished(reply);
        }
    }
    return;
}

/* section: windows */

// [slot]
int MainWindow::displayTokenWindow() {

    TokenWindow tokenWindow(this->_currentSession->token(), this);
    return tokenWindow.exec();
}

// [slot]
int MainWindow::displayEndpointsWindow() {

    EndpointsWindow endpointsWindow(this->_currentSession->endpoints(), this);
    return endpointsWindow.exec();
}

// [slot]
int MainWindow::displayResponseWindow(const QNetworkReply * const reply) {

    ResponseWindow responseWindow(reply, this->_currentSession, this);
    return responseWindow.exec();
}

// [slot]
int MainWindow::displayLogWindow() {

    LogWindow logWindow(this->_currentSession->communication(), this);
    return logWindow.exec();
}

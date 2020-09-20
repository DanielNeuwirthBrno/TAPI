/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <QApplication>
#include <QEventLoop>
#include <QFile>
#include <QInputDialog>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QNetworkProxy>
#include <QRegularExpression>
#include <QUrl>
#include <QUuid>
#include <algorithm>
#include "session.h"
#include "tables.h"
#include "types.h"

const QString Session::jsonFileType = QStringLiteral("json"); // config, Swagger
const QRegularExpression Session::swaggerUrlsRegex =
    QRegularExpression(QStringLiteral("\"urls\":\\[\\{.+\\}\\]"));

Session::Session():

    _networkManager(new QNetworkAccessManager), _endpoints(QVector<Endpoint>()),
    _accessToken(new Token), _connectionSettings(new ConnectionS5), _apiServer(new ConnectionApi),
    _db(new Database), _credentials(new Credentials), _sourceChanged(false), _fileName(QString()),
    _fileContents(QByteArray()), _configFileLastDir(QString()), _swaggerFileLastDir(QString()),
    _useProxy(false), _testModeEnabled(false) {

    setupProxy(_useProxy);

    QObject::connect(_networkManager, &QNetworkAccessManager::finished,
                     this, &Session::replyFinished);

    QMetaObject::connectSlotsByName(this);
}

Session::~Session() {

    delete _credentials;
    delete _db;
    delete _apiServer;
    delete _connectionSettings;
    delete _accessToken;
    delete _networkManager;
}

QWidget * Session::getMainWindowHandle() const {

    QWidget * parent = nullptr;

    for (auto it: QApplication::topLevelWidgets())
        if (it->objectName() == "MainWindow")
            { parent = it; break; }

    return parent;
}

void Session::setupProxy(const bool useProxy) {

    QNetworkProxy proxyServer;
    proxyServer.setHostName("127.0.0.1");
    proxyServer.setPort(8888);

    // use system proxy to enable Fiddler capture traffic
    if (useProxy)
        proxyServer.setType(QNetworkProxy::HttpProxy);
    else
        proxyServer.setType(QNetworkProxy::NoProxy);

    _networkManager->setProxy(proxyServer);

    return;
}

void Session::setValues(const QString & s5UserName, const QString & agendaDbName,
                        const QString & docDbName, const QString & systemDbName,
                        const QString & userName, const QString & serverName) {

    _connectionSettings->setUserS5(s5UserName);
    _connectionSettings->setAgenda(agendaDbName);
    _connectionSettings->setAgendaDoc(docDbName);
    _connectionSettings->setSystem(systemDbName);
    _connectionSettings->setUser(userName);
    _connectionSettings->setServer(serverName);

    return;
}

void Session::newMessage(const Request & request) {

    const Communication message(request);
    _communication.push_back(message);

    return;
}

QNetworkRequest Session::currentRequest() const {

    return _communication.last().request().request();
}

QByteArray Session::currentRequestBody() const {

    return _communication.last().request().body();
}

Response Session::lastReply(const uint16_t ID) {

    Communication * comm = this->findCorrespondingRequest(ID);

    return (comm->response());
}

QByteArray Session::lastReplyContents(const uint16_t ID) {

    Communication * comm = this->findCorrespondingRequest(ID);

    return (comm->response().response());
}

bool Session::setReplyToCurrentRequest(QNetworkReply * const reply) {

    const QByteArray replyContents = reply->readAll();
    const QList<QNetworkReply::RawHeaderPair> headers = reply->rawHeaderPairs();
    const StatusCode statusCode =
            static_cast<StatusCode>(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    const QString status = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    const QVariant ID = reply->request().attribute(Request::userAttribute(1));

    const Response newResponse(replyContents, headers, ID, statusCode, status);

    // assign response to corresponding request
    // (responses may not be received in the same order in which requests were sent)
    Communication * comm = this->findCorrespondingRequest(ID.toInt());
    if (comm == nullptr)
        return false;

    comm->setReply(newResponse);
    return true;
}

err::fileError Session::openFile(const QString & selectedFile) {

    QFile file(selectedFile);
    if (!file.exists() || !file.open(QIODevice::ReadOnly))
        return err::NOT_OPEN;

    this->_fileContents = file.readAll();
    if (this->_fileContents.isEmpty())
        return err::NOT_READ;

    file.close();
    return err::NO_ERROR;
}

Communication * Session::findCorrespondingRequest(const uint16_t ID) {

    QVector<Communication>::reverse_iterator it;

    for (it =_communication.rbegin(); it != _communication.rend(); ++it)
        if (it->ID() == ID)

            // base() returns iterator to the element that is next
            // to the element the reverse_iterator is currently pointing to
            return it.base()-1;

    return nullptr;
}

QString Session::getTableName(const QVariant & ID) {

    Communication * const comm = this->findCorrespondingRequest(ID.toInt());
    const QString url = comm->request().request().url().toString();

    QString endpointName = QString();
    QString tableName = QString();

    // parse url to get endpoint name
    const QString endpointNameRegex = QStringLiteral("[A-Z][a-z]+");
    const QStringList endpointNameTokens = url.split('/');
    for (auto it: endpointNameTokens)
        if (it.contains(QRegularExpression(endpointNameRegex)))
            { endpointName = it; break; }

    // map endpoint name to table name
    tableName = dbTables[endpointName].first;

    return tableName;
}

Session::State Session::verifyTableRecords(const QString & tableName, const QList<QString> & recordIDs,
                                 const QNetworkAccessManager::Operation & method) {

    // use agenda/document db
    bool useDocDb = false;
    for (auto it: dbTables.values())
        if (it.first == tableName)
            useDocDb = it.second;

    QString agendaDbName;
    if (!useDocDb)
        agendaDbName = this->connectionSettings()->agendaDbName();
    else {
        if (!this->connectionSettings()->agendaDbDocName().isEmpty())
            agendaDbName = this->connectionSettings()->agendaDbDocName();
        else
            agendaDbName = this->connectionSettings()->agendaDbName() + QStringLiteral("_Doc");
    }

    // connect to agenda DB
    QSqlError error;
    const bool connectionEstablished =
        this->connectToServer(agendaDbName, this->db()->agendaDbConnection(), error);

    bool dataAcquired = false;
    QString liveRecords = QString();
    QString deletedRecords = QString();

    if (connectionEstablished) {

        QString IDs;
        for (auto it: recordIDs)
            IDs += QStringLiteral("'") + it + QStringLiteral("',");
        IDs.chop(1);

        const QString queryStringCore =
            QStringLiteral("SELECT COUNT(Deleted) FROM ") + agendaDbName +
            QStringLiteral("..") + tableName + QStringLiteral(" WHERE ID IN (") + IDs +
            QStringLiteral(") AND Deleted = ");
        const QString queryString =
            QStringLiteral("SELECT (") + queryStringCore +
            QStringLiteral("0), (") + queryStringCore + QStringLiteral("1)");

        QList<QString *> attributes ({ &liveRecords, &deletedRecords });

        dataAcquired = this->db()->processSimpleQuery(queryString, this->db()->agendaDbConnection(),
                                                      error, attributes);

    }
    this->db()->agendaDbConnection()->close();

    // connection error
    if (error.type() != QSqlError::NoError) {

        const QString title = QStringLiteral("Chyba spojení");
        const QString infoText = error.driverText();
        const QString detailText = error.databaseText();
        this->db()->showDbErrorBox(title, infoText, detailText, error.type());

        return NOT_VERIFIED;
    }

    if (dataAcquired) {

        switch (method) {

            case QNetworkAccessManager::PostOperation:
            case QNetworkAccessManager::PutOperation:
                // number of live records must be equal to number of IDs
                if (liveRecords.toInt() == recordIDs.size())
                    return VERIFIED;
                break;

            case QNetworkAccessManager::DeleteOperation:
                // number of live records must be zero
                if (liveRecords.toInt() == 0 &&
                    // number of deleted records must be either zero (physical delete)
                    // or equal to number of IDs (logical delete)
                    (deletedRecords.toInt() == 0 || deletedRecords.toInt() == recordIDs.size()))
                    return VERIFIED;
                break;

            default: return NOT_VERIFIED;
        }
    }

    return NOT_VERIFIED_ERROR;
}

bool Session::parseConfigFile() {

    const QJsonDocument configFileJson = QJsonDocument::fromJson(this->_fileContents);
    if (configFileJson.isNull())
        return false;

    // whole document
    QJsonObject configFileObjectJson(configFileJson.object());
    // Configuration section
    configFileObjectJson = configFileObjectJson["Configuration"].toObject();
    // S5Db section
    configFileObjectJson = configFileObjectJson["S5Db"].toObject();

    if (configFileObjectJson.isEmpty())
        return false;

    this->setValues(configFileObjectJson["S5UserName"].toString(),
                    configFileObjectJson["AgendaDbName"].toString(),
                    configFileObjectJson["DocumentDbName"].toString(),
                    configFileObjectJson["SystemDbName"].toString(),
                    configFileObjectJson["SqlUserName"].toString(),
                    configFileObjectJson["SqlServer"].toString());

    return true;
}

bool Session::parseSwaggerFile() {

    const QJsonDocument swaggerFileJson = QJsonDocument::fromJson(this->_fileContents);
    if (swaggerFileJson.isNull())
        return false;

    // whole document
    QJsonObject swaggerObjectJson(swaggerFileJson.object());
    // paths section
    const QJsonObject paths = swaggerObjectJson["paths"].toObject();
    const QJsonObject definitions = swaggerObjectJson["definitions"].toObject();
    if (paths.isEmpty() || definitions.isEmpty())
        return false;

    for (QVector<Endpoint>::iterator it = _endpoints.begin(); it < _endpoints.end(); ++it) {

        const QString objectName = "/" + it->path();
        const QString httpMethod = it->method().toLower();
        const QJsonObject path = paths[objectName].toObject();
        const QJsonObject method = path[httpMethod].toObject();
        const QString summary = method["summary"].toString();

        if (it->propertiesAreNotSet()) {

            it->setSummary(summary);
            it->setParams(method);

            const http::dataFlow dtoObjectType =
                http::httpMethods[httpMethod.toUpper()]._dtoObjectType;
            if (dtoObjectType == http::INPUT)
                it->setDtoInputDefinitionLink(method);
            if (dtoObjectType == http::OUTPUT)
                it->setDtoOutputDefinitionLink(method);

            it->setDtoAttributes(definitions, dtoObjectType);
        }
    }
    return true;
}

bool Session::allValuesSet() const {

    if (_connectionSettings->serverName().isEmpty() ||
        _connectionSettings->userName().isEmpty() ||
        _connectionSettings->password().isEmpty() ||
        _connectionSettings->agendaDbName().isEmpty() ||
        _connectionSettings->systemDbName().isEmpty() ||
        _connectionSettings->s5UserName().isEmpty())
        return false;

    return true;
}

bool Session::loadCredentials(QSqlError & error) {

    bool dataAcquired = false;
    QString userID = QUuid().toString();

    // connect to system DB
    bool connectionEstablished =
        this->connectToServer(this->connectionSettings()->systemDbName(),
                              this->db()->systemDbConnection(), error);

    if (connectionEstablished) {

        const QString queryString =
           QStringLiteral("SELECT TOP 1 ID FROM ") + this->connectionSettings()->systemDbName() +
           QStringLiteral("..System_Users ") +
           QStringLiteral("WHERE userName = '") + this->connectionSettings()->s5UserName() +
           QStringLiteral("' AND Deleted = 0");

        // get User_ID
        QList<QString *> attributes({ &userID });
        dataAcquired = this->db()->processSimpleQuery(queryString, this->db()->systemDbConnection(),
                                                      error, attributes);
    }
    this->db()->systemDbConnection()->close();

    // connect to agenda DB
    connectionEstablished =
        this->connectToServer(this->connectionSettings()->agendaDbName(),
        this->db()->agendaDbConnection(), error);

    if (connectionEstablished) {

        const QString queryString =
            QStringLiteral("SELECT TOP 1 ClientID, ClientSecret, TypOvereni ") +
            QStringLiteral("FROM ") + this->connectionSettings()->agendaDbName() +
            QStringLiteral("..CSWSystem_UsersAPIKeys ") +
            QStringLiteral("WHERE User_ID = '") + userID + QStringLiteral("' AND Deleted = 0 ") +
            QStringLiteral("ORDER BY Create_Date DESC");

        // get ClientID, ClientSecret, TypOvereni
        QString * const grantType = new QString();
        QList<QString *> attributes
            ({ _credentials->clientID(), _credentials->clientSecret(), grantType });
        dataAcquired = this->db()->processSimpleQuery(queryString, this->db()->agendaDbConnection(),
                                                      error, attributes);
        _credentials->setGrantType(static_cast<Credentials::GrantType>((*grantType).toInt()));
        delete grantType;
    }
    this->db()->agendaDbConnection()->close();

    return (connectionEstablished && dataAcquired);
}

bool Session::connectToServer(const QString & dbName, QSqlDatabase * db, QSqlError & error) const {

    const QString connectionDataSet =
        QStringLiteral("DRIVER={SQL Server};Server=") + this->connectionSettings()->serverName() +
        QStringLiteral(";Database=") + dbName + QStringLiteral(";Uid=") +
        this->connectionSettings()->userName() + QStringLiteral(";Port=1433;Pwd=") +
        this->connectionSettings()->password() + QStringLiteral(";");

    if (db->isOpen())
        db->close();

    db->setDatabaseName(connectionDataSet);

    const bool connectionEstablished = db->open();
    if (!connectionEstablished)
        error = db->lastError();

    return connectionEstablished;
}

bool Session::setAuthorizationHeader(QNetworkRequest * const request) {

    const QString type = this->token()->type();
    const QString token = this->token()->token();

    if (type.isEmpty() || token.isEmpty() || QDateTime::currentDateTime() > this->token()->to())
        return false; // invalid token

    const QString authorizationString = type + " " + token;
    request->setRawHeader(QByteArray("Authorization"), authorizationString.toLocal8Bit());

    return true;
}

bool Session::prepareRequest(const http::httpMethodType httpMethod, const QString & endpoint,
                             const ContentType & contentType, const ContentType & accept,
                             const RequestType & requestType, bool authenticationRequired,
                             const QByteArray & body, const QUrlQuery & query) {

    if (_testModeEnabled || this->token()->isNotComplete())
        authenticationRequired = false;

    const QString protocol = protocols.at(static_cast<int>(apiServer()->protocol()));
    const uint16_t port = apiServer()->port();

    QUrl urlAddress;
    urlAddress.setScheme(protocol);
    urlAddress.setPort(port);
    if (!query.isEmpty())
        urlAddress.setQuery(query);

    const QString hostNameInclPath = apiServer()->hostName();
    QString host = hostNameInclPath;
    QString path = endpoint;

    if (hostNameInclPath.contains('/')) {

        host = hostNameInclPath.left(hostNameInclPath.indexOf('/'));
        path = hostNameInclPath.mid(hostNameInclPath.indexOf('/')) + path;
    }

    urlAddress.setHost(host, QUrl::StrictMode);
    urlAddress.setPath(path, QUrl::StrictMode);

    const QVariant typeOfRequest = static_cast<QVariant>(requestType);
    const QVariant ID = static_cast<QVariant>(Communication::_currentID);

    QNetworkRequest * newNetworkRequest = new QNetworkRequest;
    newNetworkRequest->setUrl(urlAddress);
    newNetworkRequest->setAttribute(QNetworkRequest::User, typeOfRequest);
    newNetworkRequest->setAttribute(Request::userAttribute(1), ID);
    newNetworkRequest->setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("TAPI"));

    bool isTokenValid = true;
    if (authenticationRequired)
        isTokenValid = setAuthorizationHeader(newNetworkRequest);
    if (!isTokenValid)
        return false;

    if (contentType != NOT_USED)
        newNetworkRequest->setHeader(QNetworkRequest::ContentTypeHeader, contentTypes[contentType]);
    if (accept != NOT_USED)
        newNetworkRequest->setRawHeader(QByteArray("Accept"),
                                        QByteArray(contentTypes[accept].toLocal8Bit()));
    if (!body.isEmpty())
        newNetworkRequest->setHeader(QNetworkRequest::ContentLengthHeader, body.size());

    const Request newRequest(*newNetworkRequest, httpMethod, body);
    this->newMessage(newRequest);

    delete newNetworkRequest;

    return true;
}

bool Session::prepareTestConnectionRequest() {

    const http::httpMethodType httpMethod = http::GET;
    const QString path = QString();
    const ContentType contentType = NOT_USED;
    const ContentType accept = HTML;
    const RequestType typeOfRequest = API;

    const bool requestPrepared =
        prepareRequest(httpMethod, path, contentType, accept, typeOfRequest);
    return requestPrepared;
}

bool Session::prepareGetTokenRequest() {

    const http::httpMethodType httpMethod = tokenEndpoint.httpMethod;
    const QString path = tokenEndpoint.endpoint;
    const ContentType contentType = tokenEndpoint.contentType;
    const ContentType accept = tokenEndpoint.accept;
    const RequestType typeOfRequest = TOKEN;

    const QString bodyContents =
        QStringLiteral("client_id=") + *(this->credentials()->clientID()) +
        QStringLiteral("&client_secret=") + *(this->credentials()->clientSecret()) +
        QStringLiteral("&grant_type=") + grantTypes[this->credentials()->grantType()] +
        QStringLiteral("&scope=") + this->credentials()->scope();
    const QByteArray body(bodyContents.toUtf8());

    const bool requestPrepared =
        prepareRequest(httpMethod, path, contentType, accept, typeOfRequest, false, body);
    return requestPrepared;
}

bool Session::parseTokenReply(uint16_t ID) {

    Communication * comm = this->findCorrespondingRequest(ID);
    if (comm == nullptr)
        return false;

    // test mode
    if (_testModeEnabled) {

        const QString fromResource = QStringLiteral(":/json/json/token.json");
        QFile fromRes(fromResource);
        fromRes.open(QIODevice::ReadOnly | QIODevice::Text);

        const QByteArray replyContent = fromRes.readAll();
        // QFileDevice::FileError error = fromRes.error();

        comm->setLastReplyContent(replyContent);
        comm->setLastReplyTestStatus();
        fromRes.close();
    }

    const QJsonDocument tokenReplyJson = QJsonDocument::fromJson(this->lastReplyContents(ID));
    if (tokenReplyJson.isNull())
        return false;

    QJsonObject tokenObjectJson(tokenReplyJson.object());

    if (tokenObjectJson.isEmpty())
        return false;

    this->setTokenData(tokenObjectJson["access_token"].toString(),
                       tokenObjectJson["token_type"].toString(),
                       tokenObjectJson["expires_in"].toInt());
    return true;
}

void Session::setTokenData(const QString & token, const QString & type, const int seconds) {

    const QDateTime from = QDateTime::currentDateTime();
    const QDateTime to = from.addSecs(seconds);

    this->token()->setTokenInclDate(token, type, from, to);

    return;
}

bool Session::prepareGetEndpointsRequest() {

    const http::httpMethodType httpMethod = endpointsEndpoint.httpMethod;
    const QString path = endpointsEndpoint.endpoint;
    const ContentType contentType = endpointsEndpoint.contentType;
    const ContentType accept = endpointsEndpoint.accept;
    const RequestType typeOfRequest = ENDPOINTS;

    const bool requestPrepared =
        prepareRequest(httpMethod, path, contentType, accept, typeOfRequest);
    return requestPrepared;
}

bool Session::parseEndpointsReply(uint16_t ID) {

    Communication * comm = this->findCorrespondingRequest(ID);
    if (comm == nullptr)
        return false;

    // test mode
    if (_testModeEnabled) {

        const QString fromResource = QStringLiteral(":/json/json/endpoints.json");
        QFile fromRes(fromResource);
        fromRes.open(QIODevice::ReadOnly | QIODevice::Text);

        const QByteArray replyContent = fromRes.readAll();
        // QFileDevice::FileError error = fromRes.error();

        comm->setLastReplyContent(replyContent);
        comm->setLastReplyTestStatus();
        fromRes.close();
    }

    const QJsonDocument endpointsReplyJson = QJsonDocument::fromJson(this->lastReplyContents(ID));
    if (endpointsReplyJson.isNull())
        return false;

    _endpoints.clear();

    // whole document
    QJsonObject endpointsObjectJson(endpointsReplyJson.object());
    // Data section
    const QJsonArray endpointsArray = endpointsObjectJson["Data"].toArray();
    // iterate over data (versions)
    for (int d = 0; d < endpointsArray.size(); ++d) {

        const QJsonObject endpointsObjectJson(endpointsArray[d].toObject());
        // Modules section
        const QJsonArray modulesArray = endpointsObjectJson["Modules"].toArray();

        // iterate over modules
        for (int m = 0; m < modulesArray.size(); ++m) {

            const QJsonObject modulesObject(modulesArray[m].toObject());

            if (modulesObject["Name"].toString() == "Core") {

                // Entities section
                const QJsonArray entitiesArray = modulesObject["Entities"].toArray();

                // iterate over entities
                for (int e = 0; e < entitiesArray.size(); ++e) {

                    const QJsonObject entitiesObject(entitiesArray[e].toObject());
                    // Methods section
                    const QJsonArray methods = entitiesObject["Methods"].toArray();

                    if (methods.isEmpty())
                        return false;

                    // iterate over methods
                    for (int t = 0; t < methods.size(); ++t) {

                        const QJsonObject method(methods[t].toObject());
                        const QString name = method["Name"].toString();
                        const QString httpMethod = method["HttpMethod"].toString();

                        const Endpoint newEndpoint(name, httpMethod);
                        this->_endpoints.push_back(newEndpoint);
                    }
                }
            }
        }
    }

    std::sort(_endpoints.begin(), _endpoints.end());
    return true;
}

bool Session::downloadListOfEndpoints() {

    int downloadEndpoints = 0x00004000; // yes

    if (!this->endpoints()->isEmpty()) {

        // get pointer to MainWindow
        QWidget * const parent = getMainWindowHandle();

        downloadEndpoints =
            QMessageBox::question(parent, QStringLiteral("Stažení seznamu endpointů"),
                                  QStringLiteral("Endpointy již byly staženy. Stáhnout znovu?"));
    }

    if (downloadEndpoints == 0x00004000)
        return true;

    return false;
}

bool Session::prepareSwaggerDocsRequest() {

    const http::httpMethodType httpMethod = swagger.httpMethod;
    const QString path = this->webSourceUrl();
    const ContentType contentType = swagger.contentType;
    const ContentType accept = swagger.accept;
    const RequestType typeOfRequest = SWAGGER;

    const bool requestPrepared =
        prepareRequest(httpMethod, path, contentType, accept, typeOfRequest);
    return requestPrepared;
}

bool Session::parseSwaggerDocsReply(uint16_t ID, QStringList & swaggerDocsSources) {

    Communication * comm = this->findCorrespondingRequest(ID);
    if (comm == nullptr)
        return false;

    // test mode
    if (_testModeEnabled) {

        const QString fromResource = QStringLiteral(":/html/html/swagger.html");
        QFile fromRes(fromResource);
        fromRes.open(QIODevice::ReadOnly | QIODevice::Text);

        const QByteArray replyContent = fromRes.readAll();
        // QFileDevice::FileError error = fromRes.error();

        comm->setLastReplyContent(replyContent);
        comm->setLastReplyTestStatus();
        fromRes.close();
    }

    // extract part of returned html file containing swagger docs' urls
    const QString swaggerUrls(this->lastReplyContents(ID));
    const QRegularExpressionMatch match = this->swaggerUrlsRegex.match(swaggerUrls);
    QString capturedText = match.captured();
    if (capturedText.isNull())
        return false;
    else
        capturedText = QStringLiteral("{") + capturedText + QStringLiteral("}");

    const QJsonDocument swaggerDocsJson = QJsonDocument::fromJson(capturedText.toUtf8());
    if (swaggerDocsJson.isNull())
        return false;

    QJsonObject swaggerDocsJsonObject = swaggerDocsJson.object();
    const QJsonArray swaggerDocsJsonArray = swaggerDocsJsonObject["urls"].toArray();
    for (auto it: swaggerDocsJsonArray) {

        swaggerDocsJsonObject = it.toObject();
        swaggerDocsSources.push_back(swaggerDocsJsonObject["url"].toString());
    }

    return true;
}

QString Session::selectSource(const QStringList & sourceList) {

    // get pointer to MainWindow
    QWidget * const parent = getMainWindowHandle();

     const QString source = QInputDialog::getItem(parent, QStringLiteral("Výběr Swaggeru"),
         QStringLiteral("Vyberte verzi Swagger dokumentace:"), sourceList, 0, false);

     return source;
}

bool Session::downloadSwaggerFromWeb(const QStringList & sourceList) {

    const QString source =
        (sourceList.size() > 1) ? this->selectSource(sourceList) : sourceList.at(0);
    if (source.isNull())
        return false;

    QUrl urlAddress;
    urlAddress.setScheme(protocols.at(static_cast<int>(this->apiServer()->protocol())));
    urlAddress.setPort(this->apiServer()->port());
    urlAddress.setHost(this->apiServer()->hostName(), QUrl::StrictMode);
    urlAddress.setPath(source, QUrl::StrictMode);

    QNetworkAccessManager * localManager = new QNetworkAccessManager;
    QNetworkReply * reply = localManager->get(QNetworkRequest(urlAddress));

    QEventLoop waitForReply;
    connect(reply, &QNetworkReply::finished, &waitForReply, &QEventLoop::quit);
    waitForReply.exec();

    this->_fileContents = reply->readAll();
    delete localManager;

    if (_fileContents.isEmpty())
        return false;

    return true;
}

void Session::prepareGetRequestQuery(QUrlQuery & query, const QString & path,
                                     const QPair<bool, const QString> & useOwnSelectCondition) {
    // select clause
    const QString selectClause = (useOwnSelectCondition.first == true)
        ? useOwnSelectCondition.second
        : (Endpoint::currentEndpoint() != nullptr)
              ? Endpoint::currentEndpoint()->buildSelectClause()
              : QString();

    if (!selectClause.isEmpty()) {

        if (path.contains("/v2.0"))
            query.addQueryItem(queryStringClause[SELECT], selectClause);
        else // v1.0 is default (and less succinct)
            query.addQueryItem(queryStringClause[SELECT] + queryStringClause[SELECT_PROPS], selectClause);
    }

    // filter clause (TO DO)

    return;
}

bool Session::prepareGeneralGetRequest(const QString & path, const ContentType & acceptType,
    const http::httpMethodType httpMethod, const QPair<bool, const QString> & ownSelectClause) {

    const ContentType contentType = JSON;
    const ContentType accept = acceptType;
    const RequestType typeOfRequest = OTHER;

    // prepare query (if any)
    QUrlQuery requestQuery = QUrlQuery();
    prepareGetRequestQuery(requestQuery, path, ownSelectClause);

    const bool requestPrepared = prepareRequest(httpMethod, path, contentType, accept,
                                                typeOfRequest, true, QByteArray(), requestQuery);
    return requestPrepared;
}

bool Session::preparePostRequestBody(QByteArray & body) {

    if (Endpoint::currentEndpoint() == nullptr)
        return false;

    QString bodyContents;
    const QString quotes = QStringLiteral("\"");

    for (auto it: *(Endpoint::currentEndpoint()->attributes())) {

        if (!it.value().toString().isEmpty()) {

            QString value;
            switch (types::matchDataTypes[it.type()]) {

                case types::STRING: value = quotes + it.value().toString() + quotes;
                                    break;
                case types::UUID: value = it.value().toUuid().toString();
                                  value.remove(QRegularExpression("[{|}]"));
                                  value = quotes + value + quotes;
                                  break;
                case types::DATE: value = quotes + it.value().toDateTime().toString(Qt::ISODate) + quotes;
                                  break;
                case types::BOOL: value = types::convertBoolToText(it.value());
                                  break;
                case types::INT: value = value.setNum(it.value().toInt());
                                 break;
                case types::FLOAT: value = value.setNum(it.value().toDouble());
                                   break;
                default: continue; // jump to next attribute
            };

            bodyContents += quotes + it.name() + quotes + ": " + value + ",";
        }
    }
    if (bodyContents.isEmpty())
        return false;

    bodyContents.chop(1);
    bodyContents = QStringLiteral("[ {") + bodyContents + QStringLiteral("} ]");
    body = bodyContents.toUtf8();

    return true;
}

bool Session::prepareGeneralPostRequest(const QString & path, const ContentType & acceptType,
                                        const http::httpMethodType httpMethod) {

    const ContentType contentType = JSON;
    const ContentType accept = acceptType;
    const RequestType typeOfRequest = OTHER;

    // prepare body
    QByteArray requestBody;
    bool requestPrepared = preparePostRequestBody(requestBody);

    if (requestPrepared)
        requestPrepared = prepareRequest(httpMethod, path, contentType, accept,
                                         typeOfRequest, true, requestBody);
    return requestPrepared;
}

bool Session::prepareGeneralPutRequest(const QString & path, const ContentType & acceptType) {

    return prepareGeneralPostRequest(path, acceptType, http::PUT);
}

bool Session::prepareGeneralDeleteRequest(const QString & path, const ContentType & acceptType) {

    return prepareGeneralGetRequest(path, acceptType, http::DELETE);
}

QString Session::testResource(const QNetworkAccessManager::Operation httpMethod,
                              const bool expanded) const {

    typedef QNetworkAccessManager::Operation op;

    switch (httpMethod) {

        case op::GetOperation:
            if (expanded)
                return QStringLiteral(":/json/json/getall.json");
            else
                return QStringLiteral(":/json/json/get.json");
        case op::PutOperation: return QStringLiteral(":/json/json/put.json");
        case op::PostOperation:
            if (expanded)
                return QStringLiteral(":/json/json/postall.json");
            else
                return QStringLiteral(":/json/json/post.json");
        case op::DeleteOperation: return QStringLiteral(":/json/json/delete.json");
        default: return QString();
    }

    return QString();
}

bool Session::parseReplyToGeneralRequest(
     uint16_t ID, const QNetworkAccessManager::Operation httpMethod) {

    Communication * comm = this->findCorrespondingRequest(ID);
    if (comm == nullptr)
        return false;

    // test mode
    if (_testModeEnabled) {

        const QString fromResource = testResource(httpMethod);

        QFile fromRes(fromResource);
        fromRes.open(QIODevice::ReadOnly | QIODevice::Text);

        const QByteArray replyContent = fromRes.readAll();
        // QFileDevice::FileError error = fromRes.error();

        comm->setLastReplyContent(replyContent);
        comm->setLastReplyTestStatus();
        fromRes.close();
    }

    const QJsonDocument replyContentsJson = QJsonDocument::fromJson(this->lastReplyContents(ID));
    if (replyContentsJson.isNull())
        return false;

    // state attributes
    Response currentResponse = this->lastReply(ID);
    StateAttributes stateAttribs;

    stateAttribs.pageCount = currentResponse.parseBody("PageCount");
    stateAttribs.rowCount = currentResponse.parseBody("RowCount");
    stateAttribs.status = currentResponse.parseBody("Status");
    stateAttribs.message = currentResponse.parseBody("Message");
    stateAttribs.stackTrace = currentResponse.parseBody("StackTrace");

    comm->setLastReplyStateAttribs(stateAttribs);

    return true;
}

void Session::sendGetRequestAndWaitForReply() const {

    this->_networkManager->get(this->currentRequest());
    return;
}

void Session::sendPostRequestAndWaitForReply() const {

    this->_networkManager->post(this->currentRequest(), this->currentRequestBody());
    return;
}

void Session::sendPutRequestAndWaitForReply() const {

    this->_networkManager->put(this->currentRequest(), this->currentRequestBody());
    return;
}

void Session::sendDeleteRequestAndWaitForReply() const {

    this->_networkManager->deleteResource(this->currentRequest());
    return;
}

// [slot]
void Session::replyFinished(QNetworkReply * const reply) {

    bool replySuccessfullySet = this->setReplyToCurrentRequest(reply);

    reply->close();
    reply->deleteLater();

    return;
}


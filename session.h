/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef SESSION_H
#define SESSION_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QPair>
#include <QUrlQuery>
#include <QWidget>
#include "connection.h"
#include "credentials.h"
#include "database.h"
#include "endpoint.h"
#include "error.h"
#include "methods.h"
#include "request.h"

class Session: public QObject {

    Q_OBJECT

    public:
        enum State { UNKNOWN = -1, VERIFIED = 0, NOT_VERIFIED = 1, NOT_VERIFIED_ERROR = 2 };

        Session();
        ~Session();

        const static QString jsonFileType;
        const static QRegularExpression swaggerUrlsRegex;

        inline QVector<Endpoint> * endpoints() { return &(_endpoints); }
        inline Token * token() const { return _accessToken; }
        inline ConnectionS5 * connectionSettings() const { return _connectionSettings; }
        inline ConnectionApi * apiServer() const { return _apiServer; }
        inline Database * db() const { return _db; }
        inline Credentials * credentials() const { return _credentials; }
        inline QString fileName() const { return _fileName; }
        inline bool sourceChanged() const { return _sourceChanged; }
        inline QString webSourceUrl() const { return _webSourceUrl; }
        inline QString configFileLastDir() const { return _configFileLastDir; }
        inline QString swaggerFileLastDir() const { return _swaggerFileLastDir; }
        inline QVector<Communication> & communication() { return _communication; }
        inline bool inTestMode() const { return _testModeEnabled; }

        inline void setFileName(const QString & fileName) {
           _sourceChanged = (_fileName != fileName);
           if (_sourceChanged) { _fileName = fileName; } return; }
        inline void setWebSourceUrl(const QString & url) {
           _sourceChanged = (_webSourceUrl != url);
           if (_sourceChanged) { _webSourceUrl = url; } return; }
        void setValues(const QString &, const QString &, const QString &,
                       const QString &, const QString &, const QString &);
        inline void setAndApplyProxy(const bool useProxy)
            { _useProxy = useProxy; setupProxy(_useProxy); return; }
        inline void setTestMode(const bool inTestMode) { _testModeEnabled = inTestMode; return; }

        void newMessage(const Request &);
        QNetworkRequest currentRequest() const;
        QByteArray currentRequestBody() const;

        inline static StatusCode getStatus(const QNetworkReply * const reply)
            { return (static_cast<StatusCode>(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt())); }
        inline Response lastReply(const uint16_t);
        inline QByteArray lastReplyContents(const uint16_t);

        inline err::fileError openFile(const Session * const session)
            { return (openFile(session->_fileName)); }
        err::fileError openFile(const QString &);

        Communication * findCorrespondingRequest(const uint16_t);
        QString getTableName(const QVariant &);
        State verifyTableRecords(const QString &, const QList<QString> &,
                                 const QNetworkAccessManager::Operation &);

        bool allValuesSet() const;
        bool loadCredentials(QSqlError &);
        bool connectToServer(const QString &, QSqlDatabase *, QSqlError &) const;

        bool parseConfigFile();
        bool parseSwaggerFile();

        bool prepareRequest(const http::httpMethodType, const QString &, const ContentType &,
                            const ContentType &, const RequestType &, bool = false,
                            const QByteArray & = QByteArray(), const QUrlQuery & = QUrlQuery());
        bool prepareTestConnectionRequest();

        bool prepareGetTokenRequest();
        bool parseTokenReply(uint16_t);
        void setTokenData(const QString &, const QString &, const int);

        bool prepareGetEndpointsRequest();
        bool parseEndpointsReply(uint16_t);
        bool downloadListOfEndpoints();

        bool prepareSwaggerDocsRequest();
        bool parseSwaggerDocsReply(uint16_t, QStringList &);
        bool downloadSwaggerFromWeb(const QStringList &);

        void prepareGetRequestQuery(QUrlQuery &, const QString &,
                                    const QPair<bool, const QString> & = { false, QString() });
        bool prepareGeneralGetRequest(const QString &, const ContentType &,
                                      const http::httpMethodType = http::GET,
                                      const QPair<bool, const QString> & = { false, QString() });
        bool preparePostRequestBody(QByteArray &);
        bool prepareGeneralPostRequest(const QString &, const ContentType &,
                                       const http::httpMethodType = http::POST);
        bool prepareGeneralPutRequest(const QString &, const ContentType &);
        bool prepareGeneralDeleteRequest(const QString &, const ContentType &);
        bool parseReplyToGeneralRequest(uint16_t, const QNetworkAccessManager::Operation);

        void sendGetRequestAndWaitForReply() const;
        void sendPostRequestAndWaitForReply() const;
        void sendPutRequestAndWaitForReply() const;
        void sendDeleteRequestAndWaitForReply() const;

        QNetworkAccessManager * _networkManager;

    private:
        QWidget * getMainWindowHandle() const;
        QString testResource(const QNetworkAccessManager::Operation, const bool = true) const;
        bool setAuthorizationHeader(QNetworkRequest * const);
        bool setReplyToCurrentRequest(QNetworkReply * const);
        QString selectSource(const QStringList &);
        void setupProxy(const bool);

        QVector<Endpoint> _endpoints;
        Token * _accessToken;
        ConnectionS5 * _connectionSettings;
        ConnectionApi * _apiServer;
        Database * _db;
        Credentials * _credentials;
        bool _sourceChanged;
        QString _fileName;
        QString _webSourceUrl;
        QByteArray _fileContents;
        QString _configFileLastDir;
        QString _swaggerFileLastDir;
        QVector<Communication> _communication;
        bool _useProxy;
        bool _testModeEnabled;

    private slots:
        void replyFinished(QNetworkReply *);
};

#endif // SESSION_H

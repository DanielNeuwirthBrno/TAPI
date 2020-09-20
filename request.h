/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef REQUEST_H
#define REQUEST_H

#include <QByteArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUuid>
#include "methods.h"

enum RequestType { API = 1, TOKEN = 2, ENDPOINTS = 3, SWAGGER = 4, OTHER = 5 };

enum ContentType { NOT_USED = -1, JSON = 0, XML = 1, URL_ENCODED = 2, HTML = 3 };

enum StatusCode { TEST = -1, NO_REPLY = 0, OK = 200, BAD_REQUEST = 400, NOT_AUTH = 401,
                  FORBIDDEN = 403, NOT_FOUND = 404, INT_SERVER_ERROR = 500 };

enum QueryStringClause { NONE = -1, SELECT = 0, SELECT_PROPS = 1, FILTER = 10 };

const static QMap<ContentType, QString> contentTypes = {

    { JSON, QStringLiteral("application/json") },
    { XML, QStringLiteral("application/xml") },
    { URL_ENCODED, QStringLiteral("application/x-www-form-urlencoded") },
    { HTML, QStringLiteral("text/html") }
};

const static QMap<QueryStringClause, QString> queryStringClause = {

    { SELECT, QStringLiteral("select") },
    { SELECT_PROPS, QStringLiteral(".Properties") },
    { FILTER, QStringLiteral("filter") }
};

const static struct AuxiliaryEndpoint {

    http::httpMethodType httpMethod;
    ContentType contentType;
    ContentType accept;
    QString endpoint;
    bool authorizationRequired;

} tokenEndpoint = { http::POST, URL_ENCODED, NOT_USED, QStringLiteral("/connect/token"), false },
  endpointsEndpoint = { http::GET, JSON, NOT_USED, QStringLiteral("/Admin/Roles/Endpoints"), false },
  swagger =  { http::GET, HTML, NOT_USED, QStringLiteral("/swaggerDoc/index.html"), false };

class Request {

    public:
        inline static QNetworkRequest::Attribute userAttribute(uint16_t number)
           { return (static_cast<QNetworkRequest::Attribute>(
                 (static_cast<uint16_t>(QNetworkRequest::User)) + number)); }

        Request(): _acceptFormat(JSON), _request(new QNetworkRequest) {}
        Request(const QNetworkRequest &, const http::httpMethodType, const QByteArray &);
        Request(const Request &);
        ~Request() { delete _request; }

        inline http::httpMethodType httpMethod() const { return _httpMethod; }
        inline QByteArray body() const { return _body; }
        inline QNetworkRequest request() const { return *(_request); }

    private:
        http::httpMethodType _httpMethod;
        ContentType _contentType;
        ContentType _acceptFormat;
        QByteArray _body;
        QNetworkRequest * _request;
};

struct StateAttributes {

    QString pageCount;
    QString rowCount;
    QString status;
    QString message;
    QString stackTrace;
};

class Response {

    public:
        Response() {}
        Response(const QByteArray &, const QList<QNetworkReply::RawHeaderPair> &,
                 const QVariant &, const StatusCode & = OK, const QString & = "OK");
        ~Response() {}

        inline StatusCode statusCode() const { return _statusCode; }
        inline QString statusDescription() const { return _status; }
        inline StateAttributes stateAttributes() const { return _stateAttributes; }
        inline const QList<QNetworkReply::RawHeaderPair> & headers() { return _headers; }
        inline QByteArray response() const { return _response; }

        QString parseBody(const QString &) const;
        QString dataFromBody(QList<QString> &) const;

        inline void setResponse(const QByteArray & response)
            { _response = response; return; }
        inline void setTestStatus()
            { _statusCode = TEST; _status = QStringLiteral("Test mode"); return; }
        inline void setStateAttribs(const StateAttributes & attribs)
            { _stateAttributes = attribs; return; }

    private:
        inline bool containsValidID(const QString & idFromResponse) const
            { const QUuid id(idFromResponse); return !(id.isNull()); }

        StatusCode _statusCode;
        QString _status;
        StateAttributes _stateAttributes;
        QList<QNetworkReply::RawHeaderPair> _headers;
        QVariant _ID;
        QByteArray _response;
};

class Communication {

    public:
        Communication() {}
        Communication(const Request & request):
             _ID(request.request().attribute(Request::userAttribute(1)).toInt()),
             _createDate(QDateTime::currentDateTime()), _request(request) { ++(_currentID); }
        ~Communication() {}

        static uint16_t _currentID;
        inline uint16_t ID() const { return _ID; }
        inline const QDateTime & createDate() const { return _createDate; }
        inline Request request() const { return _request; }
        inline Response response() const { return _response; }
        inline void setLastReplyContent(const QByteArray & replyContent)
            { this->_response.setResponse(replyContent); return; }
        inline void setLastReplyTestStatus() { this->_response.setTestStatus(); return; }
        inline void setLastReplyStateAttribs(const StateAttributes & stateAttribs)
            { this->_response.setStateAttribs(stateAttribs); return; }
        inline void setReply(const Response & reply) { _response = reply; return; }

    private:
        uint16_t _ID;
        QDateTime _createDate;
        Request _request;
        Response _response;
};

#endif // REQUEST_H

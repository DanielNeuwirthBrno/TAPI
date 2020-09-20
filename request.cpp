/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <cstdint>
#include "request.h"

uint16_t Communication::_currentID = 0;

Request::Request(const QNetworkRequest & request, const http::httpMethodType httpMethod,
                 const QByteArray & body): _httpMethod(httpMethod), _acceptFormat(JSON),
                 _body(body), _request(new QNetworkRequest) {

    *(_request) = request;
}

Request::Request(const Request & r2) {

    _httpMethod = r2._httpMethod;
    _contentType = r2._contentType;
    _acceptFormat = r2._acceptFormat;
    _body = r2._body;

    _request = new(QNetworkRequest);
    *(_request) = r2.request();
}

Response::Response(const QByteArray & contents, const QList<QNetworkReply::RawHeaderPair> & headers,
                   const QVariant & ID, const StatusCode & code, const QString & status):
    _statusCode(code), _status(status), _headers(headers), _ID(ID), _response(contents) {

    this->_stateAttributes.pageCount = QString();
    this->_stateAttributes.rowCount = QString();
    this->_stateAttributes.status = QString();
    this->_stateAttributes.message = QString();
    this->_stateAttributes.stackTrace = QString();
}

QString Response::parseBody(const QString & tagName) const {

    const QJsonDocument currentJsonDocument = QJsonDocument::fromJson(_response);
    if (currentJsonDocument.isNull())
        return QString();

    const QJsonObject currentJsonObject(currentJsonDocument.object());
    if (currentJsonObject.isEmpty())
        return QString();

    const QJsonValue currentJsonValue = currentJsonObject[tagName];
    if (currentJsonValue.isNull())
        return QString();

    switch (currentJsonValue.type()) {

        case QJsonValue::Bool:
            return (currentJsonValue.toBool()) ? QStringLiteral("true") : QStringLiteral("false");
        case QJsonValue::Double:
            if (currentJsonValue.toInt(INT_MIN) != INT_MIN)
                return QString::number(currentJsonValue.toInt());
            else
                return QString::number(currentJsonValue.toDouble());
        case QJsonValue::String: return currentJsonValue.toString();
        default: return QString();
    }

    return QString();
}

QString Response::dataFromBody(QList<QString> & recordIDs) const {

    const QJsonDocument currentJsonDocument = QJsonDocument::fromJson(_response);
    if (currentJsonDocument.isNull())
        return QString();

    const QJsonObject currentJsonObject(currentJsonDocument.object());
    if (currentJsonObject.isEmpty())
        return QString();

    const QJsonValue currentJsonValue = currentJsonObject["Data"];
    if (currentJsonValue.isNull())
        return QString();

    // data section contains json array
    if (currentJsonValue.type() == QJsonValue::Array) {

        const QJsonArray data = currentJsonValue.toArray();
        QJsonDocument dataDocument;
        dataDocument.setArray(data);

        for (auto it: data) {

          // json array contains json values as elements (POST/PUT)
          if (it.type() == QJsonValue::String && this->containsValidID(it.toString()))
              recordIDs.push_back(it.toString());

          // json array contains json object (GET)
          if (it.type() == QJsonValue::Object)
            ; // no action required
        }

        return dataDocument.toJson(QJsonDocument::Indented);
    }

    // data section contains json object (no method)
    if (currentJsonValue.type() == QJsonValue::Object) {

        const QJsonObject data = currentJsonValue.toObject();
        QJsonDocument dataDocument;
        dataDocument.setObject(data);

        return dataDocument.toJson(QJsonDocument::Indented);
    }

    // data section contains value (DELETE)
    if (currentJsonValue.type() == QJsonValue::String) {

        const QString contents = currentJsonValue.toString();
        if (this->containsValidID(contents))
            recordIDs.push_back(contents);

        return contents;
    }

    return QString();
}

/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#include <QDateTime>
#include <QMap>
#include <QString>

const static QMap<uint8_t, QString> grantTypes = {

    { 0, QStringLiteral("client_credentials") },
    { 1, QStringLiteral("authorization_code") },
};

class Token {

    public:
        Token() {}
        ~Token() {}

        inline QString token() const { return _token; }
        inline QString type() const { return _type; }
        inline QDateTime from() const { return _validFrom; }
        inline QDateTime to() const { return _validTo; }
        inline bool isNotComplete() const { return _token.isEmpty() || _type.isEmpty(); }

        inline void setToken(const QString & token)
          { _token = token; return; }
        inline void setType(const QString & type)
          { _type = type; return; }
        inline void setTokenInclDate(const QString & token, const QString & type,
                                     const QDateTime & from, const QDateTime & to)
            { _token = token; _type = type; _validFrom = from; _validTo = to; return; }

    private:
        QString _token;
        QString _type;
        QDateTime _validFrom;
        QDateTime _validTo;
};

class Credentials {

    public:
        enum GrantType { UNKNOWN = -1, CLIENT_CREDENTIALS = 0, AUTHORIZATION_CODE = 1 };

        Credentials(): _grantType(CLIENT_CREDENTIALS), _scope(QStringLiteral("S5Api")),
                       _clientID(new QString), _clientSecret(new QString) {}
        ~Credentials() { delete _clientID; delete _clientSecret; }

        inline QString * clientID() const { return _clientID; }
        inline QString * clientSecret() const { return _clientSecret; }
        inline GrantType grantType() const { return _grantType; }
        inline QString scope() const { return _scope; }

        inline void setClientID(const QString & id) { *(_clientID) = id; }
        inline void setClientSecret(const QString & secret) { *(_clientSecret) = secret; }
        inline void setGrantType(const GrantType & type) { _grantType = type; }

    private:
        GrantType _grantType;
        const QString _scope;
        QString * _clientID;
        QString * _clientSecret;
};

#endif // CREDENTIALS_H

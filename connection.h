/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QString>
#include <QStringList>

const static QStringList protocols {"http", "https"};
const static uint16_t defaultPort = 80;

const static QStringList hostnameValidationRegex {

    QStringLiteral("^localhost$"),
    QStringLiteral("^([a-zA-Z-]+.)?\\w+\\.[a-zA-Z-]{2}(\\/\\w+)?$"),
    QStringLiteral("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\\.|$)){4}$") // IP address
};

class ConnectionApi {

    public:
        enum Protocol { UNKNOWN = -1, HTTP = 0, HTTPS = 1 };

        ConnectionApi(): _protocol(HTTP), _port(defaultPort) {}
        ~ConnectionApi() {}

        inline Protocol protocol() const { return _protocol; }
        inline QString hostName() const { return _hostName; }
        inline uint16_t port() const { return _port; }

        inline void setValues(const Protocol & protocol, const QString & hostName, const uint16_t port)
          { _protocol = protocol; _hostName = hostName; _port = port; return; }

    private:
        Protocol _protocol;
        QString _hostName;
        uint16_t _port;
};

class ConnectionS5 {

    public:
        ConnectionS5() {}
        ~ConnectionS5() {}

        inline QString serverName() const { return _serverName; }
        inline QString userName() const { return _userName; }
        inline QString password() const { return _password; }
        inline QString agendaDbName() const { return _agendaDbName; }
        inline QString agendaDbDocName() const { return _agendaDocDbName; }
        inline QString systemDbName() const { return _systemDbName; }
        inline QString s5UserName() const { return _s5UserName; }

        inline void setServer(const QString & server) { _serverName = server; }
        inline void setUser(const QString & user) { _userName = user; }
        inline void setPassword(const QString & password) { _password = password; }
        inline void setAgenda(const QString & agenda) { _agendaDbName = agenda; }
        inline void setAgendaDoc(const QString & doc) { _agendaDocDbName = doc; }
        inline void setSystem(const QString & system) { _systemDbName = system; }
        inline void setUserS5(const QString & user) { _s5UserName = user; }

    private:
        QString _serverName;
        QString _userName;
        QString _password;
        QString _agendaDbName;
        QString _agendaDocDbName;
        QString _systemDbName;
        QString _s5UserName;
};

#endif // CONNECTION_H

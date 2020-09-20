/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef DATABASE_H
#define DATABASE_H

#include <QList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

namespace sql {

    struct ConnectionToSqlServer {

        const static QString _driverName;
        const static QString _connectionToAgendaName;
        const static QString _connectionToSystemName;
    };
}

class Database {

    public:
        Database();
        ~Database();

        inline sql::ConnectionToSqlServer sqlConnectionSettings() const
            { return _sqlConnectionSettings; }
        inline QSqlDatabase * agendaDbConnection() const
            { return _agendaDbConnection; }
        inline QSqlDatabase * systemDbConnection() const
            { return _systemDbConnection; }

        void showDbErrorBox(const QString &, const QString &, const QString &,
                            const QSqlError::ErrorType & = QSqlError::NoError) const;

        bool processSimpleQuery(const QString &, QSqlDatabase * const,
                                QSqlError &, QList<QString *> &);

    private:
        sql::ConnectionToSqlServer _sqlConnectionSettings;
        QSqlDatabase * _agendaDbConnection;
        QSqlDatabase * _systemDbConnection;
};

#endif // DATABASE_H

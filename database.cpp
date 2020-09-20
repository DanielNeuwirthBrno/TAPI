/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <QGridLayout>
#include <QIcon>
#include <QMessageBox>
#include <QSqlQuery>
#include <QVariant>
#include "database.h"
#include "error.h"

const QString sql::ConnectionToSqlServer::_driverName = QStringLiteral("QODBC3");
const QString sql::ConnectionToSqlServer::_connectionToAgendaName =
      QStringLiteral("connectionToAgenda");
const QString sql::ConnectionToSqlServer::_connectionToSystemName =
      QStringLiteral("connectionToSystem");

Database::Database(): _agendaDbConnection(new QSqlDatabase), _systemDbConnection(new QSqlDatabase) {

    *(_agendaDbConnection) =
        QSqlDatabase::addDatabase(this->sqlConnectionSettings()._driverName,
                                  this->sqlConnectionSettings()._connectionToAgendaName);
    *(_systemDbConnection) =
        QSqlDatabase::addDatabase(this->sqlConnectionSettings()._driverName,
                                  this->sqlConnectionSettings()._connectionToSystemName);
}

Database::~Database() {

    delete _agendaDbConnection;
    delete _systemDbConnection;
    QSqlDatabase::removeDatabase(this->sqlConnectionSettings()._connectionToAgendaName);
    QSqlDatabase::removeDatabase(this->sqlConnectionSettings()._connectionToSystemName);
}

void Database::showDbErrorBox(const QString & title, const QString & infoText,
    const QString & detailText, const QSqlError::ErrorType & type) const {

    QMessageBox * messageBox = new QMessageBox;
    const QIcon * icon = new QIcon(QStringLiteral(":/icons/icons/dialog-error.png"));
    messageBox->setWindowIcon(*icon);
    delete icon;

    messageBox->setWindowTitle(title);
    messageBox->setTextFormat(Qt::RichText);
    const QString text = QStringLiteral("<b>[") + QString::number(type) + QStringLiteral("] ") +
                         err::connectionErrors[type] + QStringLiteral("</b>");
    messageBox->setText(text);
    messageBox->setInformativeText(infoText);
    messageBox->setDetailedText(detailText);
    messageBox->setIcon(QMessageBox::Warning);
    messageBox->setStandardButtons(QMessageBox::Ok);

    QGridLayout * layout = static_cast<QGridLayout *>(messageBox->layout());
    layout->setColumnMinimumWidth(2, 300);

    messageBox->exec();

    delete messageBox;
    return;
}

bool Database::processSimpleQuery(const QString & queryString, QSqlDatabase * const db,
                                  QSqlError & error, QList<QString *> & attributes) {

    bool recordRetrieved = false;

    QSqlQuery * query = new QSqlQuery(queryString, *db);

    if (!query->lastError().isValid()) {

        if (query->size() == 0)
           return recordRetrieved;

        recordRetrieved = query->first();
        if (recordRetrieved) {

            for (int i = 0; i < attributes.size(); ++i)
                *(attributes.at(i)) = query->value(i).toString();
        }
    }

    error = query->lastError();
    delete query;

    return recordRetrieved;
}

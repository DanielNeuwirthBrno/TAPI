/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef ERROR_H
#define ERROR_H

#include <QMap>
#include <QSqlError>
#include <QString>

namespace err {

    const QMap<QSqlError::ErrorType, QString> connectionErrors = {

        { static_cast<QSqlError::ErrorType>(-1), QStringLiteral("UndeterminedError") },
        { QSqlError::NoError, QString("EmptyResult") },
        { QSqlError::ConnectionError, QStringLiteral("ConnectionError") },
        { QSqlError::StatementError, QStringLiteral("StatementError") },
        { QSqlError::TransactionError, QStringLiteral("TransactionError") },
        { QSqlError::UnknownError, QStringLiteral("UnknownError") }
    };

    enum fileError { NO_ERROR = 0, NOT_OPEN = 1, NOT_READ = 2, NOT_PARSED = 3 };

    const QMap<fileError, QString> fileOpenErrors = {

        { fileError::NO_ERROR, QStringLiteral("EmptyResult") },
        { fileError::NOT_OPEN, QStringLiteral("OpenError") },
        { fileError::NOT_READ, QStringLiteral("ReadError") },
        { fileError::NOT_PARSED, QStringLiteral("ParseError") }
    };

    enum swaggerError { SWAGGER_OK = 0, SOURCE_NOT_AVAILABLE, SOURCE_NOT_PARSED,
                        FILE_NAMES_NOT_EXTRACTED, FILE_NOT_DOWNLOADED, FILE_NOT_PARSED };

    const QMap<swaggerError, QString> swaggerErrors = {

        { swaggerError::SWAGGER_OK, QStringLiteral("OK.") },
        { swaggerError::SOURCE_NOT_AVAILABLE, QStringLiteral("Požadovaná zdrojová stránka není momentálně dostupná.") },
        { swaggerError::SOURCE_NOT_PARSED, QStringLiteral("Obsah zdrojové stránky se nepodařilo správně dekódovat.") },
        { swaggerError::FILE_NAMES_NOT_EXTRACTED, QStringLiteral("Nepodařilo se získat názvy zdrojových souborů.") },
        { swaggerError::FILE_NOT_DOWNLOADED, QStringLiteral("Nepodařilo se stáhnout zdrojový soubor s dokumentací.") },
        { swaggerError::FILE_NOT_PARSED, QStringLiteral("Soubor s dokumentací se nepodařilo správně dekódovat.") }
    };
}

#endif // ERROR_H

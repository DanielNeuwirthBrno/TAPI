/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef TYPES_H
#define TYPES_H

#include <QDateTime>
#include <QMap>
#include <QMetaType>
#include <QString>
#include <QUuid>
#include <QVariant>

namespace types {

    enum dataTypes { UNDETERMINED = 0, STRING, UUID, DATE, BOOL, INT, FLOAT };

    const static QMap<QString, dataTypes> matchDataTypes = {
        { "string", STRING }, { "string/uuid", UUID }, { "string/date-time", DATE },
        { "boolean", BOOL }, { "integer/int32", INT }, { "number/double", FLOAT }
    };

    inline QString convertBoolToText(const QVariant boolValue)
        { return (boolValue.toBool() == true) ? QStringLiteral("true") : QStringLiteral("false"); }

    inline bool isGivenValueForActualTypeValid(const dataTypes type, QVariant & value) {

        bool valid = false;
        switch(type) {

            case STRING: { // actual input-value is always string-based (text() from UI/QLineEdit
                           // stored as QVariant) so this test should never fail (but who knows?)
                           const QString string = value.toString();
                           if (string != QString()) valid = true;
                           break;
                         }
            case UUID: { const QUuid id = value.toUuid(); if (id != QUuid()) valid = true; break; }
            case DATE: { const QString dateAsString = value.toDateTime().toString(Qt::ISODate);
                         if (dateAsString != QString()) valid = true;
                         break;
                       }
            case BOOL: value = value.toBool(); valid = true; break;
            case INT: value.toInt(&valid); break;
            case FLOAT: value.toFloat(&valid); break;
            default: return false;
        }
        return valid;
    }
}

#endif // TYPES_H

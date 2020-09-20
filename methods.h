/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef METHODS_H
#define METHODS_H

#include <QColor>
#include <QList>
#include <QMap>
#include <QString>

namespace http {

    enum httpMethodType { UNKNOWN = -1, GET = 0, POST = 1, PUT = 2, DELETE = 3 };
    enum dataFlow { NOFLOW = -1, OUTPUT = 0, INPUT = 1 };

    struct httpMethod {

        httpMethodType _method;
        QColor _color;
        bool _bodyRequired;
        dataFlow _dtoObjectType;
        QString _iconPath;
    };

    // method name, { enum value, colour, contains body, DTO object type, arrow indicator }
    const static QMap<QString, httpMethod> httpMethods = {
        { QStringLiteral("GET"),
            { GET, QColor(97,175,254), false, http::OUTPUT, QStringLiteral("go-previous-brown") } },
        { QStringLiteral("POST"),
            { POST, QColor(73,204,144), true, http::INPUT, QStringLiteral("go-next-brown") } },
        { QStringLiteral("PUT"),
            { PUT, QColor(252,161,48), true, http::INPUT, QStringLiteral("go-next-brown") } },
        { QStringLiteral("DELETE"),
            { DELETE, QColor(249,62,62), false, http::NOFLOW, QString() } }
    };

    inline QString convertEnumValueToText(const httpMethodType type) {

        const QList<httpMethod> methodsEnumValues = httpMethods.values();
        const QList<QString> methodsStringValues = httpMethods.keys();

        for (int i = 0; i < methodsEnumValues.size(); ++i)
            if (methodsEnumValues.at(i)._method == type)
                return methodsStringValues[i];

        return QString();
    }
}

#endif // METHODS_H

/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <QJsonObject>
#include <QRegularExpression>
#include <QVariant>
#include <QVector>
#include <tuple>
#include "methods.h"

class Attributes {

    public:
        Attributes() = delete;
        Attributes(const QString &, const QString &, const http::dataFlow = http::NOFLOW);
        ~Attributes() {}

        inline QString name() const { return _variableName; }
        inline QString type() const { return _dataType; }
        inline QVariant value() const { return _value; }
        inline bool useInRequest() const { return _useInRequest; }
        inline void changeUseInRequest() { _useInRequest ^= true; return; }
        inline void setValue(const QVariant & value) { _value = value; return; }

    private:
        QString _variableName;
        QString _dataType;
        QVariant _value;
        bool _useInRequest;
};

class Parameters {

    public:
        Parameters() = delete;
        Parameters(const QString & name, const bool required, const QString & type):
            _name(name), _required(required), _dataType(type) {}
        ~Parameters() {}

        enum paramsState { UNKNOWN_STATE = 0, SUPPLIED = 1, REQUIRED_AND_NOT_SUPPLIED = 2,
                           NOT_REQUIRED_AND_NOT_SUPPLIED = 3};

        inline QString name() const { return _name; }
        inline QString type() const { return _dataType; }
        inline bool required() const { return _required; }
        inline QVariant value() const { return _value; }
        inline void setValue(const QVariant & value) { _value = value; }

    private:
        QString _name;
        bool _required;
        QString _dataType;
        QVariant _value;
};

class Endpoint {

    public:
        Endpoint(): _path(QString()), _summary(QString()), _httpMethod(QString()),
                    _dtoLabel(QString()), _parameters(new QVector<Parameters>),
                    _dataTransferObject(new QVector<Attributes>) {}
        Endpoint(const QString &, const QString &);
        Endpoint(const Endpoint &);
        Endpoint & operator=(const Endpoint &);
        ~Endpoint() { delete _dataTransferObject; delete _parameters; }

        const static QRegularExpression paramsRegex;

        inline QString path() const { return _path; }
        QString pathWithParameters() const;
        inline QString completePath() const { return _httpMethod + " " + _path; }
        inline QString summary() const { return _summary; }
        inline QString method() const { return _httpMethod; }
        inline QString dtoLabel() const { return _dtoLabel; }
        inline QVector<Parameters> * parameters() const { return _parameters; }
        inline QVector<Attributes> * attributes() const { return _dataTransferObject; }
        inline void setSummary(const QString & summary) { _summary = summary; return; }

        bool allAttributesSelected() const;
        QString buildSelectClause() const;

        inline static Endpoint * currentEndpoint() { return _currentEndpoint; }
        inline static void setCurrentEndpoint(Endpoint * const endpoint = nullptr)
            { Endpoint::_currentEndpoint = endpoint; return; }
        inline static void eraseCurrentEndpoint() { setCurrentEndpoint(); return; }

        inline bool hasPathParams() const { return !(_parameters->isEmpty()); }
        bool isAtLeastOnePathParameterSupplied() const;
        bool areRequiredPathParamsSupplied() const;
        inline bool hasBodyAttributes() const { return !(_dataTransferObject->isEmpty()); }
        bool isAtLeastOneBodyAttributeSupplied() const;
        bool itemNotEligible() const;
        inline bool propertiesAreNotSet() const
            { return (summary().isEmpty() && !hasPathParams() && !hasBodyAttributes()); }

        QStringList extractInputParamsFromPath() const;
        void setParams(const QJsonObject &);
        void setDtoInputDefinitionLink(const QJsonObject &);
        void setDtoOutputDefinitionLink(const QJsonObject &);
        void setDtoAttributes(const QJsonObject &, const http::dataFlow);

        inline bool operator<(const Endpoint & rhs) const
            { return std::tie(_path, _httpMethod) < std::tie(rhs._path, rhs._httpMethod); }
        inline bool operator==(const Endpoint & rhs) const
            { return std::tie(_path, _httpMethod) == std::tie(rhs._path, rhs._httpMethod); }

    private:
        inline QString makeType(const QJsonObject &) const;
        static Endpoint * _currentEndpoint;
        QString _path;
        QString _summary;
        QString _httpMethod;
        QString _dtoLabel;
        QVector<Parameters> * _parameters;
        QVector<Attributes> * _dataTransferObject;
};

#endif // ENDPOINT_H

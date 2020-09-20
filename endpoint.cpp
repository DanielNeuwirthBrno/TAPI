/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <QJsonArray>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QStringList>
#include "endpoint.h"

Attributes::Attributes(const QString & name, const QString & type, const http::dataFlow flow):
    _variableName(name), _dataType(type) {

    switch(flow) {
        case http::OUTPUT: _useInRequest = true; break;
        case http::INPUT: _useInRequest = false; break;
        default: _useInRequest = false;
    };
};

const QRegularExpression Endpoint::paramsRegex =
    QRegularExpression(QStringLiteral("\\{[a-zA-Z]+?\\}"));

Endpoint * Endpoint::_currentEndpoint = nullptr;

Endpoint::Endpoint(const QString & path, const QString & method):
    _path(path), _summary(QString()), _httpMethod(method), _dtoLabel(QString()),
    _parameters(new QVector<Parameters>), _dataTransferObject(new QVector<Attributes>) {}

Endpoint::Endpoint(const Endpoint & e2) {

    _path = e2._path;
    _summary = e2._summary;
    _httpMethod = e2._httpMethod;
    _dtoLabel = e2._dtoLabel;

    _parameters = new(QVector<Parameters>);
    *(_parameters) = *(e2._parameters);

    _dataTransferObject = new(QVector<Attributes>);
    *(_dataTransferObject) = *(e2._dataTransferObject);
}

Endpoint & Endpoint::operator=(const Endpoint & e2) {

    if (this != &e2) {

        _path = e2._path;
        _summary = e2._summary;
        _httpMethod = e2._httpMethod;
        _dtoLabel = e2._dtoLabel;

        _parameters = new(QVector<Parameters>);
        *(_parameters) = *(e2._parameters);

        _dataTransferObject = new(QVector<Attributes>);
        *(_dataTransferObject) = *(e2._dataTransferObject);
    }
    return (*this);
}

QString Endpoint::pathWithParameters() const {

    if (!this->hasPathParams())
        return _path;

    QString pathWithParameters = _path;
    QRegularExpressionMatchIterator allMatches = this->paramsRegex.globalMatch(_path);

    while (allMatches.hasNext()) {

        const QRegularExpressionMatch match = allMatches.next();
        const QString pathParam = match.captured();

        for (auto it: *(_parameters))
            if (it.name() == pathParam) {

                pathWithParameters.replace(pathParam, it.value().toString());
                break;
            }
    }    
    return pathWithParameters.remove(QRegularExpression("[{|}]"));
}

bool Endpoint::allAttributesSelected() const {

    uint16_t noOfSelectedAttributes = 0;
    for (auto it: *(this->_dataTransferObject))
        if (it.useInRequest())
            ++noOfSelectedAttributes;

    return (noOfSelectedAttributes == _dataTransferObject->size());
}

QString Endpoint::buildSelectClause() const {

    QString selectClause = QString();

    // all attributes => no select
    if (this->allAttributesSelected())
        return selectClause;

    for (auto it: *(this->_dataTransferObject))
        if (it.useInRequest())
            selectClause += it.name() + QStringLiteral(",");

    if (selectClause != QString())
        selectClause.chop(1);

    return selectClause;
}

bool Endpoint::isAtLeastOnePathParameterSupplied() const {

    if (!this->hasPathParams())
        return true;

    for (auto it: *(this->_parameters))
        if (!it.value().isNull())
            return true;

    return false;
}

bool Endpoint::areRequiredPathParamsSupplied() const {

    if (!this->hasPathParams())
        return true;

    for (auto it: *(this->_parameters))
        if (it.required() && it.value().isNull())
            return false;

    return true;
}

bool Endpoint::isAtLeastOneBodyAttributeSupplied() const {

    // zero attribs = OK; not required attribs only (GET) = OK
    if (!this->hasBodyAttributes() || !http::httpMethods[_httpMethod.toUpper()]._bodyRequired)
        return true;

    for (auto it: *(this->_dataTransferObject))
        if (!it.value().isNull())
            return true;

    return false;
}

bool Endpoint::itemNotEligible() const {

    const bool paramsMissing = path().contains(paramsRegex) && !hasPathParams();
    const bool attributesMissing =
        http::httpMethods[_httpMethod.toUpper()]._bodyRequired && !hasBodyAttributes();

    return (paramsMissing || attributesMissing);
}

QStringList Endpoint::extractInputParamsFromPath() const {

    QStringList params;
    QRegularExpressionMatchIterator allMatches = paramsRegex.globalMatch(this->_path);

    while (allMatches.hasNext())  {

        const QRegularExpressionMatch match = allMatches.next();
        params << match.captured();
    }
    return params;
}

void Endpoint::setParams(const QJsonObject & method) {

    this->_parameters->clear();

    const QStringList listOfParamNames = extractInputParamsFromPath();
    const QJsonArray paramsArray = method["parameters"].toArray();

    for (int i = 0; i < paramsArray.size(); ++i) {

        const QJsonObject paramsObject(paramsArray[i].toObject());

        // path parameters
        for (auto it: listOfParamNames) {

            if (paramsObject["name"].toString() == (it.mid(1,it.length()-2))) {

                const bool required = paramsObject["required"].toBool();
                const QString dataType = this->makeType(paramsObject);
                const Parameters params(it, required, dataType);
                this->_parameters->push_back(params);
            }
        }

        // query parameters
    }
    return;
}

void Endpoint::setDtoInputDefinitionLink(const QJsonObject & method) {

    // for POST/PUT are DTOs under /parameters section
    const QJsonArray paramsArray = method["parameters"].toArray();

    // definition for standard POST/PUT requests: /items/schema/items/ref
    // definition for PUT /UpdateCompany request: /item/schema/ref
    // definition for Request type POST requests: /data/schema/ref
    // definition for Request type PUT requests: /resultData/schema/ref
    const QStringList elements = { "items", "item", "data", "resultData" };

    for (int i = 0; i < paramsArray.size(); ++i) {

        const QJsonObject paramsObject(paramsArray[i].toObject());

        if (elements.contains(paramsObject["name"].toString(), Qt::CaseInsensitive)) {

            QJsonObject schemaObject = paramsObject["schema"].toObject();
            if (schemaObject["items"].isObject())
                schemaObject = schemaObject["items"].toObject();
            QString dtoLink = schemaObject["$ref"].toString();
            dtoLink = dtoLink.right(dtoLink.length() - dtoLink.lastIndexOf('/') - 1);
            this->_dtoLabel = dtoLink;
        }
    }
    return;
}

void Endpoint::setDtoOutputDefinitionLink(const QJsonObject & method) {

    // for GET are DTOs under /responses section
    const QJsonObject respObject = method["responses"].toObject();

    // definition for standard GET requests: /200/schema/items/ref
    // definition for GET/{id} requests: /200/schema/ref

    QJsonObject schemaObject = respObject["200"].toObject();
    schemaObject = schemaObject["schema"].toObject();
    if (schemaObject["items"].isObject())
        schemaObject = schemaObject["items"].toObject();
    QString dtoLink = schemaObject["$ref"].toString();
    dtoLink = dtoLink.right(dtoLink.length() - dtoLink.lastIndexOf('/') - 1);
    this->_dtoLabel = dtoLink;

    return;
}

void Endpoint::setDtoAttributes(const QJsonObject & definitions, const http::dataFlow flow) {

    this->_dataTransferObject->clear();
    if (!definitions[this->_dtoLabel].isObject())
        return;

    const QJsonObject dtoObject(definitions[this->_dtoLabel].toObject());
    const QJsonObject propertiesObject = dtoObject["properties"].toObject();
    const QStringList properties = propertiesObject.keys();

    for (auto it: properties) {

        const QJsonObject attributeObject = propertiesObject[it].toObject();
        const QString dataType = this->makeType(attributeObject);
        const Attributes dtoAttributes(it, dataType, flow);
        this->_dataTransferObject->push_back(dtoAttributes);
    }
    return;
}

QString Endpoint::makeType(const QJsonObject & params) const {

    QString delimiter = QStringLiteral("/");

    if (params["type"].toString().isEmpty() || params["format"].toString().isEmpty())
        delimiter.clear();

    return (params["type"].toString() + delimiter + params["format"].toString());
}

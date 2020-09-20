/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <QMessageBox>
#include <QPair>
#include <QSize>
#include <QUuid>
#include <QVector>
#include "buildrequestwindow.h"
#include "random.h"
#include "types.h"

bool BuildRequestWindow::_repaintWindow = true;

BuildRequestWindow::BuildRequestWindow(Endpoint * currentEndpoint, QWidget * parent):
    QDialog(parent), _endpoint(currentEndpoint), ui(new Ui_BuildRequestWindow) {

    ui->setupUi(this, _endpoint);

    random::seedRandomGenerator();

    connect(ui->parametersListTable, &QTableWidget::itemChanged,
            this, &BuildRequestWindow::setParameterValue);
    connect(ui->attributesListTable, &QTableWidget::itemChanged,
            this, &BuildRequestWindow::setAttributeValue);
    connect(ui->attributesListTable, &QTableWidget::cellDoubleClicked,
            this, &BuildRequestWindow::changeUseInRequestValue);

    connect(ui->selectAllButton, &QPushButton::clicked,
            this, &BuildRequestWindow::selectAllAttributes);
    connect(ui->deselectAllButton, &QPushButton::clicked,
            this, &BuildRequestWindow::deselectAllAttributes);

    connect(ui->generateParametersButton, &QPushButton::clicked,
            this, &BuildRequestWindow::generateParameters);
    connect(ui->generateAttributesButton, &QPushButton::clicked,
            this, &BuildRequestWindow::generateAttributes);
    connect(ui->generateButton, &QPushButton::clicked,
            this, &BuildRequestWindow::generateAllData);

    connect(ui->clearParametersButton, &QPushButton::clicked,
            this, &BuildRequestWindow::clearParameters);
    connect(ui->clearAttributesButton, &QPushButton::clicked,
            this, &BuildRequestWindow::clearAttributes);
    connect(ui->clearButton, &QPushButton::clicked, this, &BuildRequestWindow::clearAllData);

    connect(ui->confirmButton, &QPushButton::clicked, this, &BuildRequestWindow::close);
}

void BuildRequestWindow::repaint() {

    if (_repaintWindow) {

        this->resize(QSize(this->parentWidget()->height(), this->width()));
        this->adjustSize();
        this->update();
    }
    return;
}

void BuildRequestWindow::showInvalidTypeErrorBox(const QString & value, const QString & type) const {

    QMessageBox * messageBox = new QMessageBox;
    const QIcon * icon = new QIcon(QStringLiteral(":/icons/icons/dialog-error.png"));
    messageBox->setWindowIcon(*icon);
    delete icon;

    messageBox->setWindowTitle(QStringLiteral("Nevalidní hodnota"));
    messageBox->setTextFormat(Qt::RichText);
    const QString headerText =
        QStringLiteral("<b>Zadaná hodnota: [") + value + QStringLiteral("]</b>");
    messageBox->setText(headerText);
    const QString infoText =
        QStringLiteral("Systém očekává hodnotu datového typu ") + type;
    messageBox->setInformativeText(infoText+".");
    const QString details = infoText + QStringLiteral(", na kterou nelze zadanou hodnotu ") +
                            value + QStringLiteral(" implicitně konvertovat.");
    messageBox->setDetailedText(details);
    messageBox->setIcon(QMessageBox::Warning);
    messageBox->setStandardButtons(QMessageBox::Ok);

    QGridLayout * layout = static_cast<QGridLayout *>(messageBox->layout());
    layout->setColumnMinimumWidth(2, 300);

    messageBox->exec();

    delete messageBox;
    return;
}

bool BuildRequestWindow::isValueConvertibleToType(QVariant & value, const QString & type,
                                                  QTableWidgetItem * const item) const {

    // check if assigned value is convertible to underlying data-type
    if (!value.isNull() && !types::isGivenValueForActualTypeValid(types::matchDataTypes[type], value)) {

        this->showInvalidTypeErrorBox(item->text(), type);
        item->setText(QString());
        // appropriate icon is automatically set due to subsequent itemChanged signal
        return false;
    }
    return true;
}

void BuildRequestWindow::setParameterStateIcon(QLabel * labelForIcon, const bool isRequired,
                                               const QVariant & value) const {

    const QPixmap * const requiredParamIcon = (!value.isNull())
        ? &ui->paramsStateIcons[Parameters::SUPPLIED]
        : (isRequired) ? &ui->paramsStateIcons[Parameters::REQUIRED_AND_NOT_SUPPLIED]
                       : &ui->paramsStateIcons[Parameters::NOT_REQUIRED_AND_NOT_SUPPLIED];
    labelForIcon->setPixmap(*requiredParamIcon);
    labelForIcon->setAlignment(Qt::AlignCenter);

    return;
}

// [slot]
void BuildRequestWindow::setParameterValue(QTableWidgetItem * const item) {

    const int currentRow = item->row();
    QVariant currentValue = item->text();
    Parameters & currentParameter = (*this->_endpoint->parameters())[currentRow];

    if (!isValueConvertibleToType(currentValue, currentParameter.type(), item))
        return;

    // (in case of bool variable) convert whatever text was entered to true/false value
    if (types::matchDataTypes[currentParameter.type()] == types::BOOL) {

        const QString textBoolValue = types::convertBoolToText(currentValue);
        item->setText(textBoolValue);
    }
    currentParameter.setValue(currentValue);

    // change state-of-parameter icon
    QLabel * labelForParamsIcon = new QLabel;
    setParameterStateIcon(labelForParamsIcon, currentParameter.required(), currentValue);
    ui->parametersListTable->setCellWidget(currentRow, 3, labelForParamsIcon);

    ui->parametersListTable->resizeColumnToContents(2);
    if ((ui->attributesListTable->columnWidth(2)) >
        (ui->parametersListTable->columnWidth(2)))
        ui->parametersListTable->setColumnWidth(2, ui->attributesListTable->columnWidth(2));

    repaint();
    return;
}

// [slot]
void BuildRequestWindow::setAttributeValue(QTableWidgetItem * const item) {

    const int currentRow = item->row();
    QVariant currentValue = item->text();
    Attributes & currentAttribute = (*this->_endpoint->attributes())[currentRow];

    if (!isValueConvertibleToType(currentValue, currentAttribute.type(), item))
        return;

    // (in case of bool variable) convert whatever text was entered to true/false value
    if (types::matchDataTypes[currentAttribute.type()] == types::BOOL) {

        const QString textBoolValue = types::convertBoolToText(currentValue);
        item->setText(textBoolValue);
    }
    currentAttribute.setValue(currentValue);

    // change state-of-attribute icon
    if ((!currentValue.isNull() && !currentAttribute.useInRequest()) ||
        (currentValue.isNull() && currentAttribute.useInRequest()))
        this->changeUseInRequestValue(currentRow);

    ui->attributesListTable->resizeColumnToContents(2);
    if ((ui->parametersListTable->columnWidth(2)) >
        (ui->attributesListTable->columnWidth(2)))
        ui->attributesListTable->setColumnWidth(2, ui->parametersListTable->columnWidth(2));

    repaint();
    return;
}

// [slot]
void BuildRequestWindow::changeUseInRequestValue(const int row, const int column) {

    if (column != 3)
        return;

    Attributes & currentAttribute = (*this->_endpoint->attributes())[row];
    currentAttribute.changeUseInRequest();

    QLabel * labelForAttribsIcon = new QLabel;
    const QPair<QPixmap, uint8_t> * const requestIcon =
        (currentAttribute.useInRequest()) ? ui->useInRequestIcon : ui->notUseInRequestIcon;
    labelForAttribsIcon->setPixmap
        (requestIcon->first.scaled(requestIcon->second, requestIcon->second, Qt::KeepAspectRatio));
    labelForAttribsIcon->setAlignment(Qt::AlignCenter);

    ui->attributesListTable->setCellWidget(row, column, labelForAttribsIcon);
}

// [slot]
void BuildRequestWindow::generateParameters() {

    _repaintWindow = false;

    QVector<Parameters>::iterator it;
    QTableWidgetItem * item;
    uint8_t row = 0;

    for (it = _endpoint->parameters()->begin(); it != _endpoint->parameters()->end(); ++it) {

        if (it+1 == _endpoint->parameters()->end())
            _repaintWindow = true;

        QVariant newValue = random::randomValue(it->type());

        item = ui->parametersListTable->item(row++, 2);
        item->setText(newValue.toString());
        // underlying value is automatically cleared due to itemChanged signal
        // it->setValue(newValue);
    }

    _repaintWindow = true;
    return;
}

// [slot]
void BuildRequestWindow::selectAllAttributes(const bool deselect) {

    QVector<Attributes>::iterator it;
    uint16_t row = 0;

    for (it = _endpoint->attributes()->begin(); it != _endpoint->attributes()->end(); ++it, ++row) {

        if ((!it->useInRequest()) ^ deselect)
          this->changeUseInRequestValue(row);
    }
    return;
}

// [slot]
void BuildRequestWindow::generateAttributes() {

    _repaintWindow = false;

    QVector<Attributes>::iterator it;
    QTableWidgetItem * item;
    uint16_t row = 0;

    for (it = _endpoint->attributes()->begin(); it != _endpoint->attributes()->end(); ++it) {

        if (it+1 == _endpoint->attributes()->end())
            _repaintWindow = true;

        QVariant newValue = random::randomValue(it->type());

        item = ui->attributesListTable->item(row++, 2);
        item->setText(newValue.toString());
        // underlying value is automatically cleared due to itemChanged signal
        // it->setValue(newValue);
    }

    _repaintWindow = true;
    return;
}

// [slot]
void BuildRequestWindow::generateAllData() {

    generateParameters();
    generateAttributes();

    return;
}

// [slot]
void BuildRequestWindow::clearParameters() {

    _repaintWindow = false;

    QVector<Parameters>::iterator it;
    QTableWidgetItem * item;
    uint8_t row = 0;

    for (it = _endpoint->parameters()->begin(); it != _endpoint->parameters()->end(); ++it) {

        if (it+1 == _endpoint->parameters()->end())
            _repaintWindow = true;

        item = ui->parametersListTable->item(row++, 2);
        item->setText(QString());
        // underlying value is automatically cleared due to itemChanged signal
        // it->setValue(QVariant());
    }

    _repaintWindow = true;
    return;
}

// [slot]
void BuildRequestWindow::clearAttributes() {

    _repaintWindow = false;

    QVector<Attributes>::iterator it;
    QTableWidgetItem * item;
    uint16_t row = 0;

    for (it = _endpoint->attributes()->begin(); it != _endpoint->attributes()->end(); ++it) {

        if (it+1 == _endpoint->attributes()->end())
            _repaintWindow = true;

        item = ui->attributesListTable->item(row++, 2);
        item->setText(QString());
        // underlying value is automatically cleared due to itemChanged signal
        // it->setValue(QVariant());
        if (it->useInRequest())
          this->changeUseInRequestValue(row-1);
    }

    _repaintWindow = true;
    return;
}

// [slot]
void BuildRequestWindow::clearAllData() {

    clearParameters();
    clearAttributes();

    return;
}

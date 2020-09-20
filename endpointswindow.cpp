/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include "buildrequestwindow.h"
#include "endpointswindow.h"
#include "pathwindow.h"

EndpointsWindow::EndpointsWindow(QVector<Endpoint> * endpoints, QWidget * parent):
    QDialog(parent), _endpoints(endpoints), ui(new Ui_EndpointsWindow) {

    ui->setupUi(this, _endpoints);

    connect(ui->endpointsListTable, &QTableWidget::currentItemChanged,
            this, &EndpointsWindow::showSummary);
    connect(ui->endpointsListTable, &QTableWidget::currentItemChanged,
            this, &EndpointsWindow::enableAcceptRequestButton);

    // connect(ui->header, &QHeaderView::sectionClicked,
    //         this, &EndpointsWindow::sortSelectedColumn);
    connect(ui->searchBox, &QLineEdit::textEdited,
            this, &EndpointsWindow::moveCursorDuringSearch);
    connect(ui->searchButton, &QPushButton::clicked,
            this, &EndpointsWindow::moveCursorDuringSearch);
    connect(ui->inputParamsPathButton, &QPushButton::clicked,
            this, &EndpointsWindow::displayPathWindow);
    connect(ui->buildRequestButton, &QPushButton::clicked,
            this, &EndpointsWindow::buildRequestWindow);
    connect(ui->buildRequestButton, &QPushButton::clicked, this, [this]() -> void
            { this->enableAcceptRequestButton(ui->endpointsListTable->currentItem()); } );
    connect(ui->acceptRequestButton, &QPushButton::clicked,
            this, &EndpointsWindow::acceptCurrentEndpoint);
    connect(ui->cancelButton, &QPushButton::clicked, this, &EndpointsWindow::close);
}

// [slot]
void EndpointsWindow::showSummary(QTableWidgetItem * const item) {

    const Endpoint currentEndpoint = this->_endpoints->at(item->row());

    ui->descriptionTextEdit->setPlainText(currentEndpoint.summary());
    ui->inputParamsPathButton->setEnabled(currentEndpoint.hasPathParams());
    ui->inputParamsBodyButton->setEnabled(currentEndpoint.hasBodyAttributes());

    return;
}

// [slot]
void EndpointsWindow::enableAcceptRequestButton(QTableWidgetItem * const item) {

    bool isCurrentRequestAcceptable = false;
    const Endpoint currentEndpoint = this->_endpoints->at(item->row());

    // no params, no attribs
    if (!currentEndpoint.hasPathParams() && !currentEndpoint.hasBodyAttributes())
        isCurrentRequestAcceptable = true;

    // required params supplied, no attribs
    if (currentEndpoint.areRequiredPathParamsSupplied() && !currentEndpoint.hasBodyAttributes())
        isCurrentRequestAcceptable = true;

    // no params, some attribs supplied
    if (!currentEndpoint.hasPathParams() && currentEndpoint.isAtLeastOneBodyAttributeSupplied())
        isCurrentRequestAcceptable = true;

    // required params supplied, some attribs supplied
    if (currentEndpoint.areRequiredPathParamsSupplied() && currentEndpoint.isAtLeastOneBodyAttributeSupplied())
        isCurrentRequestAcceptable = true;

    ui->acceptRequestButton->setEnabled(isCurrentRequestAcceptable);
    return;
}

// [slot]
void EndpointsWindow::sortSelectedColumn(const int column) const {

    if (column == 1 || column == 2)
        ui->endpointsListTable->sortItems(column);

    return;
}

// [slot]
void EndpointsWindow::moveCursorDuringSearch() const {

    if (ui->searchBox->text().isEmpty())
        return;

    const int currentRow = ui->endpointsListTable->currentItem()->row();
    const int noOfColumns = ui->endpointsListTable->columnCount();
    // search via "next" button starts at subsequent row
    const uint8_t moveToNextRow = (ui->searchButton->hasFocus()) ? 1 : 0;

    auto it = std::next(ui->endpointsListTableItems.begin(),
                        noOfColumns * (currentRow + moveToNextRow) + 1);
    const auto it_end =
      (it == ui->endpointsListTableItems.begin()) ? ui->endpointsListTableItems.end() : (it-1);

    for (; it != it_end; ++it) {

        // wrap around search
        if (it == ui->endpointsListTableItems.end())
            it = ui->endpointsListTableItems.begin();

        if ((*it)->column() == 2 /* this column contains path */ &&
            (*it)->flags() != Qt::NoItemFlags /* browse through active items only */ &&
            (*it)->text().contains(ui->searchBox->text(), Qt::CaseInsensitive)) {

            ui->endpointsListTable->setCurrentItem(*it);
            ui->endpointsListTable->currentItem()->setSelected(true);
            break;
        }
    }
    return;
}

// [slot]
void EndpointsWindow::acceptCurrentEndpoint() {

    const int row = ui->endpointsListTable->currentRow();
    Endpoint::setCurrentEndpoint(&(*_endpoints)[row]);

    this->close();
    return;
}

// [slot]
int EndpointsWindow::displayPathWindow() {

    const Endpoint currentEndpoint = this->_endpoints->at(ui->endpointsListTable->currentRow());

    PathWindow pathWindow(currentEndpoint, this);
    return pathWindow.exec();
}

// [slot]
int EndpointsWindow::buildRequestWindow() {

    const int row = ui->endpointsListTable->currentRow();
    // a little bit of black magic and here we are:
    Endpoint * currentEndpoint = &((*(this->_endpoints))[row]);

    BuildRequestWindow buildRequestWindow(currentEndpoint, this);
    return buildRequestWindow.exec();
}

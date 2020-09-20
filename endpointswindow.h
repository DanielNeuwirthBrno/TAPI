/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef ENDPOINTSWINDOW_H
#define ENDPOINTSWINDOW_H

#include <QWidget>
#include "endpoint.h"
#include "ui/ui_endpointswindow.h"

class EndpointsWindow: public QDialog {

    Q_OBJECT

    public:
        explicit EndpointsWindow(QVector<Endpoint> *, QWidget * = nullptr);
        ~EndpointsWindow() { delete ui; }

    private:
        QVector<Endpoint> * _endpoints;
        Ui_EndpointsWindow * ui;

    private slots:
        void moveCursorDuringSearch() const;
        void showSummary(QTableWidgetItem * const);
        void enableAcceptRequestButton(QTableWidgetItem * const);
        void sortSelectedColumn(const int) const;
        void acceptCurrentEndpoint();
        int displayPathWindow();
        int buildRequestWindow();
};

#endif // ENDPOINTSWINDOW_H

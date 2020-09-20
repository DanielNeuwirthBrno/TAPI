/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef PATHWINDOW_H
#define PATHWINDOW_H

#include <QWidget>
#include "endpoint.h"
#include "ui/ui_pathwindow.h"

class PathWindow: public QDialog {

    Q_OBJECT

    public:
        explicit PathWindow(const Endpoint & currentEndpoint, QWidget * parent = nullptr):
            QDialog(parent), _endpoint(currentEndpoint), ui(new Ui_PathWindow) {

            ui->setupUi(this, _endpoint);

            connect(ui->cancelButton, &QPushButton::clicked, this, &PathWindow::close);
        }
        ~PathWindow() { delete ui; }

    private:
        Endpoint _endpoint;
        Ui_PathWindow * ui;
};

#endif // PATHWINDOW_H

/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef REQUESTWINDOW_H
#define REQUESTWINDOW_H

#include <QWidget>
#include "request.h"
#include "ui/ui_requestwindow.h"

class RequestWindow: public QDialog {

    Q_OBJECT

    public:
        explicit RequestWindow(const Request & request, const QVariant ID, QWidget * parent = nullptr):
            QDialog(parent), ui(new Ui_RequestWindow) {

            ui->setupUi(this, request, ID);

            connect(ui->closeButton, &QPushButton::clicked, this, &RequestWindow::close);
        }
        ~RequestWindow() { delete ui; }

    private:
        Ui_RequestWindow * ui;
};

#endif // REQUESTWINDOW_H

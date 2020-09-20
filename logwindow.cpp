/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include "logwindow.h"
#include "requestwindow.h"

LogWindow::LogWindow(const QVector<Communication> & comm, QWidget * parent):
    QDialog(parent), _communication(comm), ui(new Ui_LogWindow) {

    ui->setupUi(this, _communication);

    connect(ui->displayRequestButton, &QPushButton::clicked,
            this, &LogWindow::displayRequestWindow);
    connect(ui->closeButton, &QPushButton::clicked, this, &LogWindow::close);
}

// [slot]
int LogWindow::displayRequestWindow() {

    const int row = ui->listOfCommunicationTable->currentRow();

    RequestWindow requestWindow(_communication.at(row).request(), _communication.at(row).ID(), this);
    return requestWindow.exec();
}

// [slot]
int LogWindow::displayResponseWindow() {

    return 0;
}

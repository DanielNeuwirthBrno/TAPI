/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include "requestwindow.h"
#include "responsewindow.h"

ResponseWindow::ResponseWindow(const QNetworkReply * const reply, Session * const currentSession,
    QWidget * parent): QDialog(parent), _ID(reply->request().attribute(Request::userAttribute(1))),
    _httpMethod(reply->operation()), _currentSession(currentSession), ui(new Ui_ResponseWindow) {

    // match current response to previously saved record
    QVector<Communication>::reverse_iterator it = _currentSession->communication().rbegin();
    for (; it != _currentSession->communication().rend(); ++it)
        if (it->ID() == _ID.toInt())
            break;

    ui->setupUi(this, reply, _ID, _recordIDs, it.base()-1);

    connect(ui->httpMethodButton, &QPushButton::clicked,
            this, &ResponseWindow::displayRequestWindow);
    connect(ui->verifyButton, &QPushButton::clicked, this, &ResponseWindow::verifyResults);
    connect(ui->closeButton, &QPushButton::clicked, this, &ResponseWindow::close);
}

// [slot]
void ResponseWindow::verifyResults() {

    if (ui->tableNameLineEdit->isHidden()) {

        const QString tableName = _currentSession->getTableName(_ID);
        ui->tableNameLineEdit->setHidden(false);
        ui->tableNameLineEdit->setText(tableName);
        ui->verifyButton->setText(QStringLiteral(" Ověřit "));
    }
    else {

        const Session::State verified =
            _currentSession->verifyTableRecords(ui->tableNameLineEdit->text(),
                                                _recordIDs, _httpMethod);
        ui->tableNameLineEdit->clear();
        ui->tableNameLineEdit->setHidden(true);

        switch (verified) {

            case Session::VERIFIED:
                ui->verifyButton->setText(QStringLiteral(" Ověřeno "));
                ui->verifyButton->setIcon(QIcon(QStringLiteral(
                    ":/icons/icons/preferences-desktop-notification-green.png")));
                break;

            case Session::NOT_VERIFIED_ERROR:
                ui->verifyButton->setText(QStringLiteral(" Neověřeno "));
                ui->verifyButton->setIcon(QIcon(QStringLiteral(
                    ":/icons/icons/preferences-desktop-notification-red.png")));
                break;

            case Session::NOT_VERIFIED:
            case Session::UNKNOWN:
            default:
                ui->verifyButton->setText(QStringLiteral(" Neověřeno "));
                ui->verifyButton->setIcon(QIcon(QStringLiteral(
                    ":/icons/icons/preferences-desktop-notification-yellow.png")));
        }
    }
    return;
}

// [slot]
int ResponseWindow::displayRequestWindow() {

    const Request & correspondingRequest =
        _currentSession->findCorrespondingRequest(_ID.toInt())->request();

    RequestWindow requestWindow(correspondingRequest, _ID, this);
    return requestWindow.exec();
}

/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef RESPONSEWINDOW_H
#define RESPONSEWINDOW_H

#include <QWidget>
#include "request.h"
#include "session.h"
#include "ui/ui_responsewindow.h"

class ResponseWindow: public QDialog {

    Q_OBJECT

    public:
        explicit ResponseWindow(const QNetworkReply * const, Session * const, QWidget * = nullptr);
        ~ResponseWindow() { delete ui; }

    private slots:
        void verifyResults();
        int displayRequestWindow();

    private:
        const QVariant _ID; // request ID
        QList<QString> _recordIDs; // IDs of inserted/deleted records
        QNetworkAccessManager::Operation _httpMethod;
        Session * const _currentSession;
        Ui_ResponseWindow * ui;
};

#endif // RESPONSEWINDOW_H

/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSqlError>
#include "session.h"
#include "ui/ui_mainwindow.h"

class MainWindow: public QDialog {

    Q_OBJECT

    public:
        explicit MainWindow(QWidget * = nullptr);
        ~MainWindow();

        inline void enableConnectButton() const
            { ui->connectToServerButton->setEnabled(this->_currentSession->allValuesSet()); }

        void showFileErrorBox(const QString &, const QString &, const err::fileError &,
                              const QString & = QString()) const;
        void showSwaggerErrorBox(const err::swaggerError) const;

        Ui_MainWindow * ui;

    private:
        void getEndpoints() const;
        void processSwaggerFile() const;
        void processSwaggerWebSource() const;
        void cutText(QLineEdit * const, const int) const;
        void changeIconAccordingToTestConnectionResult(const StatusCode &) const;
        void processTokenReply(const StatusCode &, uint16_t) const;
        void processEndpointsReply(const StatusCode &, uint16_t) const;
        void processSwaggerDocsReply(const StatusCode &, uint16_t) const;
        void processGeneralRequestReply(const StatusCode &, uint16_t,
                                        const QNetworkAccessManager::Operation) const;
        inline bool isOutputMethod(const QString & currentMethod) const
           { return (http::httpMethods[currentMethod]._dtoObjectType == http::OUTPUT); }

        bool _delaySwaggerProcessing;
        Session * _currentSession;

    signals:
        void processingOfGeneralRequestFinished(const QNetworkReply * const) const;

    private slots:
        void selectConfigFile();
        void parseConfigFile() const;

        void setApiServerAddress();
        void loadClientParams() const;
        void testApiConnection() const;

        void enableGenerateButton() const;
        void generateToken() const;
        void setUserAssignedToken() const;

        void enableGetEndpointsButton() const;
        void getListOfEndpoints();

        void changeSwaggerLocation() const;
        void selectSwaggerFile(const bool = false);
        void selectSwaggerWebSource(const bool = false);

        void hideSelectAndFilterWidget() const;

        void setCurrentEndpoint() const;
        void enableSendRequestButton() const;
        void sendRequest() const;

        void processReceivedReply(const QNetworkReply * const) const;

        int displayTokenWindow();
        int displayEndpointsWindow();
        int displayResponseWindow(const QNetworkReply * const);
        int displayLogWindow();
};

#endif // MAINWINDOW_H

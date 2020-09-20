/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QWidget>
#include "ui/ui_logwindow.h"

class LogWindow: public QDialog {

    Q_OBJECT

    public:
        explicit LogWindow(const QVector<Communication> &, QWidget * = nullptr);
        ~LogWindow() { delete ui; }

    private:
        const QVector<Communication> & _communication;
        Ui_LogWindow * ui;

    private slots:
        int displayRequestWindow();
        int displayResponseWindow();
};

#endif // LOGWINDOW_H

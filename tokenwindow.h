/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef TOKENWINDOW_H
#define TOKENWINDOW_H

#include <QWidget>
#include "credentials.h"
#include "ui/ui_tokenwindow.h"

class TokenWindow: public QDialog {

    Q_OBJECT

    public:
        explicit TokenWindow(Token * const token, QWidget * parent = nullptr):
            QDialog(parent), ui(new Ui_TokenWindow) {

            const QString type =
                (token->type().isEmpty()) ? QStringLiteral("neznámý typ") : token->type();
            ui->setupUi(this, token->token(), type);

            connect(ui->closeButton, &QPushButton::clicked, this, &TokenWindow::close);
        }
        ~TokenWindow() { delete ui; }

    private:
        Ui_TokenWindow * ui;
};

#endif // TOKENWINDOW_H

/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef UI_TOKENWINDOW_H
#define UI_TOKENWINDOW_H

// user interface for Token class

#include <QDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include "credentials.h"

class Ui_TokenWindow {

    public:
        QIcon * tokenWindowIcon;
        QTextEdit * tokenDisplayArea;

        QHBoxLayout * buttonsLayout;
        QPushButton * closeButton;

        QVBoxLayout * windowLayout;

        void setupUi(QDialog * TokenWindow, const QString & token, const QString & type) {

            // properties of main window
            tokenWindowIcon = new QIcon(QStringLiteral(":/icons/icons/task-attempt.png"));
            TokenWindow->setWindowIcon(*tokenWindowIcon);
            const QString title = QStringLiteral("Token (") + type + QStringLiteral(")");
            TokenWindow->setWindowTitle(title);
            TokenWindow->resize(700,250);

            tokenDisplayArea = new QTextEdit(token);
            tokenDisplayArea->setReadOnly(true);
            tokenDisplayArea->setWordWrapMode(QTextOption::WrapAnywhere);

            // buttons
            buttonsLayout = new QHBoxLayout();
            closeButton = new QPushButton(QStringLiteral("Zavřít"));
            buttonsLayout->addStretch();
            buttonsLayout->addWidget(closeButton);

            windowLayout = new QVBoxLayout(TokenWindow);
            windowLayout->addWidget(tokenDisplayArea);
            windowLayout->addLayout(buttonsLayout);

            QMetaObject::connectSlotsByName(TokenWindow);
        }
};

#endif // UI_TOKENWINDOW_H

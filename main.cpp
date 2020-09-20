/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

/* Application:     Test S5API (tapi.exe)
 * Version:         0.51 (build-51)
 * Worktime:        184 hrs
 *
 * Author:          Daniel Neuwirth
 * E-mail:          d.neuwirth@tiscali.cz
 * Modified:        2020-03-29
 *
 * IDE/framework:   Qt 5.14.0
 * Compiler:        MinGW 7.3.0 32-bit
 * Language:        C++11
 */

#include <QApplication>
#include <QLibraryInfo>
#include <QString>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char * argv[])
{
    Q_INIT_RESOURCE(resource);

    QApplication app(argc, argv);

    // translation files
    QTranslator translator;
    QString path = QLibraryInfo::location(QLibraryInfo::PrefixPath); // deployment
    if (!translator.load("qtbase_cs.qm", path)) {

        path = QLibraryInfo::location(QLibraryInfo::TranslationsPath); // development
        translator.load("qtbase_cs.qm", path);
    }
    app.installTranslator(&translator);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

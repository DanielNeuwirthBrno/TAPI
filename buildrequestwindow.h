/*******************************************************************************
 Copyright 2019-20 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef BUILDREQUESTWINDOW_H
#define BUILDREQUESTWINDOW_H

#include <QLabel>
#include <QTableWidgetItem>
#include <QWidget>
#include "endpoint.h"
#include "ui/ui_buildrequestwindow.h"

class BuildRequestWindow: public QDialog {

    Q_OBJECT

    public:
        explicit BuildRequestWindow(Endpoint *, QWidget * = nullptr);
        ~BuildRequestWindow() { delete ui; }

    private:
        void repaint();
        void showInvalidTypeErrorBox(const QString &, const QString &) const;
        bool isValueConvertibleToType(QVariant &, const QString &, QTableWidgetItem * const) const;
        void setParameterStateIcon(QLabel *, const bool, const QVariant &) const;

        static bool _repaintWindow;
        Endpoint * _endpoint;
        Ui_BuildRequestWindow * ui;

    private slots:
        void setParameterValue(QTableWidgetItem * const);
        void setAttributeValue(QTableWidgetItem * const);
        void changeUseInRequestValue(const int, const int = 3);
        void generateParameters();
        void selectAllAttributes(const bool = false);
        inline void deselectAllAttributes() { this->selectAllAttributes(true); return; }
        void generateAttributes();
        void generateAllData();
        void clearParameters();
        void clearAttributes();
        void clearAllData();
};

#endif // BUILDREQUESTWINDOW_H

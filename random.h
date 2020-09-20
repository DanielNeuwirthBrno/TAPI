/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#ifndef RANDOM_H
#define RANDOM_H

#include <QDateTime>
#include <QString>
#include <QVariant>

namespace random {

    const static struct RandomGeneratorRules {

        const uint16_t stringMaxLength = 10;
        const uint8_t lowercaseLetters = 26;
        const uint8_t shiftLetterValue = 97;

    } rules;

    void seedRandomGenerator(unsigned int = 0);

    QString generateRandomString(const int = rules.stringMaxLength);
    QDateTime generateRandomDate(QDateTime = QDateTime::currentDateTime().addDays(-30),
                                 QDateTime = QDateTime::currentDateTime());
    bool generateRandomBool();
    int32_t generateRandomInt(const int32_t = 100, const bool = true);
    double generateRandomFloat(const double = 10000.0, const bool = true, const uint8_t = 4);

    QVariant randomValue(const QString &);
}

#endif // RANDOM_H

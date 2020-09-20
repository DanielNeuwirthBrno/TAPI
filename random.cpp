/*******************************************************************************
 Copyright 2019 Daniel Neuwirth
 This program is distributed under the terms of the GNU General Public License.
*******************************************************************************/

#include <QUuid>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <utility>
#include "random.h"
#include "types.h"

void random::seedRandomGenerator(unsigned int seed) {

    const unsigned int seedValue = (seed == 0) ? std::time(nullptr) : seed;
    std::srand(seedValue);

    return;
}

QString random::generateRandomString(const int maxLength) {

    QString generatedString = QString();

    double randomValue = static_cast<double>(std::rand());
    const uint16_t stringLength =
            static_cast<uint16_t>((randomValue / RAND_MAX) * maxLength) + 1;

    for (uint16_t i = 0; i < stringLength; ++i) {

        randomValue = static_cast<double>(std::rand());
        const uint8_t generatedValue =
                static_cast<uint8_t>((randomValue / RAND_MAX) * rules.lowercaseLetters);

        const char generatedLetter = generatedValue + rules.shiftLetterValue; // a-z
        generatedString += generatedLetter;
    }

    return generatedString;
}

QDateTime random::generateRandomDate(QDateTime minDate, QDateTime maxDate) {

    if (minDate > maxDate)
        std::swap(minDate, maxDate);

    const int spanBetweenDates = minDate.daysTo(maxDate);
    const int numberOfDaysToAdd = generateRandomInt(spanBetweenDates, true);
    const QDateTime dateValue = minDate.addDays(numberOfDaysToAdd);

    return dateValue;
}

bool random::generateRandomBool() {

    const double randomValue = static_cast<double>(std::rand());
    const uint8_t boolValue = static_cast<uint8_t>((randomValue / RAND_MAX) + 0.5);

    return static_cast<bool>(boolValue);
}

int32_t random::generateRandomInt(const int32_t maxValue, const bool onlyPositive) {

    const double randomValue = static_cast<double>(std::rand());
    int32_t intValue = static_cast<int32_t>((randomValue / RAND_MAX) * maxValue);
    if (onlyPositive && intValue < 0)
        intValue /= -1;

    return intValue;
}

double random::generateRandomFloat(const double maxValue, const bool onlyPositive,
                                   const uint8_t numberOfDecimalPlaces) {

    const double randomValue = static_cast<double>(std::rand());
    double floatValue = (randomValue / RAND_MAX) * maxValue;
    floatValue = [&]() -> double { int multiplier = 1;
                                   for (int i = 0; i < numberOfDecimalPlaces; ++i, multiplier*=10);
                                   return (std::round(floatValue * multiplier) / multiplier);
                                 }();
    if (onlyPositive)
        floatValue = std::abs(floatValue);

    return floatValue;
}

QVariant random::randomValue(const QString & type) {

    QVariant newValue = QVariant();

    switch (types::matchDataTypes[type]) {

        case types::STRING: newValue = generateRandomString(); break;
        case types::UUID: newValue = QUuid::createUuid(); break;
        case types::DATE: newValue = generateRandomDate(); break;
        case types::BOOL: newValue = generateRandomBool(); break;
        case types::INT: newValue = generateRandomInt(); break;
        case types::FLOAT: newValue = generateRandomFloat(); break;
        default: ; // chyba pri generovani parametru - osetrit
    };

    return newValue;
}

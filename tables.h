#ifndef TABLES_H
#define TABLES_H

#include <QMap>
#include <QPair>
#include <QString>

// key: endpoint name, value: table name, docDb?
const static QMap<QString, QPair<QString, bool>> dbTables = {
    { QStringLiteral("Activity"),
    { QStringLiteral("Adresar_Aktivita"), false } },
    { QStringLiteral("Article"),
    { QStringLiteral("Artikly_Artikl"), false } },
    { QStringLiteral("Attachment"),
    { QStringLiteral("System_Attachment"), true } },
    { QStringLiteral("BankStatement"),
    { QStringLiteral("Finance_BankovniVypis"), false } },
    { QStringLiteral("Centre"),
    { QStringLiteral("Ciselniky_Stredisko"), false } },
    { QStringLiteral("Company"),
    { QStringLiteral("Adresar_Firma"), false } },
    { QStringLiteral("Connection"),
    { QStringLiteral("Adresar_Spojeni"), false } },
    { QStringLiteral("IssuedInvoice"),
    { QStringLiteral("Fakturace_FakturaVydana"), false } },
    { QStringLiteral("IssuedOrder"),
    { QStringLiteral("Objednavky_ObjednavkaVydana"), false } },
    { QStringLiteral("JobOrder"),
    { QStringLiteral("Ciselniky_Zakazka"), false } },
    { QStringLiteral("Operation"),
    { QStringLiteral("Ciselniky_Cinnost"), false } },
    { QStringLiteral("Person"),
    { QStringLiteral("Adresar_Osoba"), false } },
    { QStringLiteral("PrepaymentInvoice"),
    { QStringLiteral("Fakturace_ZalohovaFakturaPrijata"), false } },
    { QStringLiteral("PrepaymentIssuedInvoice"),
    { QStringLiteral("Fakturace_ZalohovaFakturaVydana"), false } },
    { QStringLiteral("ReceivedInvoice"),
    { QStringLiteral("Fakturace_FakturaPrijata"), false } },
    { QStringLiteral("ReceivedOrder"),
    { QStringLiteral("Objednavky_ObjednavkaPrijata"), false } },
    { QStringLiteral("Staff"),
    { QStringLiteral("Personalistika_Zamestnanec"), false } },
    { QStringLiteral("TypeOfActivity"),
    { QStringLiteral("Adresar_TypAktivity"), false } }
};

#endif // TABLES_H

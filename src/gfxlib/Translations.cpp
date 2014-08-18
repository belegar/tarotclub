
#include <QObject>
#include "Translations.h"

QString ContractToString(Contract c)
{
    QString labels[] =
    {
        QObject::tr("Pass"),
        QObject::tr("Take"),
        QObject::tr("Guard"),
        QObject::tr("Guard without"),
        QObject::tr("Guard against")
    };

    return labels[c.Value()];
}




QString PlaceToString(Place p)
{
    QString labels[] =
    {
        QObject::tr("South"),
        QObject::tr("East"),
        QObject::tr("North"),
        QObject::tr("West"),
        QObject::tr("Fifth")
    };
    return labels[p.Value()];
}

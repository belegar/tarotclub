/****************************************************************************
** Meta object code from reading C++ file 'Bot.h'
**
** Created: Tue Jan 1 21:37:56 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "base/Bot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Bot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_Bot[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      10,    5,    4,    4, 0x0a,
      27,    4,    4,    4, 0x0a,
      51,   49,    4,    4, 0x0a,
      81,   79,    4,    4, 0x0a,
     109,  107,    4,    4, 0x0a,
     143,    4,    4,    4, 0x0a,
     156,    4,    4,    4, 0x0a,
     175,    4,    4,    4, 0x0a,
     198,  194,    4,    4, 0x0a,
     229,    4,    4,    4, 0x0a,
     248,  245,    4,    4, 0x0a,
     270,    4,    4,    4, 0x0a,
     285,    4,    4,    4, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Bot[] = {
    "Bot\0\0text\0message(QString)\0"
    "slotReceptionCartes()\0p\0"
    "slotAfficheSelection(Place)\0c\0"
    "slotChoixEnchere(Contrat)\0,\0"
    "slotAfficheEnchere(Place,Contrat)\0"
    "slotRedist()\0slotAfficheChien()\0"
    "slotPrepareChien()\0p,c\0"
    "slotDepartDonne(Place,Contrat)\0"
    "slotJoueCarte()\0id\0slotAfficheCarte(int)\0"
    "slotFinDonne()\0slotWaitPli()\0"
};

const QMetaObject Bot::staticMetaObject = {
    { &Client::staticMetaObject, qt_meta_stringdata_Bot,
      qt_meta_data_Bot, 0 }
};

const QMetaObject *Bot::metaObject() const
{
    return &staticMetaObject;
}

void *Bot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Bot))
	return static_cast<void*>(const_cast< Bot*>(this));
    return Client::qt_metacast(_clname);
}

int Bot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Client::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: message((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: slotReceptionCartes(); break;
        case 2: slotAfficheSelection((*reinterpret_cast< Place(*)>(_a[1]))); break;
        case 3: slotChoixEnchere((*reinterpret_cast< Contrat(*)>(_a[1]))); break;
        case 4: slotAfficheEnchere((*reinterpret_cast< Place(*)>(_a[1])),(*reinterpret_cast< Contrat(*)>(_a[2]))); break;
        case 5: slotRedist(); break;
        case 6: slotAfficheChien(); break;
        case 7: slotPrepareChien(); break;
        case 8: slotDepartDonne((*reinterpret_cast< Place(*)>(_a[1])),(*reinterpret_cast< Contrat(*)>(_a[2]))); break;
        case 9: slotJoueCarte(); break;
        case 10: slotAfficheCarte((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: slotFinDonne(); break;
        case 12: slotWaitPli(); break;
        }
        _id -= 13;
    }
    return _id;
}

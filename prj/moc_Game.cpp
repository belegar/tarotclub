/****************************************************************************
** Meta object code from reading C++ file 'Game.h'
**
** Created: Mon Oct 5 01:10:19 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/Game.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Game.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Game[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
       6,    5,    5,    5, 0x0a,
      25,    5,    5,    5, 0x0a,
      41,   39,    5,    5, 0x0a,
      65,    5,    5,    5, 0x0a,
      82,   39,    5,    5, 0x0a,
     107,    5,    5,    5, 0x0a,
     131,  129,    5,    5, 0x0a,
     159,   39,    5,    5, 0x0a,
     189,  185,    5,    5, 0x0a,
     223,    5,    5,    5, 0x0a,
     242,    5,    5,    5, 0x0a,
     255,    5,    5,    5, 0x0a,
     274,  185,    5,    5, 0x0a,
     305,    5,    5,    5, 0x0a,
     324,  321,    5,    5, 0x0a,
     346,    5,    5,    5, 0x0a,
     361,    5,    5,    5, 0x0a,
     375,    5,    5,    5, 0x0a,
     390,    5,    5,    5, 0x0a,
     405,    5,    5,    5, 0x0a,
     420,    5,    5,    5, 0x0a,
     435,    5,    5,    5, 0x0a,
     450,    5,    5,    5, 0x0a,
     474,  469,    5,    5, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Game[] = {
    "Game\0\0slotNewLocalGame()\0showOptions()\0"
    "c\0slotClickCard(GfxCard*)\0slotClickTapis()\0"
    "slotMoveCursor(GfxCard*)\0slotReceptionCartes()\0"
    "p\0slotAfficheSelection(Place)\0"
    "slotChoixEnchere(Contrat)\0p,c\0"
    "slotAfficheEnchere(Place,Contrat)\0"
    "slotAfficheChien()\0slotRedist()\0"
    "slotPrepareChien()\0slotDepartDonne(Place,Contrat)\0"
    "slotJoueCarte()\0id\0slotAfficheCarte(int)\0"
    "slotFinDonne()\0slotWaitPli()\0"
    "slotBoutton1()\0slotBoutton2()\0"
    "slotBoutton3()\0slotBoutton4()\0"
    "slotBoutton5()\0slotAccepteChien()\0"
    "cont\0setEnchere(Contrat)\0"
};

const QMetaObject Game::staticMetaObject = {
    { &MainWindow::staticMetaObject, qt_meta_stringdata_Game,
      qt_meta_data_Game, 0 }
};

const QMetaObject *Game::metaObject() const
{
    return &staticMetaObject;
}

void *Game::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Game))
        return static_cast<void*>(const_cast< Game*>(this));
    return MainWindow::qt_metacast(_clname);
}

int Game::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotNewLocalGame(); break;
        case 1: showOptions(); break;
        case 2: slotClickCard((*reinterpret_cast< GfxCard*(*)>(_a[1]))); break;
        case 3: slotClickTapis(); break;
        case 4: slotMoveCursor((*reinterpret_cast< GfxCard*(*)>(_a[1]))); break;
        case 5: slotReceptionCartes(); break;
        case 6: slotAfficheSelection((*reinterpret_cast< Place(*)>(_a[1]))); break;
        case 7: slotChoixEnchere((*reinterpret_cast< Contrat(*)>(_a[1]))); break;
        case 8: slotAfficheEnchere((*reinterpret_cast< Place(*)>(_a[1])),(*reinterpret_cast< Contrat(*)>(_a[2]))); break;
        case 9: slotAfficheChien(); break;
        case 10: slotRedist(); break;
        case 11: slotPrepareChien(); break;
        case 12: slotDepartDonne((*reinterpret_cast< Place(*)>(_a[1])),(*reinterpret_cast< Contrat(*)>(_a[2]))); break;
        case 13: slotJoueCarte(); break;
        case 14: slotAfficheCarte((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: slotFinDonne(); break;
        case 16: slotWaitPli(); break;
        case 17: slotBoutton1(); break;
        case 18: slotBoutton2(); break;
        case 19: slotBoutton3(); break;
        case 20: slotBoutton4(); break;
        case 21: slotBoutton5(); break;
        case 22: slotAccepteChien(); break;
        case 23: setEnchere((*reinterpret_cast< Contrat(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 24;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'Client.h'
**
** Created: Mon Oct 5 01:10:21 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/Client.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Client[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      16,    8,    7,    7, 0x05,
      51,   37,    7,    7, 0x05,
      89,    7,    7,    7, 0x05,
     111,    7,    7,    7, 0x05,
     139,    7,    7,    7, 0x05,
     167,  165,    7,    7, 0x05,
     201,  165,    7,    7, 0x05,
     232,    7,    7,    7, 0x05,
     251,    7,    7,    7, 0x05,
     270,    7,    7,    7, 0x05,
     283,    7,    7,    7, 0x05,
     299,    7,    7,    7, 0x05,
     321,    7,    7,    7, 0x05,
     336,    7,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
     350,    7,    7,    7, 0x0a,
     367,    7,    7,    7, 0x0a,
     385,    7,    7,    7, 0x0a,
     403,    7,    7,    7, 0x0a,
     423,  418,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Client[] = {
    "Client\0\0message\0sgnlMessage(QString)\0"
    "nombre,idents\0sgnlListeDesJoueurs(quint8,Identity*)\0"
    "sgnlReceptionCartes()\0sgnlAfficheSelection(Place)\0"
    "sgnlChoixEnchere(Contrat)\0,\0"
    "sgnlAfficheEnchere(Place,Contrat)\0"
    "sgnlDepartDonne(Place,Contrat)\0"
    "sgnlAfficheChien()\0sgnlPrepareChien()\0"
    "sgnlRedist()\0sgnlJoueCarte()\0"
    "sgnlAfficheCarte(int)\0sgnlFinDonne()\0"
    "sgnlWaitPli()\0socketReadData()\0"
    "socketConnected()\0socketHostFound()\0"
    "socketClosed()\0code\0"
    "socketError(QAbstractSocket::SocketError)\0"
};

const QMetaObject Client::staticMetaObject = {
    { &Player::staticMetaObject, qt_meta_stringdata_Client,
      qt_meta_data_Client, 0 }
};

const QMetaObject *Client::metaObject() const
{
    return &staticMetaObject;
}

void *Client::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Client))
        return static_cast<void*>(const_cast< Client*>(this));
    return Player::qt_metacast(_clname);
}

int Client::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Player::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sgnlMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: sgnlListeDesJoueurs((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< Identity*(*)>(_a[2]))); break;
        case 2: sgnlReceptionCartes(); break;
        case 3: sgnlAfficheSelection((*reinterpret_cast< Place(*)>(_a[1]))); break;
        case 4: sgnlChoixEnchere((*reinterpret_cast< Contrat(*)>(_a[1]))); break;
        case 5: sgnlAfficheEnchere((*reinterpret_cast< Place(*)>(_a[1])),(*reinterpret_cast< Contrat(*)>(_a[2]))); break;
        case 6: sgnlDepartDonne((*reinterpret_cast< Place(*)>(_a[1])),(*reinterpret_cast< Contrat(*)>(_a[2]))); break;
        case 7: sgnlAfficheChien(); break;
        case 8: sgnlPrepareChien(); break;
        case 9: sgnlRedist(); break;
        case 10: sgnlJoueCarte(); break;
        case 11: sgnlAfficheCarte((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: sgnlFinDonne(); break;
        case 13: sgnlWaitPli(); break;
        case 14: socketReadData(); break;
        case 15: socketConnected(); break;
        case 16: socketHostFound(); break;
        case 17: socketClosed(); break;
        case 18: socketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void Client::sgnlMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Client::sgnlListeDesJoueurs(quint8 _t1, Identity * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Client::sgnlReceptionCartes()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Client::sgnlAfficheSelection(Place _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Client::sgnlChoixEnchere(Contrat _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Client::sgnlAfficheEnchere(Place _t1, Contrat _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Client::sgnlDepartDonne(Place _t1, Contrat _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Client::sgnlAfficheChien()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void Client::sgnlPrepareChien()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void Client::sgnlRedist()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void Client::sgnlJoueCarte()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void Client::sgnlAfficheCarte(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void Client::sgnlFinDonne()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void Client::sgnlWaitPli()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}
QT_END_MOC_NAMESPACE

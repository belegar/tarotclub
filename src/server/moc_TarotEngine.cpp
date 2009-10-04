/****************************************************************************
** Meta object code from reading C++ file 'TarotEngine.h'
**
** Created: Tue Jan 1 21:40:36 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "TarotEngine.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TarotEngine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_TarotEngine[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,
      29,   12,   12,   12, 0x0a,
      45,   12,   12,   12, 0x0a,
      61,   12,   12,   12, 0x0a,
      77,   12,   12,   12, 0x0a,
      93,   12,   12,   12, 0x0a,
     105,   12,   12,   12, 0x0a,
     117,   12,   12,   12, 0x0a,
     129,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_TarotEngine[] = {
    "TarotEngine\0\0newConnection()\0"
    "clientClosed1()\0clientClosed2()\0"
    "clientClosed3()\0clientClosed4()\0"
    "readData1()\0readData2()\0readData3()\0"
    "readData4()\0"
};

const QMetaObject TarotEngine::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_TarotEngine,
      qt_meta_data_TarotEngine, 0 }
};

const QMetaObject *TarotEngine::metaObject() const
{
    return &staticMetaObject;
}

void *TarotEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TarotEngine))
	return static_cast<void*>(const_cast< TarotEngine*>(this));
    return QThread::qt_metacast(_clname);
}

int TarotEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newConnection(); break;
        case 1: clientClosed1(); break;
        case 2: clientClosed2(); break;
        case 3: clientClosed3(); break;
        case 4: clientClosed4(); break;
        case 5: readData1(); break;
        case 6: readData2(); break;
        case 7: readData3(); break;
        case 8: readData4(); break;
        }
        _id -= 9;
    }
    return _id;
}

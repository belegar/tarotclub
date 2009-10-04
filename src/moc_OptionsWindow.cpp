/****************************************************************************
** Meta object code from reading C++ file 'OptionsWindow.h'
**
** Created: Tue Jan 1 21:37:46 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "windows/OptionsWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OptionsWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_OptionsWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x0a,
      27,   14,   14,   14, 0x0a,
      43,   14,   14,   14, 0x0a,
      63,   14,   14,   14, 0x0a,
      83,   14,   14,   14, 0x0a,
      99,   14,   14,   14, 0x0a,
     115,   14,   14,   14, 0x0a,
     132,   14,   14,   14, 0x0a,
     150,   14,   14,   14, 0x0a,
     178,  172,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_OptionsWindow[] = {
    "OptionsWindow\0\0slotBtnOk()\0slotBtnDefaut()\0"
    "slider1Changed(int)\0slider2Changed(int)\0"
    "slotBtnPixSud()\0slotBtnPixEst()\0"
    "slotBtnPixNord()\0slotBtnPixOuest()\0"
    "slotBtnPixNordOuest()\0value\0"
    "slotScrollBarTapis(int)\0"
};

const QMetaObject OptionsWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_OptionsWindow,
      qt_meta_data_OptionsWindow, 0 }
};

const QMetaObject *OptionsWindow::metaObject() const
{
    return &staticMetaObject;
}

void *OptionsWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWindow))
	return static_cast<void*>(const_cast< OptionsWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int OptionsWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotBtnOk(); break;
        case 1: slotBtnDefaut(); break;
        case 2: slider1Changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: slider2Changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: slotBtnPixSud(); break;
        case 5: slotBtnPixEst(); break;
        case 6: slotBtnPixNord(); break;
        case 7: slotBtnPixOuest(); break;
        case 8: slotBtnPixNordOuest(); break;
        case 9: slotScrollBarTapis((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 10;
    }
    return _id;
}

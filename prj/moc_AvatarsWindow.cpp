/****************************************************************************
** Meta object code from reading C++ file 'AvatarsWindow.h'
**
** Created: Mon Oct 5 01:10:13 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/AvatarsWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AvatarsWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AvatarsWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      29,   15,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_AvatarsWindow[] = {
    "AvatarsWindow\0\0item,previous\0"
    "slotShowImage(QListWidgetItem*,QListWidgetItem*)\0"
};

const QMetaObject AvatarsWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_AvatarsWindow,
      qt_meta_data_AvatarsWindow, 0 }
};

const QMetaObject *AvatarsWindow::metaObject() const
{
    return &staticMetaObject;
}

void *AvatarsWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AvatarsWindow))
        return static_cast<void*>(const_cast< AvatarsWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int AvatarsWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotShowImage((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'ScoresDock.h'
**
** Created: Tue Jan 1 21:37:50 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "windows/ScoresDock.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ScoresDock.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ScoresDock[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ScoresDock[] = {
    "ScoresDock\0\0sgnlClose()\0"
};

const QMetaObject ScoresDock::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_ScoresDock,
      qt_meta_data_ScoresDock, 0 }
};

const QMetaObject *ScoresDock::metaObject() const
{
    return &staticMetaObject;
}

void *ScoresDock::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ScoresDock))
	return static_cast<void*>(const_cast< ScoresDock*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int ScoresDock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sgnlClose(); break;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void ScoresDock::sgnlClose()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

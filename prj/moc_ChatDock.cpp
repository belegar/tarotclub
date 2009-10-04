/****************************************************************************
** Meta object code from reading C++ file 'ChatDock.h'
**
** Created: Mon Oct 5 01:10:16 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/ChatDock.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChatDock.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ChatDock[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      35,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,    9,    9,    9, 0x0a,
      75,   67,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ChatDock[] = {
    "ChatDock\0\0sgnlEmitMessage(QString)\0"
    "sgnlClose()\0slotReturnPressed()\0message\0"
    "message(QString)\0"
};

const QMetaObject ChatDock::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_ChatDock,
      qt_meta_data_ChatDock, 0 }
};

const QMetaObject *ChatDock::metaObject() const
{
    return &staticMetaObject;
}

void *ChatDock::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChatDock))
        return static_cast<void*>(const_cast< ChatDock*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int ChatDock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sgnlEmitMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: sgnlClose(); break;
        case 2: slotReturnPressed(); break;
        case 3: message((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ChatDock::sgnlEmitMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ChatDock::sgnlClose()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE

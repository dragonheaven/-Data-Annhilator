/****************************************************************************
** Meta object code from reading C++ file 'maindialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "maindialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'maindialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      34,   11,   11,   11, 0x08,
      59,   11,   11,   11, 0x08,
      84,   11,   11,   11, 0x08,
     108,   11,   11,   11, 0x08,
     128,   11,   11,   11, 0x08,
     147,   11,   11,   11, 0x08,
     165,   11,   11,   11, 0x08,
     188,   11,   11,   11, 0x08,
     212,   11,   11,   11, 0x08,
     237,   11,   11,   11, 0x08,
     264,   11,   11,   11, 0x08,
     283,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainDialog[] = {
    "MainDialog\0\0on_rtl_wipe_clicked()\0"
    "on_rtl_history_clicked()\0"
    "on_rtl_credits_clicked()\0"
    "on_rtl_reboot_clicked()\0on_doWipe_clicked()\0"
    "on_updDB_clicked()\0on_quit_clicked()\0"
    "on_viewError_clicked()\0on_saveReport_clicked()\0"
    "on_rescanDisks_clicked()\0"
    "resizeEvent(QResizeEvent*)\0"
    "wmodeChanged(bool)\0listBootDrive(bool)\0"
};

void MainDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainDialog *_t = static_cast<MainDialog *>(_o);
        switch (_id) {
        case 0: _t->on_rtl_wipe_clicked(); break;
        case 1: _t->on_rtl_history_clicked(); break;
        case 2: _t->on_rtl_credits_clicked(); break;
        case 3: _t->on_rtl_reboot_clicked(); break;
        case 4: _t->on_doWipe_clicked(); break;
        case 5: _t->on_updDB_clicked(); break;
        case 6: _t->on_quit_clicked(); break;
        case 7: _t->on_viewError_clicked(); break;
        case 8: _t->on_saveReport_clicked(); break;
        case 9: _t->on_rescanDisks_clicked(); break;
        case 10: _t->resizeEvent((*reinterpret_cast< QResizeEvent*(*)>(_a[1]))); break;
        case 11: _t->wmodeChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->listBootDrive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MainDialog,
      qt_meta_data_MainDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainDialog))
        return static_cast<void*>(const_cast< MainDialog*>(this));
    if (!strcmp(_clname, "Ui::MainDialog"))
        return static_cast< Ui::MainDialog*>(const_cast< MainDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MainDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

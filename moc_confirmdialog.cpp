/****************************************************************************
** Meta object code from reading C++ file 'confirmdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "confirmdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'confirmdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ConfirmDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      33,   11,   11,   11, 0x08,
      53,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ConfirmDlg[] = {
    "ConfirmDlg\0\0on_yesButt_clicked()\0"
    "on_noButt_clicked()\0on_nrPasses_valueChanged()\0"
};

void ConfirmDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ConfirmDlg *_t = static_cast<ConfirmDlg *>(_o);
        switch (_id) {
        case 0: _t->on_yesButt_clicked(); break;
        case 1: _t->on_noButt_clicked(); break;
        case 2: _t->on_nrPasses_valueChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ConfirmDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ConfirmDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ConfirmDlg,
      qt_meta_data_ConfirmDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ConfirmDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ConfirmDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ConfirmDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ConfirmDlg))
        return static_cast<void*>(const_cast< ConfirmDlg*>(this));
    if (!strcmp(_clname, "Ui::ConfirmDlg"))
        return static_cast< Ui::ConfirmDlg*>(const_cast< ConfirmDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int ConfirmDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

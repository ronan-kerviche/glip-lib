/****************************************************************************
** Meta object code from reading C++ file 'WindowRendering.hpp'
**
** Created: Sat Sep 17 08:30:50 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/WindowRendering.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WindowRendering.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WindowRenderer[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WindowRenderer[] = {
    "WindowRenderer\0\0draw()\0"
};

const QMetaObject WindowRenderer::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_WindowRenderer,
      qt_meta_data_WindowRenderer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WindowRenderer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WindowRenderer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WindowRenderer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WindowRenderer))
        return static_cast<void*>(const_cast< WindowRenderer*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int WindowRenderer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: draw(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

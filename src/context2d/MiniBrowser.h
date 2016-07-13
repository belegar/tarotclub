#ifndef MINI_BROWSER_H
#define MINI_BROWSER_H

#include <qobject.h>
#include <qlist.h>
#include <qhash.h>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMap>
#include <QJSEngine>
#include <QJSValue>
#include <QWidget>
#include <QQuickWidget>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QMouseEvent;
class QKeyEvent;
QT_END_NAMESPACE

#include "JSEngine.h"


namespace test
{
/*
class BrowserEnvironment : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal innerWidth READ getInnerWidth)
    Q_PROPERTY(qreal innerHeight READ getInnerHeight)

public:
    BrowserEnvironment(QJSEngine *engine, QObject *parent = 0);
    ~BrowserEnvironment();

    int getInnerWidth() { return width; }
    int getInnerHeight() { return height; }

    QJSValue evaluate(const QString &code,
                          const QString &fileName = QString());

    QJSValue toWrapper(QObject *object);

    void handleEvent(QContext2DCanvas *canvas, QMouseEvent *e);
    void handleEvent(QContext2DCanvas *canvas, QKeyEvent *e);

    QJSEngine *engine() const;
    bool hasIntervalTimers() const;
    void triggerTimers();

    void reset();

//! [1]
public slots:

    int setInterval(const QJSValue &expression, int delay);
    void clearInterval(int timerId);

    int setTimeout(const QJSValue &expression, int delay);
    void clearTimeout(int timerId);

    QJSValue getComputedStyle();
//! [1]

//! [2]
signals:
    void scriptError(const QJSValue &error);
//! [2]

protected:
    void timerEvent(QTimerEvent *event);

private:
    QJSValue eventHandler(QContext2DCanvas *canvas,
                              const QString &type, QJSValue *who);
    QJSValue newFakeDomEvent(const QString &type,
                                 const QJSValue &target);
    void maybeEmitScriptError();

    QJSEngine *m_engine;
    QJSValue mGlobalObject;
    QJSValue m_document;
    QJSValue fakeEnv;
    QHash<int, QJSValue> m_intervalHash;
    QHash<int, QJSValue> m_timeoutHash;

    Document *mDocument;

    int width;
    int height;
};

*/


class Window : public QObject
{
    Q_OBJECT


public:
    Window(QObject *parent = 0);
    ~Window();

    bool hasIntervalTimers() const;
    void triggerTimers();

public slots:

    int setInterval(const QJSValue &expression, int delay);
    void clearInterval(int timerId);

    int setTimeout(const QJSValue &expression, int delay);
    void clearTimeout(int timerId);

protected:
    void timerEvent(QTimerEvent *event);

private:
    QHash<int, QJSValue> m_intervalHash;
    QHash<int, QJSValue> m_timeoutHash;
};


class Document : public QObject
{
    Q_OBJECT

public:
    Document();
    ~Document();

    QJSValue FindEventCallback(const QString &name)
    {
        if (mEventList.contains(name))
        {
            return mEventList.value(name);
        }
        else
        {
            return QJSValue();
        }
    }

public slots:
//    QJSValue getElementById(const QString &id) const;
//    QJSValue getElementsByTagName(const QString &name) const;
//    QJSValue createElement(const QString &name) const;


    // EventTarget
//    void addEventListener(const QString &type, const QJSValue &listener,
//                          bool useCapture);

    void Print(int step);

private:

    QMap<QString, QJSValue> mEventList;
};


class MiniBrowser : public QQuickWidget
{
    Q_OBJECT

public:
    MiniBrowser(QWidget *parent = 0);
    ~MiniBrowser();

    void reset();

    void moveTo(qreal x, qreal y);
    void lineTo(qreal x, qreal y);


public slots:
    // EventTarget
    void addEventListener(const QString &type, const QJSValue &listener,
                          bool useCapture);

protected:

//    virtual void paintEvent(QPaintEvent *e);
//    virtual void mouseMoveEvent(QMouseEvent *e);
//    virtual void mousePressEvent(QMouseEvent *e);
//    virtual void mouseReleaseEvent(QMouseEvent *e);
//    virtual void keyPressEvent(QKeyEvent *e);
//    virtual void keyReleaseEvent(QKeyEvent *e);
//    virtual void resizeEvent(QResizeEvent *e);


private slots:
    void contentsChanged(const QImage &image);
//! [0]

private:

};


/*
class MiniBrowser : public QWidget
{
public:
    MiniBrowser(QWidget *parent)
        : QWidget(parent)
    {
        setMouseTracking(true);
    }

    void Initialize()
    {
        mEngine.Initialize();
      //  mEngine.EvaluateFile("U:/tarotclub/assets/canvasjs/easeljs-0.8.2.min.js");
    }

private:
    JSEngine mEngine;
};
*/


} // namespace test

#endif // MINI_BROWSER_H

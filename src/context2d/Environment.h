/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <qobject.h>
#include <qlist.h>
#include <qhash.h>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QKeyEvent>
//#include <QJSEngine>
//#include <QScriptable>

#include <QJSValue>
#include <QJSEngine>
#include <QMap>

class QContext2DCanvas;
class Document;

//! [0]
class Environment : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal innerWidth READ getInnerWidth)
    Q_PROPERTY(qreal innerHeight READ getInnerHeight)

public:
    Environment(QObject *parent = 0);
    ~Environment();

    int getInnerWidth() { return width; }
    int getInnerHeight() { return height; }

    void addCanvas(QContext2DCanvas *canvas);
    QContext2DCanvas *canvasByName(const QString &name) const;
    QList<QContext2DCanvas*> canvases() const;

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
    QList<QContext2DCanvas*> m_canvases;
    QHash<int, QJSValue> m_intervalHash;
    QHash<int, QJSValue> m_timeoutHash;

    Document *mDocument;

    int width;
    int height;
};

//! [3]
class Document : public QObject
{
    Q_OBJECT
public:
    Document(Environment *env);
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
    QJSValue getElementById(const QString &id) const;
    QJSValue getElementsByTagName(const QString &name) const;
    QJSValue createElement(const QString &name) const;

    // EventTarget
    void addEventListener(const QString &type, const QJSValue &listener,
                          bool useCapture);

    void Print(int step);

private:

    QMap<QString, QJSValue> mEventList;
};
//! [3]
/*
class CanvasGradientPrototype : public QObject
{
    Q_OBJECT
protected:
    CanvasGradientPrototype(QObject *parent = 0);
public:
    static void setup(QJSEngine *engine);

public slots:
    void addColorStop(qreal offset, const QString &color);
};
*/

#endif

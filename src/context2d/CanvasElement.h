#ifndef CANVAS_ELEMENT_H
#define CANVAS_ELEMENT_H

#include "Context2d.h"
#include "IEnvironment.h"

class CanvasElement : public QObject
{
    Q_OBJECT

    // Canvas properties made available in JavaScript world
    Q_PROPERTY( int width READ getWidth WRITE setWidth )
    Q_PROPERTY( int height READ getHeight WRITE setHeight )

public:
    CanvasElement(IEnvironment *env);
    ~CanvasElement();

    Context2D *context() { return mContext; }

    void reset();
    void setSize(int width, int height);

    int getWidth();
    int getHeight();

    void setHeight(int height);
    void setWidth(int width);

public slots:
    QJSValue getContext(const QString &str);
    void addEventListener(const QString &type, const QJSValue &listener, bool useCapture);

private:
    IEnvironment *mEnv;
    Context2D   *mContext;

};
#endif // CANVAS_ELEMENT_H

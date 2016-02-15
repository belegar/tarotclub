#include "CanvasElement.h"
#include "Environment.h"

CanvasElement::CanvasElement(IEnvironment *env)
    : mEnv(env)
    , mContext(new Context2D())
{

}

CanvasElement::~CanvasElement()
{

}

void CanvasElement::reset()
{
    mContext->reset();
}

void CanvasElement::setSize(int width, int height)
{
    mContext->setSize(width, height);
}

QJSValue CanvasElement::getContext(const QString &str)
{
    if (str != "2d")
    {
        return QJSValue();
    }
    return mEnv->toWrapper(mContext);
}

int CanvasElement::getWidth()
{
    int width = 0;
    if (mContext != NULL)
    {
       width = mContext->getWidth();
    }

    return width;
}

int CanvasElement::getHeight()
{
    int height = 0;
    if (mContext != NULL)
    {
       height = mContext->getHeight();
    }

    return height;
}

void CanvasElement::setHeight(int height)
{
    (void) height;
}

void CanvasElement::setWidth(int width)
{
    (void) width;
}

void CanvasElement::addEventListener(const QString &type, const QJSValue &listener,
                                        bool useCapture)
{
    Q_UNUSED(useCapture);
    if (listener.isCallable()) {
        QJSValue self = mEnv->toWrapper(this);
        self.setProperty("on" + type, listener);
    }
}

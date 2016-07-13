#ifndef IENVIRONMENT_H
#define IENVIRONMENT_H

class QJSValue;
class QObject;


class IEnvironment
{
public:
    virtual ~IEnvironment() {}

    virtual QJSValue toWrapper(QObject *object) = 0;

};

#endif // IENVIRONMENT_H


#ifndef USERMETHOD_H
#define USERMETHOD_H
# include <OPTIMUS/optimizer.h>

/**
 * @brief The UserMethod class is the skeleton class for the used defined optimization methods. The user should extend this class.
 */
class UserMethod :public Optimizer
{
private:
    int paramValue;
public:
    /**
     * @brief UserMethod
     */
    UserMethod();
    /**
     * @brief init
     */
    virtual void init();
    /**
     * @brief step
     */
    virtual void step();
    /**
     * @brief terminated
     * @return
     */
    virtual bool terminated();
    /**
     * @brief done
     */
    virtual void done();
    /**
     * @brief ~UserMethod
     */
    virtual ~UserMethod();
};

#endif // USERMETHOD_H

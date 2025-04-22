#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
class Singleton
{
    friend T;
    Singleton() = default;
    virtual ~Singleton() = default;

public:
    static auto Instance()
        -> T &
    {
        static T t;
        return t;
    }
};

#endif
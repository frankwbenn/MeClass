#pragma once

#include <memory>
#include <vector>
class IManaged
{
public:
    virtual ~IManaged() = default;
};

class ObjectPool
{
private:
    std::vector<std::unique_ptr<IManaged>> Objects;
    ObjectPool() = default;
public:
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

    static ObjectPool& Get()
    {
        static ObjectPool objpool;
        return objpool;
    }

    template<typename T, typename... Args>
    void Spawn(Args&&... args)
    {
        Objects.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }
};

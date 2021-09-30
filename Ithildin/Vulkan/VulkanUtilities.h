#pragma once
#include "../Core/Core.h"
#include <vector>

namespace Raytracing
{
    template <typename TValue>
    inline void GetEnumerateVector(VkResult(enumerate) (uint32_t*, TValue*), std::vector<TValue>& vector, const char* operation)
    {
        uint32_t count = 0;
        CheckResult(enumerate(&count, nullptr), "Query Count");
        vector.resize(count);
        CheckResult(enumerate(&count, vector.data()), operation);
    }

    template <typename TValue>
    inline std::vector<TValue> GetEnumerateVector(VkResult(enumerate)(uint32_t*, TValue*), const char* operation)
    {
        std::vector<TValue> result;
        GetEnumerateVector(enumerate, result, operation);

        return result;
    }

    template <typename THandle, typename TValue>
    inline void GetEnumerateVector(THandle handle, VkResult(enumerate) (THandle, uint32_t*, TValue*), std::vector<TValue>& vector, const char* operation)
    {
        uint32_t count = 0;
        CheckResult(enumerate(handle, &count, nullptr), "Query Count");
        vector.resize(count);
        CheckResult(enumerate(handle, &count, vector.data()), operation);
    }

    template<typename THandle, typename TValue>
    inline std::vector<TValue> GetEnumerateVector(THandle handle, void(enumerate)(THandle, uint32_t*, TValue*), const char* operation)
    {
        std::vector<TValue> result;
        GetEnumerateVector(handle, enumerate, result, operation);

        return result;
    }

    template<typename THandle, typename TValue>
    inline std::vector<TValue> GetEnumerateVector(THandle handle, VkResult(enumerate)(THandle, uint32_t*, TValue*), const char* operation)
    {
        std::vector<TValue> result;
        GetEnumerateVector(handle, enumerate, result, operation);

        return result;
    }

    template <typename THandle1, typename THandle2, typename TValue>
    inline std::vector<TValue> GetEnumerateVector(THandle1 handle1, THandle2 handle2, VkResult(enumerate) (THandle1, THandle2, uint32_t*, TValue*), const char* operation)
    {
        std::vector<TValue> result;
        GetEnumerateVector(handle1, handle2, enumerate, result, operation);

        return result;
    }
}
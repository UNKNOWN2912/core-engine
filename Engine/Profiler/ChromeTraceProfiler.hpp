#pragma once
#include <cstdint>
#include <mutex>
#include <string_view>
#include <string>

class ChromeFunctionTrace;

class ChromeTraceProfiler
{
    public:
        ChromeTraceProfiler(std::string_view filename);
        ChromeTraceProfiler(){}
        ~ChromeTraceProfiler();

        ChromeFunctionTrace ProfileFunction(std::string_view name, uint32_t pid, uint32_t tid);

        
        void WriteBeginEntry(std::string_view name, uint32_t pid, uint32_t tid, float currentTime);
        void WriteEndEntry(std::string_view name, uint32_t pid, uint32_t tid, float currentTime);
        
        void WriteBeginEntry(std::string_view name, uint32_t pid, uint32_t tid);
        void WriteEndEntry(std::string_view name, uint32_t pid, uint32_t tid);
        
    private:
        void SetWriteFile(std::string_view filename);
        std::mutex mWriteMutex;
        FILE* mFp = nullptr;
};

class ChromeFunctionTrace
{
    public:
        ChromeFunctionTrace(std::string_view name, uint32_t pid, uint32_t tid, ChromeTraceProfiler* profiler);
        ~ChromeFunctionTrace();
    private:
        std::string mName;
        uint32_t mPid;
        uint32_t mTid;
        ChromeTraceProfiler* mProfiler;
};
#pragma once
#include <cstdint>
#include <mutex>
#include <queue>
#include <string_view>
#include <string>
#include <thread>

class ChromeFunctionTrace;

struct ChromeTraceEntry
{
    std::string name;
    uint32_t pid;
    uint32_t tid;
    uint32_t timeInMicrosecond;
    bool begin = false;

    ChromeTraceEntry(std::string_view name, uint32_t pid, uint32_t tid, uint32_t timeInMicrosecond, bool begin)
        : name(name), pid(pid), tid(tid), timeInMicrosecond(timeInMicrosecond), begin(begin)
    {}
};

class ChromeTraceProfiler
{
    public:
        ChromeTraceProfiler(std::string_view filename);
        ~ChromeTraceProfiler();

        ChromeFunctionTrace ProfileFunction(std::string_view name, uint32_t pid, uint32_t tid);
        
        void WriteBeginEntry(std::string_view name, uint32_t pid, uint32_t tid, float currentTime);
        void WriteEndEntry(std::string_view name, uint32_t pid, uint32_t tid, float currentTime);
        
        void WriteBeginEntry(std::string_view name, uint32_t pid, uint32_t tid);
        void WriteEndEntry(std::string_view name, uint32_t pid, uint32_t tid);

        void Reader();

        void EnableTracing(bool enable);
        
    private:
        bool mEnableWrite = false;
        bool mRunning = true;
        std::queue<ChromeTraceEntry> mTraceQueue;
        std::mutex mWriteMutex;

        std::thread mReaderThread;

        void SetWriteFile(std::string_view filename);
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

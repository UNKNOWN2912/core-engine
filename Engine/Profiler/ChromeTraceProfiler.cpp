#include "ChromeTraceProfiler.hpp"
#include "Core/Timer.hpp"
#include <cstdio>
#include <format>
#include <thread>


#undef CHROME_TRACE_FUNCTION

ChromeTraceProfiler::ChromeTraceProfiler(std::string_view filename)
{
    SetWriteFile(filename);
    mReaderThread = std::thread(&ChromeTraceProfiler::Reader, this);
}

ChromeTraceProfiler::~ChromeTraceProfiler()
{
    LOG("Destructor");

    {
        std::lock_guard<std::mutex> lock(mWriteMutex);
        mRunning = false;
    }
    

    mReaderThread.join();

    

    fseek(mFp, -2, SEEK_END);
    fwrite("\n]", 1, 2, mFp);

    fclose(mFp);
}

ChromeFunctionTrace ChromeTraceProfiler::ProfileFunction(std::string_view name, uint32_t pid, uint32_t tid) 
{
    return ChromeFunctionTrace(name, pid, tid, this);     
}

void ChromeTraceProfiler::SetWriteFile(std::string_view filename) 
{
    if(mFp != nullptr)
        fclose(mFp);
        
    mFp = fopen(filename.data(), "w");

    fwrite("[\n", 1, 2, mFp);

    fflush(mFp);

}

void ChromeTraceProfiler::WriteBeginEntry(std::string_view name, uint32_t pid, uint32_t tid, float currentTime) 
{
    if(!mEnableWrite)
        return;
    uint64_t nanoSecond = currentTime * 1e6;
    std::lock_guard<std::mutex> lock(mWriteMutex);
    mTraceQueue.emplace(name ,pid, tid, currentTime, true);
}


void ChromeTraceProfiler::WriteEndEntry(std::string_view name, uint32_t pid, uint32_t tid, float currentTime) 
{
    if(!mEnableWrite)
        return;
    uint64_t nanoSecond = currentTime * 1e6;
    std::lock_guard<std::mutex> lock(mWriteMutex);
    mTraceQueue.emplace(name ,pid, tid, currentTime, false);
}

void ChromeTraceProfiler::WriteBeginEntry(std::string_view name, uint32_t pid, uint32_t tid) 
{
    WriteBeginEntry(name, pid, tid, GetGlobalTimeElapsed());    
}

void ChromeTraceProfiler::WriteEndEntry(std::string_view name, uint32_t pid, uint32_t tid) 
{
    WriteEndEntry(name, pid, tid, GetGlobalTimeElapsed());    
}

void ChromeTraceProfiler::Reader() 
{
    while(mRunning)
    {
        std::lock_guard<std::mutex> lock(mWriteMutex);
        while(!mTraceQueue.empty())
        {
            ChromeTraceEntry entry = mTraceQueue.front();
            mTraceQueue.pop();
            std::string formattedString = std::format("\t{{\"name\": \"{}\", \"cat\": \"PERF\", \"ph\": \"{}\", \"pid\": {}, \"tid\": {}, \"ts\": {}}},\n", entry.name.data(), (entry.begin) ? 'B'  : 'E', entry.pid, entry.tid, entry.timeInMicrosecond);
            fwrite(formattedString.data(), 1, formattedString.size(), mFp);
        }
    }    
}

void ChromeTraceProfiler::EnableTracing(bool enable) 
{
    mEnableWrite = enable;    
}

ChromeFunctionTrace::ChromeFunctionTrace(std::string_view name, uint32_t pid, uint32_t tid, ChromeTraceProfiler* profiler)
    : mName(name), mPid(pid), mTid(tid), mProfiler(profiler)
{ 
    mProfiler->WriteBeginEntry(mName, mPid, mTid); 
}

ChromeFunctionTrace::~ChromeFunctionTrace() 
{
    mProfiler->WriteEndEntry(mName, mPid, mTid); 
}

#include "ChromeTraceProfiler.hpp"
#include "Core/Timer.hpp"
#include <cstdio>
#include <format>


#undef CHROME_TRACE_FUNCTION

ChromeTraceProfiler::ChromeTraceProfiler(std::string_view filename)
{
    SetWriteFile(filename);
}

ChromeTraceProfiler::~ChromeTraceProfiler()
{
    fseek(mFp, -2, SEEK_END);
    fwrite("\n]", 1, 2, mFp);
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
}

void ChromeTraceProfiler::WriteBeginEntry(std::string_view name, uint32_t pid, uint32_t tid, float currentTime) 
{
    uint64_t nanoSecond = currentTime * 1e6;

    std::string formattedString = std::format("\t{{\"name\": \"{}\", \"cat\": \"PERF\", \"ph\": \"B\", \"pid\": {}, \"tid\": {}, \"ts\": {}}},\n", name.data(), pid, tid, nanoSecond);
    std::lock_guard<std::mutex> lock(mWriteMutex);
    fwrite(formattedString.data(), 1, formattedString.size(), mFp);
}


void ChromeTraceProfiler::WriteEndEntry(std::string_view name, uint32_t pid, uint32_t tid, float currentTime) 
{
    uint64_t nanoSecond = currentTime * 1e6;

    std::string formattedString = std::format("\t{{\"name\": \"{}\", \"cat\": \"PERF\", \"ph\": \"E\", \"pid\": {}, \"tid\": {}, \"ts\": {}}},\n", name.data(), pid, tid, nanoSecond);
    
    std::lock_guard<std::mutex> lock(mWriteMutex);
    fwrite(formattedString.data(), 1, formattedString.size(), mFp);
}

void ChromeTraceProfiler::WriteBeginEntry(std::string_view name, uint32_t pid, uint32_t tid) 
{
    WriteBeginEntry(name, pid, tid, GetGlobalTimeElapsed());    
}

void ChromeTraceProfiler::WriteEndEntry(std::string_view name, uint32_t pid, uint32_t tid) 
{
    WriteEndEntry(name, pid, tid, GetGlobalTimeElapsed());    
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

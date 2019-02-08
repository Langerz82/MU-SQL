
#include "ByteBuffer.h"
#include "Logging/Log.h"

void ByteBufferException::PrintPosError() const
{
    char const* traceStr;

#ifdef HAVE_ACE_STACK_TRACE_H
    ACE_Stack_Trace trace;
    traceStr = trace.c_str();
#else
    traceStr = NULL;
#endif

    sLog.outError(
        "Attempted to %s in ByteBuffer (pos: " SIZEFMTD " size: " SIZEFMTD ") "
        "value with size: " SIZEFMTD "%s%s",
        (add ? "put" : "get"), pos, size, esize,
        traceStr ? "\n" : "", traceStr ? traceStr : "");
}

void ByteBuffer::print_storage() const
{
    if (!sLog.HasLogLevelOrHigher(LOG_LVL_DEBUG))   // optimize disabled debug output
        { return; }

    std::ostringstream ss;
    ss <<  "STORAGE_SIZE: " << size() << "\n";

    if (sLog.IsIncludeTime())
        { ss << "         "; }

    for (size_t i = 0; i < size(); ++i)
        { ss << uint32(read<uint8>(i)) << " - "; }

    sLog.outDebug("%s", ss.str().c_str());
}

void ByteBuffer::textlike() const
{
    if (!sLog.HasLogLevelOrHigher(LOG_LVL_DEBUG))   // optimize disabled debug output
        { return; }

    std::ostringstream ss;
    ss <<  "STORAGE_SIZE: " << size() << "\n";

    if (sLog.IsIncludeTime())
        { ss << "         "; }

    for (size_t i = 0; i < size(); ++i)
        { ss << read<uint8>(i); }

    sLog.outDebug("%s", ss.str().c_str());
}

void ByteBuffer::hexlike() const
{
    if (!sLog.HasLogLevelOrHigher(LOG_LVL_DEBUG))   // optimize disabled debug output
        { return; }

    std::ostringstream ss;
    ss <<  "STORAGE_SIZE: " << size() << "\n";

    if (sLog.IsIncludeTime())
        { ss << "         "; }

    size_t j = 1, k = 1;

    for (size_t i = 0; i < size(); ++i)
    {
        if ((i == (j * 8)) && ((i != (k * 16))))
        {
            ss << "| ";
            ++j;
        }
        else if (i == (k * 16))
        {
            ss << "\n";

            if (sLog.IsIncludeTime())
                { ss << "         "; }

            ++k;
            ++j;
        }

        char buf[4];
        snprintf(buf, 4, "%02X", read<uint8>(i));
        ss << buf << " ";
    }

    sLog.outDebug("%s", ss.str().c_str());
}

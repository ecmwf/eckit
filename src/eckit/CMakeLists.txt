### endianess

test_big_endian( _BIG_ENDIAN )

if( _BIG_ENDIAN )
        set( ECKIT_BIG_ENDIAN    1 )
        set( ECKIT_LITTLE_ENDIAN 0 )
else()
        set( ECKIT_BIG_ENDIAN    0 )
        set( ECKIT_LITTLE_ENDIAN 1 )
endif()

set( ECKIT_BIG_ENDIAN    ${ECKIT_BIG_ENDIAN}    )
set( ECKIT_LITTLE_ENDIAN ${ECKIT_LITTLE_ENDIAN} )

### check support for DL library

check_include_files( dlfcn.h  ECKIT_HAVE_DLFCN_H )

cmake_push_check_state(RESET)
    set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_DL_LIBS} )
    check_c_source_compiles( "#define _GNU_SOURCE\n#include <dlfcn.h>\nint main(){ void* addr; Dl_info info; dladdr(addr, &info); }\n"
        ECKIT_HAVE_DLADDR )
cmake_pop_check_state()

### system headers and symbols tests for eckit config headers

check_symbol_exists( MAP_ANONYMOUS "sys/mman.h"  ECKIT_HAVE_MAP_ANONYMOUS )
check_symbol_exists( MAP_ANON      "sys/mman.h"  ECKIT_HAVE_MAP_ANON )
check_symbol_exists( funopen       "stdio.h"     ECKIT_HAVE_FUNOPEN )
check_symbol_exists( fsync         "unistd.h"    ECKIT_HAVE_FSYNC)
check_symbol_exists( fdatasync     "unistd.h"    ECKIT_HAVE_FDATASYNC)
check_symbol_exists( F_FULLFSYNC   "fcntl.h"     ECKIT_HAVE_F_FULLFSYNC)
check_symbol_exists( fmemopen      "stdio.h"     ECKIT_HAVE_FMEMOPEN )

check_c_source_compiles( "#define _GNU_SOURCE\n#include <stdio.h>\nint main(){ void* cookie; const char* mode; cookie_io_functions_t iof; FILE* fopencookie(void *cookie, const char *mode, cookie_io_functions_t iof); }"
    ECKIT_HAVE_FOPENCOOKIE )

check_c_source_compiles( "#include <unistd.h>\n#include <execinfo.h>\n int main(){ void ** buffer; int i = backtrace(buffer, 256); }\n"
    ECKIT_HAVE_EXECINFO_BACKTRACE )

check_cxx_source_compiles( "#include <cxxabi.h>\n int main() { char * type; int status; char * r = abi::__cxa_demangle(type, 0, 0, &status); }"
    ECKIT_HAVE_CXXABI_H )

check_c_source_compiles( "#include <time.h>\nint main(){ time_t now; time(&now); struct tm t; gmtime_r(&now,&t); }\n"
    ECKIT_HAVE_GMTIME_R )

check_c_source_compiles( "#include <dirent.h>\nint main(){ DIR *dirp; struct dirent *entry; struct dirent **result; int i = readdir_r(dirp, entry, result); }\n"
    ECKIT_HAVE_READDIR_R )

check_c_source_compiles( "#include <sys/types.h>\n#include <dirent.h>\nint main(){ DIR *dirp; int i = dirfd(dirp); }\n"
    ECKIT_HAVE_DIRFD )

check_c_source_compiles( "#include <dirent.h>\nint main(){ DIR *dirp; struct dirent *entry; if(entry->d_type) { dirp = 0; } }\n"
    ECKIT_HAVE_DIRENT_D_TYPE )

check_cxx_source_compiles( "int main() { __int128 i = 0; return 0;}"
    ECKIT_HAVE_CXX_INT_128 )

### config headers

ecbuild_generate_config_headers( DESTINATION ${INSTALL_INCLUDE_DIR}/eckit )

configure_file( eckit_config.h.in   eckit_config.h  )
configure_file( eckit_version.h.in  eckit_version.h )

install(FILES
			${CMAKE_CURRENT_BINARY_DIR}/eckit_config.h
			${CMAKE_CURRENT_BINARY_DIR}/eckit_version.h
		DESTINATION
			${INSTALL_INCLUDE_DIR}/eckit )

configure_file( eckit_version.cc.in  eckit_version.cc )

### eckit sources

list( APPEND eckit_srcs eckit.h eckit_version.cc )

list( APPEND eckit_container_srcs

container/BTree.h
container/BTree.cc
container/Trie.h
container/Trie.cc
container/BloomFilter.h
container/BloomFilter.cc
container/Queue.h
container/Cache.h
container/CacheLRU.h
container/CacheLRU.cc

container/CacheManager.h
container/CacheManager.cc
container/DenseMap.h
container/StatCollector.h
container/Recycler.h
container/ClassExtent.h
container/KDMemory.h
container/KDMapped.cc
container/KDMapped.h

container/KDTree.h
container/BSPTree.h

container/MappedArray.h
container/MappedArray.cc

container/SharedMemArray.h
container/SharedMemArray.cc

container/sptree/SPIterator.h
container/sptree/SPNode.h

container/sptree/SPNodeQueue.h
container/sptree/SPTree.h
container/sptree/SPValue.h
container/sptree/SPNodeInfo.h
container/sptree/SPMetadata.h

container/kdtree/KDNode.h
container/kdtree/KDNode.cc
container/kdtree/KDNode.cc

container/bsptree/BSPNode.h
container/bsptree/BSPNode.cc
container/bsptree/BSPHyperPlane.h


)

list( APPEND eckit_io_srcs
io/Length.h
io/Length.cc
io/DataHandle.h
io/DataHandle.cc
io/FDataSync.h
io/FDataSync.cc
io/Select.h
io/Select.cc
io/CommandStream.h
io/CommandStream.cc
io/Offset.h
io/Offset.cc
io/DataBlob.h
io/DataBlob.cc
io/Buffer.cc
io/Buffer.h
io/SharedBuffer.cc
io/SharedBuffer.h
io/CircularBuffer.cc
io/CircularBuffer.h
io/BufferCache.h
io/BufferCache.cc
io/FileLocker.h
io/FileLocker.cc
io/FileLock.h
io/FileLock.cc
io/FilePool.h
io/FilePool.cc
io/ResizableBuffer.h
io/ResizableBuffer.cc
io/DblBuffer.h
io/DblBuffer.cc
io/HandleBuf.h
io/HandleBuf.cc
io/StdFile.h
io/StdFile.cc
io/StdPipe.h
io/StdPipe.cc
io/StdioBuf.h
io/StdioBuf.cc
io/FileBase.h
io/FileBase.cc
io/TransferWatcher.h
io/TransferWatcher.cc
io/MoverTransfer.h
io/MoverTransfer.cc
io/SockBuf.h
io/SockBuf.cc
io/AutoCloser.h
io/AIOHandle.h
io/AIOHandle.cc
io/AsyncHandle.h
io/AsyncHandle.cc
io/BitIO.h
io/BitIO.cc
io/Compress.h
io/Compress.cc
io/Base64.h
io/Base64.cc
io/SharedHandle.h
io/SharedHandle.cc
io/BufferedHandle.h
io/BufferedHandle.cc
io/MemoryHandle.h
io/MemoryHandle.cc
io/EmptyHandle.h
io/EmptyHandle.cc
io/FTPHandle.h
io/FTPHandle.cc
io/URLHandle.h
io/URLHandle.cc
io/EasyCURL.h
io/EasyCURL.cc
io/FileDescHandle.h
io/FileDescHandle.cc
io/FileHandle.h
io/FileHandle.cc
io/MarsFSHandle.h
io/MarsFSHandle.cc
io/MarsFSPartHandle.h
io/MarsFSPartHandle.cc
io/PartHandle.h
io/PartHandle.cc
io/MultiPartHandle.h
io/MultiPartHandle.cc
io/StatsHandle.h
io/StatsHandle.cc
io/MoverHandle.h
io/MoverHandle.cc
io/MultiHandle.h
io/MultiHandle.cc
io/PartFileHandle.h
io/PartFileHandle.cc
io/PipeHandle.h
io/PipeHandle.cc
io/StdFileHandle.h
io/StdFileHandle.cc
io/RawFileHandle.cc
io/RawFileHandle.h
io/TCPHandle.h
io/TCPHandle.cc
io/TCPSocketHandle.h
io/TCPSocketHandle.cc
io/TeeHandle.h
io/TeeHandle.cc
io/Pipeline.h
io/Pipeline.cc
io/HandleHolder.h
io/HandleHolder.cc
io/cluster/ClusterDisks.h
io/cluster/ClusterDisks.cc
io/cluster/ClusterNode.h
io/cluster/ClusterNode.cc
io/cluster/ClusterNodes.h
io/cluster/ClusterNodes.cc
io/cluster/NodeInfo.h
io/cluster/NodeInfo.cc )

list( APPEND eckit_filesystem_srcs
filesystem/FileSystemSize.h
filesystem/PathExpander.h
filesystem/PathExpander.cc
filesystem/PathName.h
filesystem/PathName.cc
filesystem/BasePathNameT.h
filesystem/BasePathNameT.cc
filesystem/BasePathName.cc
filesystem/BasePathName.h
filesystem/LocalPathName.h
filesystem/LocalPathName.cc
filesystem/FileSpace.h
filesystem/FileSpace.cc
filesystem/FileSpaceStrategies.h
filesystem/FileSpaceStrategies.cc
filesystem/FileSystem.h
filesystem/FileSystem.cc
filesystem/TempFile.h
filesystem/TempFile.cc
filesystem/TmpDir.h
filesystem/TmpDir.cc
filesystem/TmpFile.h
filesystem/TmpFile.cc
filesystem/marsfs/MarsFSFile.h
filesystem/marsfs/MarsFSFile.cc
filesystem/marsfs/MarsFSPath.h
filesystem/marsfs/MarsFSPath.cc
filesystem/marsfs/MarsFSClient.h
filesystem/marsfs/MarsFSClient.cc
filesystem/URIManager.h
filesystem/URIManager.cc
filesystem/URI.h
filesystem/URI.cc )

list( APPEND eckit_thread_srcs
    thread/AutoLock.h
    thread/Mutex.cc
    thread/Mutex.h
    thread/MutexCond.cc
    thread/MutexCond.h
    thread/Once.h
    thread/StaticMutex.cc
    thread/StaticMutex.h
    thread/Thread.cc
    thread/Thread.h
    thread/ThreadControler.cc
    thread/ThreadControler.h
    thread/ThreadPool.cc
    thread/ThreadPool.h
    thread/ThreadSingleton.h
)

list( APPEND eckit_config_srcs
config/Configurable.cc
config/Configurable.h
config/Configuration.cc
config/Configuration.h
config/Configured.cc
config/Configured.h
config/EtcTable.cc
config/EtcTable.h
config/JSONConfiguration.h
config/LibEcKit.cc
config/LibEcKit.h
config/LocalConfiguration.cc
config/LocalConfiguration.h
config/Parametrisation.cc
config/Parametrisation.h
config/Resource.h
config/ResourceBase.h
config/ResourceBase.cc
config/ResourceMgr.cc
config/ResourceMgr.h
config/YAMLConfiguration.cc
config/YAMLConfiguration.h
)

list( APPEND eckit_runtime_srcs
runtime/Task.h
runtime/Task.cc
runtime/TaskID.h
runtime/Main.h
runtime/Main.cc
runtime/Application.h
runtime/Application.cc
runtime/Tool.h
runtime/Tool.cc
runtime/Library.h
runtime/Library.cc
runtime/Monitor.h
runtime/Monitor.cc
runtime/Monitorable.h
runtime/Monitorable.cc
runtime/TaskInfo.h
runtime/TaskInfo.cc
runtime/PipeApplication.h
runtime/PipeApplication.cc
runtime/PipeHandler.h
runtime/PipeHandler.cc
runtime/ProcessControler.h
runtime/ProcessControler.cc
runtime/Dispatcher.h
runtime/Pipe.h
runtime/ProducerConsumer.h )

list( APPEND eckit_log_srcs
log/BigNum.h
log/BigNum.cc
log/Bytes.h
log/Bytes.cc
log/CallbackTarget.h
log/CallbackTarget.cc
log/TimeStampTarget.h
log/TimeStampTarget.cc
log/PrefixTarget.h
log/PrefixTarget.cc
log/WrapperTarget.h
log/WrapperTarget.cc
log/IndentTarget.h
log/IndentTarget.cc
log/Channel.h
log/Channel.cc
log/OStreamTarget.h
log/OStreamTarget.cc
log/LineBasedTarget.h
log/LineBasedTarget.cc
log/TeeTarget.h
log/TeeTarget.cc
log/StatusTarget.h
log/StatusTarget.cc
log/MonitorTarget.h
log/MonitorTarget.cc
log/MessageTarget.h
log/MessageTarget.cc
log/Number.h
log/Number.cc
log/ChannelBuffer.h
log/ChannelBuffer.cc
log/CodeLocation.h
log/CodeLocation.cc
log/ColouringTarget.h
log/ColouringTarget.cc
log/Colour.h
log/Colour.cc
log/ETA.h
log/ETA.cc
log/FileTarget.h
log/FileTarget.cc
log/Log.h
log/Log.cc
log/Plural.h
log/Progress.h
log/Progress.cc
log/RotationTarget.h
log/RotationTarget.cc
log/SavedStatus.h
log/SavedStatus.cc
log/Seconds.h
log/Seconds.cc
log/SysLog.h
log/SysLog.cc
log/Statistics.h
log/Statistics.cc
log/LogTarget.h
log/LogTarget.cc
log/ProgressTimer.h
log/ProgressTimer.cc
log/Timer.h
log/Timer.cc
log/TraceTimer.h
log/ResourceUsage.h
log/ResourceUsage.cc
log/TimeStamp.h
log/TimeStamp.cc
log/UserChannel.h
log/UserChannel.cc
)

list( APPEND eckit_exception_srcs
exception/Exceptions.h
exception/Exceptions.cc )

list( APPEND eckit_types_srcs
types/Types.h
types/Types.cc
types/Double.h
types/Double.cc
types/Fraction.h
types/Fraction.cc
types/FixedString.h
types/FloatCompare.h
types/FloatCompare.cc
types/ClimateDate.h
types/ClimateDate.cc
types/Coord.h
types/Coord.cc
types/DateTime.h
types/DateTime.cc
types/DayOfYear.h
types/DayOfYear.cc
types/Grid.h
types/Grid.cc
types/Metadata.h
types/Metadata.cc
types/Month.h
types/Month.cc
types/TimeInterval.h
types/TimeInterval.cc
types/Date.h
types/Date.cc
types/UUID.h
types/UUID.cc
types/Time.h
types/Time.cc
types/VerifyingDate.h
types/VerifyingDate.cc
)

list( APPEND eckit_parser_srcs
parser/StreamParser.h
parser/StreamParser.cc
parser/StringTools.h
parser/StringTools.cc
parser/Tokenizer.h
parser/Tokenizer.cc
parser/JSON.h
parser/JSON.cc
parser/ObjectParser.h
parser/ObjectParser.cc
parser/JSONParser.h
parser/JSONParser.cc
parser/YAMLParser.h
parser/YAMLParser.cc
parser/JSONDataBlob.h
parser/JSONDataBlob.cc
parser/JSONMetadata.h
parser/JSONMetadata.cc )

list( APPEND eckit_value_srcs
value/BoolContent.h
value/BoolContent.cc
value/CompositeParams.cc
value/CompositeParams.h
value/Content.h
value/Content.cc
value/DateContent.h
value/DateContent.cc
value/TimeContent.h
value/TimeContent.cc
value/DateTimeContent.h
value/DateTimeContent.cc
value/DispatchParams.h
value/DoubleContent.h
value/DoubleContent.cc
value/Expression.h
value/ListContent.h
value/ListContent.cc
value/MapContent.h
value/MapContent.cc
value/OrderedMapContent.h
value/OrderedMapContent.cc
value/NilContent.h
value/NilContent.cc
value/NumberContent.h
value/NumberContent.cc
value/Params.cc
value/Params.h
value/Properties.h
value/Properties.cc
value/ScopeParams.cc
value/ScopeParams.h
value/StringContent.h
value/StringContent.cc
value/Value.h
value/Value.cc

#sql/SQLMATCHSubquerySession.h
#sql/SQLMATCHSubquerySession.cc
#sql/SQLMATCHSubquerySessionOutput.h
#sql/SQLMATCHSubquerySessionOutput.cc
sql/Environment.cc
sql/Environment.h
sql/SQLBitColumn.cc
sql/SQLBitColumn.h
sql/SQLColumn.cc
sql/SQLColumn.h
sql/SQLDatabase.cc
sql/SQLDatabase.h
sql/SQLDistinctOutput.cc
sql/SQLDistinctOutput.h
sql/SQLOrderOutput.cc
sql/SQLOrderOutput.h
sql/SQLOutput.cc
sql/SQLOutput.h
sql/SQLOutputConfig.cc
sql/SQLOutputConfig.h
sql/SQLParser.cc
sql/SQLParser.h
sql/SelectOneTable.cc
sql/SelectOneTable.h
sql/SQLSelect.cc
sql/SQLSelect.h
sql/SQLSelectFactory.cc
sql/SQLSelectFactory.h
sql/SQLSession.cc
sql/SQLSession.h
sql/SQLSimpleOutput.cc
sql/SQLSimpleOutput.h
sql/SQLStatement.cc
sql/SQLStatement.h
sql/SQLTable.cc
sql/SQLTable.h
sql/SQLTableFactory.cc
sql/SQLTableFactory.h
sql/SQLTypedefs.h

sql/expression/BitColumnExpression.cc
sql/expression/BitColumnExpression.h
sql/expression/ColumnExpression.cc
sql/expression/ColumnExpression.h
sql/expression/ConstantExpression.cc
sql/expression/ConstantExpression.h
sql/expression/NumberExpression.cc
sql/expression/NumberExpression.h
sql/expression/ParameterExpression.cc
sql/expression/ParameterExpression.h
sql/expression/StringExpression.cc
sql/expression/StringExpression.h
sql/expression/SQLExpression.cc
sql/expression/SQLExpression.h
sql/expression/SQLExpressions.cc
sql/expression/SQLExpressions.h
sql/expression/ShiftedColumnExpression.cc
sql/expression/ShiftedColumnExpression.h
sql/expression/SQLExpressionEvaluated.cc
sql/expression/SQLExpressionEvaluated.h
sql/expression/OrderByExpressions.cc
sql/expression/OrderByExpressions.h

sql/expression/function/piconst.h
sql/expression/function/EqRegionCache.cc
sql/expression/function/EqRegionCache.h
sql/expression/function/RggRegionCache.cc
sql/expression/function/RggRegionCache.h
sql/expression/function/RegionCache.cc
sql/expression/function/RegionCache.h
sql/expression/function/FunctionExpression.cc
sql/expression/function/FunctionExpression.h
sql/expression/function/FunctionAND.cc
sql/expression/function/FunctionAND.h
sql/expression/function/FunctionAVG.cc
sql/expression/function/FunctionAVG.h
sql/expression/function/FunctionCOUNT.cc
sql/expression/function/FunctionCOUNT.h
sql/expression/function/FunctionDOTP.cc
sql/expression/function/FunctionDOTP.h
sql/expression/function/FunctionEQ.cc
sql/expression/function/FunctionEQ.h
sql/expression/function/FunctionRLIKE.cc
sql/expression/function/FunctionRLIKE.h
sql/expression/function/FunctionEQ_BOXLAT.cc
sql/expression/function/FunctionEQ_BOXLAT.h
sql/expression/function/FunctionEQ_BOXLON.cc
sql/expression/function/FunctionEQ_BOXLON.h
#sql/expression/function/FunctionMATCH.h
#sql/expression/function/FunctionMATCH.cc
sql/expression/function/FunctionFactory.cc
sql/expression/function/FunctionFactory.h
sql/expression/function/FunctionIN.cc
sql/expression/function/FunctionIN.h
sql/expression/function/FunctionIntegerExpression.cc
sql/expression/function/FunctionIntegerExpression.h
sql/expression/function/FunctionJOIN.cc
sql/expression/function/FunctionJOIN.h
sql/expression/function/FunctionJULIAN.cc
sql/expression/function/FunctionJULIAN.h
sql/expression/function/FunctionJULIAN_SECONDS.cc
sql/expression/function/FunctionJULIAN_SECONDS.h
sql/expression/function/FunctionMAX.cc
sql/expression/function/FunctionMAX.h
sql/expression/function/FunctionMIN.cc
sql/expression/function/FunctionMIN.h
sql/expression/function/FunctionFIRST.cc
sql/expression/function/FunctionFIRST.h
sql/expression/function/FunctionLAST.cc
sql/expression/function/FunctionLAST.h
sql/expression/function/FunctionNORM.cc
sql/expression/function/FunctionNORM.h
sql/expression/function/FunctionNOT_IN.cc
sql/expression/function/FunctionNOT_IN.h
sql/expression/function/FunctionNOT_NULL.cc
sql/expression/function/FunctionNOT_NULL.h
sql/expression/function/FunctionNULL.cc
sql/expression/function/FunctionNULL.h
sql/expression/function/FunctionNVL.cc
sql/expression/function/FunctionNVL.h
sql/expression/function/FunctionOR.cc
sql/expression/function/FunctionOR.h
sql/expression/function/FunctionRGG_BOXLAT.cc
sql/expression/function/FunctionRGG_BOXLAT.h
sql/expression/function/FunctionRGG_BOXLON.cc
sql/expression/function/FunctionRGG_BOXLON.h
sql/expression/function/FunctionRMS.cc
sql/expression/function/FunctionRMS.h
sql/expression/function/FunctionROWNUMBER.cc
sql/expression/function/FunctionROWNUMBER.h
sql/expression/function/FunctionSTDEV.cc
sql/expression/function/FunctionSTDEV.h
sql/expression/function/FunctionSUM.cc
sql/expression/function/FunctionSUM.h
sql/expression/function/FunctionTDIFF.cc
sql/expression/function/FunctionTDIFF.h
sql/expression/function/FunctionTHIN.cc
sql/expression/function/FunctionTHIN.h
sql/expression/function/FunctionTIMESTAMP.cc
sql/expression/function/FunctionTIMESTAMP.h
sql/expression/function/FunctionVAR.cc
sql/expression/function/FunctionVAR.h

sql/type/SQLBit.cc
sql/type/SQLBit.h
sql/type/SQLBitfield.cc
sql/type/SQLBitfield.h
sql/type/SQLDouble.cc
sql/type/SQLDouble.h
sql/type/SQLInt.cc
sql/type/SQLInt.h
sql/type/SQLReal.cc
sql/type/SQLReal.h
sql/type/SQLString.cc
sql/type/SQLString.h
sql/type/SQLType.cc
sql/type/SQLType.h
)

list( APPEND eckit_os_srcs
os/Stat.h
os/System.h
os/System.cc
os/AutoUmask.h
os/SemLocker.h
os/SemLocker.cc
os/Semaphore.h
os/Semaphore.cc
os/SignalHandler.h
os/SignalHandler.cc
os/Password.h
os/Password.cc
os/BackTrace.h
os/BackTrace.cc
os/AutoAlarm.h
os/AutoAlarm.cc
os/SharedInt.h
os/SharedInt.cc )

list( APPEND eckit_net_srcs
net/TCPClient.h
net/TCPClient.cc
net/TCPSocket.h
net/TCPSocket.cc
net/TCPStream.h
net/TCPStream.cc
net/NetAddress.h
net/NetAddress.cc
net/NetService.h
net/NetService.cc
net/NetUser.h
net/NetUser.cc
net/Telnetable.h
net/Telnetable.cc
net/Telnet.h
net/Telnet.cc
net/TelnetUser.h
net/Connector.h
net/Connector.cc
net/TCPServer.h
net/TCPServer.cc
net/UDPClient.h
net/UDPClient.cc
net/UDPServer.h
net/UDPServer.cc
net/Port.h
net/Port.cc )

list( APPEND eckit_serialisation_srcs
serialisation/BadTag.h
serialisation/BadTag.cc
serialisation/Stream.h
serialisation/Stream.cc
serialisation/Streamable.h
serialisation/Streamable.cc
serialisation/FileStream.h
serialisation/FileStream.cc
serialisation/MemoryStream.h
serialisation/MemoryStream.cc
serialisation/HandleStream.h
serialisation/FstreamStream.h
serialisation/IfstreamStream.h
serialisation/PipeStream.h
serialisation/PipeStream.cc
serialisation/Reanimator.h
serialisation/Reanimator.cc
serialisation/ReanimatorBase.cc )

list( APPEND eckit_persist_srcs
persist/Bless.h
persist/Exporter.h
persist/Exporter.cc
persist/DumpLoad.h
persist/DumpLoad.cc
persist/Isa.h
persist/Isa.cc )

list( APPEND eckit_utils_srcs
utils/Translator.h
utils/Translator.cc
utils/Hash.h
utils/Hash.cc
utils/MD5.h
utils/MD5.cc
utils/Regex.h
utils/Regex.cc
utils/RendezvousHash.h
utils/RendezvousHash.cc
utils/HyperCube.h
utils/HyperCube.cc
utils/RLE.h
utils/RLE.cc )

if(ECKIT_HAVE_SSL)
    list( APPEND eckit_utils_srcs
      utils/MD4.h
      utils/MD4.cc
      utils/SHA1.h
      utils/SHA1.cc
    )
endif()

if(ECKIT_HAVE_XXHASH)
    list( APPEND eckit_utils_srcs
      utils/xxHash.h
      utils/xxHash.cc
    )
endif()

list( APPEND eckit_memory_srcs
memory/Builder.h
memory/Builder.cc
memory/Counted.cc
memory/Counted.h
memory/Factory.h
memory/Owned.h
memory/MemoryPool.h
memory/MemoryPool.cc
memory/MemoryBuffer.h
memory/MemoryBuffer.cc
memory/MMap.cc
memory/MMap.h
memory/Shmget.cc
memory/Shmget.h
memory/SharedPtr.h
memory/SharedPtr.cc
memory/Padded.h
memory/MapAllocator.h
memory/MapAllocator.cc
memory/NonCopyable.h
memory/NonCopyable.cc
memory/ScopedPtr.h
memory/Zero.h )

list( APPEND eckit_compat_srcs
compat/Inited.h
compat/StrStream.h
)

list( APPEND eckit_maths_srcs
maths/Functions.h
maths/Functions.cc
)


list( APPEND eckit_system_srcs
    system/Library.cc
    system/Library.h
    system/ResourceUsage.cc
    system/ResourceUsage.h
    system/SystemInfo.cc
    system/SystemInfo.h
    system/MemoryInfo.cc
    system/MemoryInfo.h
)

set(ECKIT_SYSTEM_EXTRA_LIBS "")
if( EC_OS_NAME STREQUAL "linux" )
    list( APPEND eckit_system_srcs system/SystemInfoLinux.h system/SystemInfoLinux.cc )
elseif( EC_OS_NAME STREQUAL "freebsd" )
    list( APPEND eckit_system_srcs system/SystemInfoFreeBSD.h system/SystemInfoFreeBSD.cc )
    list( APPEND ECKIT_SYSTEM_EXTRA_LIBS "execinfo" ) # Add in support for backtrace on FreeBSD
elseif( EC_OS_NAME STREQUAL "macosx" )
    list( APPEND eckit_system_srcs system/SystemInfoMacOSX.h system/SystemInfoMacOSX.cc )
else()
    ecbuild_warn("eckit does not support SystemInfo for OS '${EC_OS_NAME}'")
endif()

if(HAVE_JEMALLOC)
    list(APPEND ECKIT_SYSTEM_EXTRA_LIBS ${JEMALLOC_LIBRARIES})
endif()

list( APPEND eckit_bases_srcs
bases/Watcher.h
bases/Watcher.cc
bases/Loader.h
bases/Loader.cc )

list( APPEND eckit_transaction_srcs
    transaction/TxnEvent.h
    transaction/TxnEvent.cc
    transaction/TxnLog.h
    transaction/TxnLog.cc
)

list( APPEND eckit_testing_srcs
    testing/Test.h
)

list( APPEND eckit_dirs
	bases
	compat
	config
	container
	exception
	filesystem
	io
	log
    maths
	memory
	net
	os
	parser
	persist
	runtime
	serialisation
	thread
	transaction
    types
    testing
    utils
	value
    system
)

foreach( dir ${eckit_dirs} )
  list( APPEND eckit_srcs ${eckit_${dir}_srcs} )
endforeach()

list( APPEND eckit_templates
					container/CacheLRU.cc
                    container/BTree.cc
                    container/Trie.cc
                    container/BloomFilter.cc
                    container/MappedArray.cc
                    container/SharedMemArray.cc
                    container/bsptree/BSPNode.cc
					container/kdtree/KDNode.cc
					container/sptree/SPNode.cc
					filesystem/BasePathNameT.cc
					io/FileBase.cc
					runtime/PipeHandler.cc
					serialisation/Reanimator.cc
					transaction/TxnLog.cc
					types/Types.cc
					utils/RLE.cc
                    sql/SQLIteratorOutput.cc
                    sql/expression/ShiftedColumnExpression.cc
)

list( APPEND eckit_persistent
					io/Length.h
					io/Offset.h
					types/ClimateDate.h
					types/Date.h
					types/DateTime.h
					types/DayOfYear.h
					types/Double.h
					types/Grid.h
					types/Month.h
					types/Time.h
					types/VerifyingDate.h
)

### eckit library

ecbuild_add_library(TARGET eckit
					INSTALL_HEADERS LISTED
					HEADER_DESTINATION
						${INSTALL_INCLUDE_DIR}/eckit
					SOURCES
						${eckit_srcs}
					GENERATED
						eckit_version.cc
					TEMPLATES
						${eckit_templates}
					PERSISTENT
						${eckit_persistent}
                    PRIVATE_INCLUDES
                        "${CURL_INCLUDE_DIRS}"
                        "${XXHASH_INCLUDE_DIRS}"
                    INCLUDES
                        "${OPENSSL_INCLUDE_DIR}"
                        "${AIO_INCLUDE_DIRS}"
					LIBS
                        ${OPENSSL_LIBRARIES}
                        ${CURL_LIBRARIES}
                        ${XXHASH_LIBRARIES}
                        ${AIO_LIBRARIES}
                        ${CMATH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT}
                        ${CMAKE_DL_LIBS}
                        ${ECKIT_SYSTEM_EXTRA_LIBS}
                   LINKER_LANGUAGE CXX )

### Parser for SQL functionality

ecbuild_generate_yy( YYPREFIX           eckit_sql_
                     YACC               sql/sqly
                     LEX                sql/sqll
                     FLEX_FLAGS         "-d"
                     DEPENDANT          sql/SQLParser.cc )

### sub-directories

if( HAVE_ECKIT_CMD )
  add_subdirectory( cmd )
endif()

add_subdirectory( geometry )
add_subdirectory( linalg )
add_subdirectory( maths )
add_subdirectory( mpi )
add_subdirectory( option )
add_subdirectory( web )

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

check_include_files( dlfcn.h  eckit_HAVE_DLFCN_H )

if(NOT eckit_HAVE_DLFCN_H)
message(FATAL_ERROR "eckit requires dlfcn.h for supporting dlopen of shared libraries")
endif()

cmake_push_check_state(RESET)
    set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_DL_LIBS} )
    check_c_source_compiles( "#define _GNU_SOURCE\n#include <dlfcn.h>\nint main(){ void* addr; Dl_info info; dladdr(addr, &info); }\n"
        eckit_HAVE_DLADDR )
cmake_pop_check_state()

### system headers and symbols tests for eckit config headers

check_symbol_exists( MAP_ANONYMOUS "sys/mman.h"  eckit_HAVE_MAP_ANONYMOUS )
check_symbol_exists( MAP_ANON      "sys/mman.h"  eckit_HAVE_MAP_ANON )
check_symbol_exists( funopen       "stdio.h"     eckit_HAVE_FUNOPEN )
check_symbol_exists( fsync         "unistd.h"    eckit_HAVE_FSYNC)
check_symbol_exists( fdatasync     "unistd.h"    eckit_HAVE_FDATASYNC)
check_symbol_exists( F_FULLFSYNC   "fcntl.h"     eckit_HAVE_F_FULLFSYNC)
check_symbol_exists( fmemopen      "stdio.h"     eckit_HAVE_FMEMOPEN )
check_symbol_exists( dlinfo        "dlfcn.h"     eckit_HAVE_DLINFO)

check_c_source_compiles( "#define _GNU_SOURCE\n#include <stdio.h>\nint main(){ void* cookie; const char* mode; cookie_io_functions_t iof; FILE* fopencookie(void *cookie, const char *mode, cookie_io_functions_t iof); }"
    eckit_HAVE_FOPENCOOKIE )

check_c_source_compiles( "#include <unistd.h>\n#include <execinfo.h>\n int main(){ void ** buffer; int i = backtrace(buffer, 256); }\n"
    eckit_HAVE_EXECINFO_BACKTRACE )

check_cxx_source_compiles( "#include <cxxabi.h>\n int main() { char * type; int status; char * r = abi::__cxa_demangle(type, 0, 0, &status); }"
    eckit_HAVE_CXXABI_H )

check_c_source_compiles( "#include <time.h>\nint main(){ time_t now; time(&now); struct tm t; gmtime_r(&now,&t); }\n"
    eckit_HAVE_GMTIME_R )

check_c_source_compiles( "#include <dirent.h>\nint main(){ DIR *dirp; struct dirent *entry; struct dirent **result; int i = readdir_r(dirp, entry, result); }\n"
    eckit_HAVE_READDIR_R )

check_c_source_compiles( "#include <sys/types.h>\n#include <dirent.h>\nint main(){ DIR *dirp; int i = dirfd(dirp); }\n"
    eckit_HAVE_DIRFD )

check_c_source_compiles( "#include <dirent.h>\nint main(){ DIR *dirp; struct dirent *entry; if(entry->d_type) { dirp = 0; } }\n"
    eckit_HAVE_DIRENT_D_TYPE )

check_cxx_source_compiles( "int main() { __int128 i = 0; return 0;}"
    eckit_HAVE_CXX_INT_128 )

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

list( APPEND eckit_srcs eckit.h deprecated.h ${CMAKE_CURRENT_BINARY_DIR}/eckit_version.cc )

list( APPEND eckit_container_srcs

container/BSPTree.h
container/BTree.cc
container/BTree.h
container/BloomFilter.cc
container/BloomFilter.h
container/Cache.h
container/CacheLRU.cc
container/CacheLRU.h
container/CacheManager.cc
container/CacheManager.h
container/ClassExtent.h
container/DenseMap.h
container/DenseSet.h
container/KDMapped.cc
container/KDMapped.h
container/KDMemory.h
container/KDTree.h
container/MappedArray.cc
container/MappedArray.h
container/Queue.h
container/Recycler.h
container/SharedMemArray.cc
container/SharedMemArray.h
container/StatCollector.h
container/Trie.cc
container/Trie.h
container/bsptree/BSPHyperPlane.h
container/bsptree/BSPNode.cc
container/bsptree/BSPNode.h
container/kdtree/KDNode.cc
container/kdtree/KDNode.cc
container/kdtree/KDNode.h
container/sptree/SPIterator.h
container/sptree/SPMetadata.h
container/sptree/SPNode.h
container/sptree/SPNodeInfo.h
container/sptree/SPNodeQueue.h
container/sptree/SPTree.h
container/sptree/SPValue.h
)

list( APPEND eckit_io_srcs
io/AsyncHandle.cc
io/AsyncHandle.h
io/AIOHandle.cc
io/AIOHandle.h
io/AutoCloser.h
io/Base64.cc
io/Base64.h
io/BitIO.cc
io/BitIO.h
io/Buffer.cc
io/Buffer.h
io/BufferCache.cc
io/BufferCache.h
io/BufferList.cc
io/BufferList.h
io/BufferedHandle.cc
io/BufferedHandle.h
io/PeekHandle.cc
io/PeekHandle.h
io/SeekableHandle.cc
io/SeekableHandle.h
io/CircularBuffer.cc
io/CircularBuffer.h
io/CommandStream.cc
io/CommandStream.h
io/Compress.cc
io/Compress.h
io/DataHandle.cc
io/DataHandle.h
io/DblBuffer.cc
io/DblBuffer.h
io/EmptyHandle.cc
io/EmptyHandle.h
io/FDataSync.cc
io/FDataSync.h
io/FTPHandle.cc
io/FTPHandle.h
io/FileBase.cc
io/FileBase.h
io/FileDescHandle.cc
io/FileDescHandle.h
io/FileHandle.cc
io/FileHandle.h
io/FOpenDataHandle.cc
io/MMappedFileHandle.cc
io/MMappedFileHandle.h
io/FileLock.cc
io/FileLock.h
io/FileLocker.cc
io/FileLocker.h
io/FilePool.cc
io/FilePool.h
io/FileHandle.cc
io/FileHandle.h
io/HandleBuf.cc
io/HandleBuf.h
io/HandleHolder.cc
io/HandleHolder.h
io/Length.cc
io/Length.h
io/MemoryHandle.cc
io/MemoryHandle.h
io/MoverTransfer.cc
io/MoverTransfer.h
io/MultiHandle.cc
io/MultiHandle.h
io/Offset.cc
io/Offset.h
io/PartFileHandle.cc
io/PartFileHandle.h
io/PartHandle.cc
io/PartHandle.h
io/PipeHandle.cc
io/PipeHandle.h
io/Pipeline.cc
io/Pipeline.h
io/PooledFile.cc
io/PooledFile.h
io/PooledHandle.cc
io/PooledHandle.h
io/PooledFileDescriptor.cc
io/PooledFileDescriptor.h
io/RawFileHandle.cc
io/RawFileHandle.h
io/ResizableBuffer.h
io/Select.cc
io/Select.h
io/SharedBuffer.cc
io/SharedBuffer.h
io/SharedHandle.cc
io/SharedHandle.h
io/SockBuf.cc
io/SockBuf.h
io/StatsHandle.cc
io/StatsHandle.h
io/StdFile.cc
io/StdFile.h
io/StdPipe.cc
io/StdPipe.h
io/StdioBuf.cc
io/StdioBuf.h
io/TCPHandle.cc
io/TCPHandle.h
io/MultiSocketHandle.cc
io/MultiSocketHandle.h
io/TCPSocketHandle.cc
io/TCPSocketHandle.h
io/TeeHandle.cc
io/TeeHandle.h
io/TransferWatcher.cc
io/TransferWatcher.h
io/cluster/ClusterDisks.cc
io/cluster/ClusterDisks.h
io/cluster/ClusterNode.cc
io/cluster/ClusterNode.h
io/cluster/ClusterNodes.cc
io/cluster/ClusterNodes.h
io/cluster/NodeInfo.cc
io/cluster/NodeInfo.h
)

if(HAVE_CURL)
list(APPEND eckit_io_srcs
io/EasyCURL.cc
io/EasyCURL.h
io/URLHandle.cc
io/URLHandle.h)
endif()

list(APPEND eckit_message_srcs
message/Decoder.cc
message/Decoder.h
message/Message.cc
message/Message.h
message/MessageContent.cc
message/MessageContent.h
message/Reader.cc
message/Reader.h
message/Splitter.cc
message/Splitter.h
)

if(HAVE_RADOS)
list( APPEND eckit_io_srcs
io/rados/RadosHandle.cc
io/rados/RadosHandle.h
io/rados/RadosCluster.h
io/rados/RadosCluster.cc
io/rados/RadosReadHandle.cc
io/rados/RadosReadHandle.h
io/rados/RadosWriteHandle.cc
io/rados/RadosWriteHandle.h
io/rados/RadosObject.h
io/rados/RadosObject.cc
io/rados/RadosAttributes.h
io/rados/RadosAttributes.cc
)
endif()

list( APPEND eckit_filesystem_srcs
filesystem/BasePathName.cc
filesystem/BasePathName.h
filesystem/BasePathNameT.cc
filesystem/BasePathNameT.h
filesystem/FileMode.cc
filesystem/FileMode.h
filesystem/FileSpace.cc
filesystem/FileSpace.h
filesystem/FileSpaceStrategies.cc
filesystem/FileSpaceStrategies.h
filesystem/FileSystem.cc
filesystem/FileSystem.h
filesystem/FileSystemSize.h
filesystem/LocalPathName.cc
filesystem/LocalPathName.h
filesystem/PathExpander.cc
filesystem/PathExpander.h
filesystem/PathName.cc
filesystem/PathName.h
filesystem/PathNameFactory.cc
filesystem/PathNameFactory.h
filesystem/TempFile.cc
filesystem/TempFile.h
filesystem/TmpDir.cc
filesystem/TmpDir.h
filesystem/StdDir.cc
filesystem/StdDir.h
filesystem/TmpFile.cc
filesystem/TmpFile.h
filesystem/URI.cc
filesystem/URI.h
filesystem/URIManager.cc
filesystem/URIManager.h
filesystem/LocalFileManager.cc
filesystem/LocalFileManager.h
)

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
config/ResourceBase.cc
config/ResourceBase.h
config/ResourceMgr.cc
config/ResourceMgr.h
config/YAMLConfiguration.cc
config/YAMLConfiguration.h
)

list( APPEND eckit_runtime_srcs
runtime/Application.cc
runtime/Application.h
runtime/Dispatcher.h
runtime/Library.cc
runtime/Library.h
runtime/Main.cc
runtime/Main.h
runtime/Metrics.cc
runtime/Metrics.h
runtime/Monitor.cc
runtime/Monitor.h
runtime/Monitorable.cc
runtime/Monitorable.h
runtime/Pipe.h
runtime/PipeApplication.cc
runtime/PipeApplication.h
runtime/PipeHandler.cc
runtime/PipeHandler.h
runtime/ProcessControler.cc
runtime/ProcessControler.h
runtime/ProducerConsumer.h
runtime/Telemetry.cc
runtime/Telemetry.h
runtime/SessionID.cc
runtime/SessionID.h
runtime/Task.cc
runtime/Task.h
runtime/TaskID.h
runtime/TaskInfo.cc
runtime/TaskInfo.h
runtime/Tool.cc
runtime/Tool.h
)

list( APPEND eckit_log_srcs
log/BigNum.cc
log/BigNum.h
log/Bytes.cc
log/Bytes.h
log/CallbackTarget.cc
log/CallbackTarget.h
log/Channel.cc
log/Channel.h
log/ChannelBuffer.cc
log/ChannelBuffer.h
log/CodeLocation.cc
log/CodeLocation.h
log/Colour.cc
log/Colour.h
log/ColouringTarget.cc
log/ColouringTarget.h
log/ETA.cc
log/ETA.h
log/FileTarget.cc
log/FileTarget.h
log/IndentTarget.cc
log/IndentTarget.h
log/JSON.cc
log/JSON.h
log/LineBasedTarget.cc
log/LineBasedTarget.h
log/Log.cc
log/Log.h
log/LogTarget.cc
log/LogTarget.h
log/MessageTarget.cc
log/MessageTarget.h
log/MonitorTarget.cc
log/MonitorTarget.h
log/Number.cc
log/Number.h
log/OStreamTarget.cc
log/OStreamTarget.h
log/Plural.h
log/PrefixTarget.cc
log/PrefixTarget.h
log/Progress.cc
log/Progress.h
log/ProgressTimer.cc
log/ProgressTimer.h
log/ResourceUsage.cc
log/ResourceUsage.h
log/RotationTarget.cc
log/RotationTarget.h
log/SavedStatus.cc
log/SavedStatus.h
log/Seconds.cc
log/Seconds.h
log/Statistics.cc
log/Statistics.h
log/StatusTarget.cc
log/StatusTarget.h
log/SysLog.cc
log/SysLog.h
log/TeeTarget.cc
log/TeeTarget.h
log/TimeStamp.cc
log/TimeStamp.h
log/TimeStampTarget.cc
log/TimeStampTarget.h
log/Timer.cc
log/Timer.h
log/TraceTimer.h
log/UserChannel.cc
log/UserChannel.h
log/WrapperTarget.cc
log/WrapperTarget.h
)

list( APPEND eckit_exception_srcs
exception/Exceptions.cc
exception/Exceptions.h
)

list( APPEND eckit_types_srcs
types/ClimateDate.cc
types/ClimateDate.h
types/Coord.cc
types/Coord.h
types/Date.cc
types/Date.h
types/DateTime.cc
types/DateTime.h
types/DayOfYear.cc
types/DayOfYear.h
types/Double.cc
types/Double.h
types/FixedString.h
types/FloatCompare.cc
types/FloatCompare.h
types/Fraction.cc
types/Fraction.h
types/Grid.cc
types/Grid.h
types/Hour.cc
types/Hour.h
types/Month.cc
types/Month.h
types/Time.cc
types/Time.h
types/TimeInterval.cc
types/TimeInterval.h
types/Types.cc
types/Types.h
types/UUID.cc
types/UUID.h
types/SemanticVersion.cc
types/SemanticVersion.h
types/VerifyingDate.cc
types/VerifyingDate.h
)

list( APPEND eckit_parser_srcs
parser/CSVParser.cc
parser/CSVParser.h
parser/JSON.h
parser/JSONParser.cc
parser/JSONParser.h
parser/ObjectParser.cc
parser/ObjectParser.h
parser/StreamParser.cc
parser/StreamParser.h
parser/YAMLParser.cc
parser/YAMLParser.h
)

list( APPEND eckit_value_srcs
value/BoolContent.cc
value/BoolContent.h
value/CompositeParams.cc
value/CompositeParams.h
value/Content.cc
value/Content.h
value/DateContent.cc
value/DateContent.h
value/DateTimeContent.cc
value/DateTimeContent.h
value/DispatchParams.h
value/DoubleContent.cc
value/DoubleContent.h
value/Expression.h
value/ListContent.cc
value/ListContent.h
value/MapContent.cc
value/MapContent.h
value/NilContent.cc
value/NilContent.h
value/NumberContent.cc
value/NumberContent.h
value/OrderedMapContent.cc
value/OrderedMapContent.h
value/Params.cc
value/Params.h
value/Properties.cc
value/Properties.h
value/ScopeParams.cc
value/ScopeParams.h
value/StringContent.cc
value/StringContent.h
value/TimeContent.cc
value/TimeContent.h
value/Value.cc
value/Value.h
)

list( APPEND eckit_os_srcs
os/AutoAlarm.cc
os/AutoAlarm.h
os/AutoUmask.h
os/BackTrace.cc
os/BackTrace.h
os/Password.cc
os/Password.h
os/SemLocker.cc
os/SemLocker.h
os/Semaphore.cc
os/Semaphore.h
os/SharedInt.cc
os/SharedInt.h
os/SignalHandler.cc
os/SignalHandler.h
os/Stat.h
os/System.cc
os/System.h
)

list( APPEND eckit_net_srcs
net/Connector.cc
net/Connector.h
net/Endpoint.cc
net/Endpoint.h
net/HttpHeader.cc
net/HttpHeader.h
net/IPAddress.cc
net/IPAddress.h
net/NetMask.cc
net/NetMask.h
net/NetService.cc
net/NetService.h
net/NetUser.cc
net/NetUser.h
net/Port.cc
net/Port.h
net/ProxiedTCPClient.cc
net/ProxiedTCPClient.h
net/ProxiedTCPServer.cc
net/ProxiedTCPServer.h
net/SocketOptions.cc
net/SocketOptions.h
net/TCPClient.cc
net/TCPClient.h
net/TCPServer.cc
net/TCPServer.h
net/TCPSocket.cc
net/TCPSocket.h
net/MultiSocket.cc
net/MultiSocket.h
net/TCPStream.cc
net/TCPStream.h
net/Telnet.cc
net/Telnet.h
net/TelnetUser.cc
net/TelnetUser.h
net/Telnetable.cc
net/Telnetable.h
net/UDPClient.cc
net/UDPClient.h
net/UDPServer.cc
net/UDPServer.h
)

list( APPEND eckit_serialisation_srcs
serialisation/BadTag.cc
serialisation/BadTag.h
serialisation/FileStream.cc
serialisation/FileStream.h
serialisation/FstreamStream.h
serialisation/HandleStream.h
serialisation/IfstreamStream.h
serialisation/MemoryStream.cc
serialisation/MemoryStream.h
serialisation/PipeStream.cc
serialisation/PipeStream.h
serialisation/Reanimator.cc
serialisation/Reanimator.h
serialisation/ReanimatorBase.cc
serialisation/ResizableMemoryStream.cc
serialisation/ResizableMemoryStream.h
serialisation/Stream.cc
serialisation/Stream.h
serialisation/Streamable.cc
serialisation/Streamable.h
)

list( APPEND eckit_persist_srcs
persist/Bless.h
persist/DumpLoad.cc
persist/DumpLoad.h
persist/Exporter.cc
persist/Exporter.h
persist/Isa.cc
persist/Isa.h
)

list( APPEND eckit_utils_srcs
  utils/ByteSwap.h
  utils/Compressor.cc
  utils/Compressor.h
  utils/Hash.cc
  utils/Hash.h
  utils/HyperCube.cc
  utils/HyperCube.h
  utils/MD5.cc
  utils/MD5.h
  utils/Optional.h
  utils/RLE.cc
  utils/RLE.h
  utils/Regex.cc
  utils/Regex.h
  utils/RendezvousHash.cc
  utils/RendezvousHash.h
  utils/StringTools.cc
  utils/StringTools.h
  utils/Tokenizer.cc
  utils/Tokenizer.h
  utils/Clock.h
  utils/Translator.cc
  utils/Translator.h
)

if(eckit_HAVE_BZIP2)
    list( APPEND eckit_utils_srcs
      utils/BZip2Compressor.cc
      utils/BZip2Compressor.h
    )
endif()

if(eckit_HAVE_SNAPPY)
  list( APPEND eckit_utils_srcs
    utils/SnappyCompressor.cc
    utils/SnappyCompressor.h
  )
endif()

if(eckit_HAVE_LZ4)
  list( APPEND eckit_utils_srcs
    utils/LZ4Compressor.cc
    utils/LZ4Compressor.h
  )
endif()

if(eckit_HAVE_AEC)
  list( APPEND eckit_utils_srcs
    utils/AECCompressor.cc
    utils/AECCompressor.h
  )
endif()

if(eckit_HAVE_SSL)
    list( APPEND eckit_utils_srcs
      utils/MD4.cc
      utils/MD4.h
      utils/SHA1.cc
      utils/SHA1.h
    )
endif()

if(eckit_HAVE_RSYNC)
  list( APPEND eckit_utils_srcs
    utils/Rsync.cc
    utils/Rsync.h
  )
endif()

if(eckit_HAVE_XXHASH)
    list( APPEND eckit_utils_srcs
        utils/xxHashing.cc
        utils/xxHashing.h
        contrib/xxhash/xxhash.h
    )
    if( CMAKE_CXX_COMPILER_ID MATCHES PGI|NVHPC AND
        CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 21.9 )
        # ECKIT-574: work around missing reference to "__builtin_rotateleft64"
        set_source_files_properties(utils/xxHashing.cc PROPERTIES COMPILE_FLAGS -DNO_CLANG_BUILTIN)
    endif()
endif()


list( APPEND eckit_memory_srcs
memory/Builder.cc
memory/Builder.h
memory/Counted.cc
memory/Counted.h
memory/Factory.h
memory/MMap.cc
memory/MMap.h
memory/MapAllocator.cc
memory/MapAllocator.h
memory/MemoryBuffer.cc
memory/MemoryBuffer.h
memory/NonCopyable.cc
memory/NonCopyable.h
memory/OnlyMovable.h
memory/Owned.h
memory/Padded.h
memory/ScopedPtr.h
memory/SharedPtr.cc
memory/SharedPtr.h
memory/Shmget.cc
memory/Shmget.h
memory/Zero.h
)

list( APPEND eckit_compat_srcs
compat/Inited.h
compat/StrStream.h
)

list( APPEND eckit_maths_srcs
maths/Functions.cc
maths/Functions.h
)


list( APPEND eckit_system_srcs
    system/Plugin.cc
    system/Plugin.h
    system/Library.cc
    system/Library.h
    system/LibraryManager.cc
    system/LibraryManager.h
    system/MemoryInfo.cc
    system/MemoryInfo.h
    system/ResourceUsage.cc
    system/ResourceUsage.h
    system/SystemInfo.cc
    system/SystemInfo.h
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
bases/Loader.cc
bases/Loader.h
bases/Watcher.cc
bases/Watcher.h
)

list( APPEND eckit_transaction_srcs
    transaction/TxnEvent.cc
    transaction/TxnEvent.h
    transaction/TxnLog.cc
    transaction/TxnLog.h
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
    memory
    message
    net
    os
    parser
    persist
    runtime
    serialisation
    thread
    transaction
    value
    maths
    system
    testing
    types
    utils
)

foreach( dir ${eckit_dirs} )
  list( APPEND eckit_srcs ${eckit_${dir}_srcs} )
endforeach()

list( APPEND eckit_templates
                    container/CacheLRU.cc
                    container/kdtree/KDNode.cc
                    container/sptree/SPNode.cc
                    filesystem/BasePathNameT.cc
                    io/FileBase.cc
                    runtime/PipeHandler.cc
                    serialisation/Reanimator.cc
                    transaction/TxnLog.cc
                    types/Types.cc
                    utils/RLE.cc
                    container/BTree.cc
                    container/BloomFilter.cc
                    container/MappedArray.cc
                    container/SharedMemArray.cc
                    container/Trie.cc
                    container/bsptree/BSPNode.cc
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

ecbuild_add_library(
          TARGET eckit
          TYPE SHARED
          INSTALL_HEADERS LISTED
          HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit

          SOURCES
						  ${eckit_srcs}

          TEMPLATES
						  ${eckit_templates}

          PERSISTENT
						  ${eckit_persistent}

          PUBLIC_INCLUDES
              $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
              $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>

          PRIVATE_INCLUDES
              "${CURL_INCLUDE_DIRS}"
              "${SNAPPY_INCLUDE_DIRS}"
              "${LZ4_INCLUDE_DIRS}"
              "${BZIP2_INCLUDE_DIRS}"
              "${AEC_INCLUDE_DIRS}"
              "${RADOS_INCLUDE_DIRS}"
              "${OPENSSL_INCLUDE_DIR}"
              "${AIO_INCLUDE_DIRS}"

          PRIVATE_LIBS
              "${SNAPPY_LIBRARIES}"
              "${LIBRSYNC_LIBRARIES}"
              "${LZ4_LIBRARIES}"
              "${BZIP2_LIBRARIES}"
              "${AEC_LIBRARIES}"
              "${OPENSSL_LIBRARIES}"
              "${CURL_LIBRARIES}"
              "${AIO_LIBRARIES}"
              "${RADOS_LIBRARIES}"

          PUBLIC_LIBS
              ${CMATH_LIBRARIES}
              ${RT_LIBRARIES}
              ${THREADS_LIBRARIES}
              ${CMAKE_DL_LIBS}
              ${ECKIT_SYSTEM_EXTRA_LIBS}

          LINKER_LANGUAGE CXX )

### sub-directories

if( HAVE_ECKIT_CMD )
  add_subdirectory( cmd )
endif()

if( HAVE_ECKIT_SQL )
  add_subdirectory( sql )
endif()

add_subdirectory( geometry )
add_subdirectory( linalg )
add_subdirectory( maths )
add_subdirectory( mpi )
add_subdirectory( option )
add_subdirectory( web )

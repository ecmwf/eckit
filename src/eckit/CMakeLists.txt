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
io/Buffer.cc
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
io/AIOHandle.h
io/AIOHandle.cc
io/BufferedHandle.h
io/BufferedHandle.cc
io/MemoryHandle.h
io/MemoryHandle.cc
io/EmptyHandle.h
io/EmptyHandle.cc
io/FTPHandle.h
io/FTPHandle.cc
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
io/SharedHandle.h
io/SharedHandle.cc
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
filesystem/PathName.h
filesystem/PathName.cc
filesystem/BasePathNameT.h
filesystem/BasePathNameT.cc
filesystem/BasePathName.h
filesystem/LocalPathName.h
filesystem/LocalPathName.cc
filesystem/FileManager.h
filesystem/FileManager.cc
filesystem/FileName.h
filesystem/FileName.cc
filesystem/FileSpace.h
filesystem/FileSpace.cc
filesystem/FileSpaceStrategies.h
filesystem/FileSpaceStrategies.cc
filesystem/FileSystem.h
filesystem/FileSystem.cc
filesystem/TempFile.h
filesystem/TempFile.cc
filesystem/TmpFile.h
filesystem/TmpFile.cc
filesystem/marsfs/MarsFSFile.h
filesystem/marsfs/MarsFSFile.cc
filesystem/marsfs/MarsFSPath.h
filesystem/marsfs/MarsFSPath.cc
filesystem/marsfs/MarsFSClient.h
filesystem/marsfs/MarsFSClient.cc )

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
config/JSONConfiguration.cc
config/JSONConfiguration.h
config/LibEcKit.cc
config/LibEcKit.h
config/LocalConfiguration.cc
config/LocalConfiguration.h
config/Parametrisation.h
config/Resource.h
config/Resource.cc
config/ResourceBase.cc
config/ResourceMgr.cc
config/ResourceMgr.h
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
log/MessageTarget.h
log/MessageTarget.cc
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
log/OStreamHandle.h
log/Plural.h
log/Progress.h
log/Progress.cc
log/RotationTarget.h
log/RotationTarget.cc
log/SavedStatus.h
log/SavedStatus.cc
log/Seconds.h
log/Seconds.cc
log/Statistics.h
log/Statistics.cc
log/LogTarget.h
log/LogTarget.cc
log/Timer.h
log/Timer.cc
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
parser/JSONParser.h
parser/JSONParser.cc
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
value/DispatchParams.h
value/DoubleContent.h
value/DoubleContent.cc
value/Expression.h
value/ListContent.h
value/ListContent.cc
value/MapContent.h
value/MapContent.cc
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
value/Value.cc )

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
net/Port.h
net/Port.cc )

list( APPEND eckit_serialisation_srcs
serialisation/Stream.h
serialisation/Stream.cc
serialisation/Streamable.h
serialisation/Streamable.cc
serialisation/FileStream.h
serialisation/FileStream.cc
serialisation/MemoryStream.h
serialisation/MemoryStream.cc
serialisation/HandleStream.h
serialisation/PipeStream.h
serialisation/PipeStream.cc
serialisation/Reanimator.h
serialisation/Reanimator.cc
serialisation/ReanimatorBase.cc )

list( APPEND eckit_persist_srcs
persist/Exporter.h
persist/Exporter.cc
persist/DumpLoad.h
persist/DumpLoad.cc
persist/Isa.cc )

list( APPEND eckit_utils_srcs
utils/Translator.h
utils/Translator.cc
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

list( APPEND eckit_memory_srcs
memory/Builder.h
memory/Builder.cc
memory/Counted.cc
memory/Counted.h
memory/Factory.h
memory/Owned.h
memory/MemoryPool.h
memory/MemoryPool.cc
memory/SharedPtr.h
memory/Padded.h
memory/MapAllocator.h
memory/MapAllocator.cc
memory/NonCopyable.h
memory/NonCopyable.cc
memory/ScopedPtr.h )

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
)

if( EC_OS_NAME STREQUAL "linux" )
    list( APPEND eckit_system_srcs system/SystemInfoLinux.h system/SystemInfoLinux.cc )
elseif( EC_OS_NAME STREQUAL "macosx" )
    list( APPEND eckit_system_srcs system/SystemInfoMacOSX.h system/SystemInfoMacOSX.cc )
else()
    ecbuild_log_warning("eckit does not support SystemInfo for OS '${EC_OS_NAME}'")
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
testing/Setup.h
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
	testing
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
                    container/CacheManager.cc
                    container/BTree.cc
                    container/MappedArray.cc
                    container/SharedMemArray.cc
                    container/bsptree/BSPNode.cc
					container/kdtree/KDNode.cc
					container/sptree/SPNode.cc
					filesystem/BasePathNameT.cc
					io/FileBase.cc
					config/Resource.cc
					runtime/PipeHandler.cc
					serialisation/Reanimator.cc
					transaction/TxnLog.cc
					types/Types.cc
					utils/RLE.cc
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
					INCLUDES
						${AIO_INCLUDE_DIRS}
					LIBS
                        ${AIO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} )

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

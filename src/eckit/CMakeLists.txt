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
container/SimpleMappedArray.cc
container/SimpleMappedArray.h

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
io/Buffer.h
io/Buffer.cc
io/BufferCache.h
io/BufferCache.cc
io/FileLocker.h
io/FileLocker.cc
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
thread/Mutex.h
thread/Mutex.cc
thread/MutexCond.h
thread/MutexCond.cc
thread/AutoLock.h
thread/AutoLocker.h
thread/AutoLocker.cc
thread/Thread.h
thread/Thread.cc
thread/ThreadControler.h
thread/ThreadControler.cc
thread/ThreadPool.h
thread/ThreadPool.cc
thread/ThreadSingleton.h
thread/Once.h )

list( APPEND eckit_config_srcs
config/Resource.h
config/ResourceFromFiles.h
config/ResourceFromFiles.cc
config/ResourcePolicy.h
config/ResourceMgr.h
config/ResourceMgr.cc
config/Assignment.h
config/Assignment.cc
config/Block.h
config/Block.cc
config/Branch.h
config/Branch.cc
config/Compiler.h
config/Compiler.cc
config/Condition.h
config/Condition.cc
config/Function.h
config/Function.cc
config/Scope.h
config/Scope.cc
config/Script.h
config/Script.cc
config/Statement.h
config/Statement.cc
config/Configurable.h
config/Configurable.cc
config/EtcTable.h
config/EtcTable.cc
config/Parametrisation.h
config/DynamicParametrisation.h
config/DynamicParametrisation.cc
config/Configuration.h
config/Configuration.cc
config/JSONConfiguration.h
config/JSONConfiguration.cc
config/LocalConfiguration.h
config/LocalConfiguration.cc
config/FileReadPolicy.h
config/FileReadPolicy.cc
config/ResourceBase.cc )

list( APPEND eckit_runtime_srcs
runtime/Task.h
runtime/Task.cc
runtime/TaskID.h
runtime/Context.h
runtime/Context.cc
runtime/Application.h
runtime/Application.cc
runtime/Tool.h
runtime/Tool.cc
runtime/ToolBehavior.h
runtime/ToolBehavior.cc
runtime/Monitor.h
runtime/Monitor.cc
runtime/Monitorable.h
runtime/Monitorable.cc
runtime/TaskInfo.h
runtime/TaskInfo.cc
runtime/LibBehavior.h
runtime/LibBehavior.cc
runtime/StandardBehavior.h
runtime/StandardBehavior.cc
runtime/ContextBehavior.h
runtime/ContextBehavior.cc
runtime/PipeApplication.h
runtime/PipeApplication.cc
runtime/PipeHandler.h
runtime/PipeHandler.cc
runtime/Policies.h
runtime/Policies.cc
runtime/ProcessControler.h
runtime/ProcessControler.cc
runtime/Dispatcher.h
runtime/Pipe.h
runtime/ProducerConsumer.h )

list( APPEND eckit_log_srcs
log/Channel.h
log/Channel.cc
log/ChannelBuffer.h
log/ChannelBuffer.cc
log/CallbackChannel.h
log/CallbackChannel.cc
log/FileChannel.h
log/FileChannel.cc
log/FormatChannel.h
log/FormatChannel.cc
log/FormatBuffer.h
log/FormatBuffer.cc
log/ColorizeFormat.h
log/ColorizeFormat.cc
log/MonitorChannel.h
log/MonitorChannel.cc
log/MultiChannel.h
log/MultiChannel.cc
log/UserChannel.h
log/UserChannel.cc
log/OStreamHandle.h
log/Log.h
log/Log.cc
log/BigNum.h
log/BigNum.cc
log/Bytes.h
log/Bytes.cc
log/Colour.h
log/Colour.cc
log/ETA.h
log/ETA.cc
log/Seconds.h
log/Seconds.cc 
log/Plural.h
log/Progress.h
log/Progress.cc
log/SavedStatus.h
log/SavedStatus.cc
log/Timer.h
log/Timer.cc
log/TimeStamp.h
log/TimeStamp.cc
log/CodeLocation.h
log/CodeLocation.cc )

list( APPEND eckit_exception_srcs
exception/BadValue.h
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
parser/Request.h 
parser/Request.cc 
parser/Cell.h
parser/Cell.cc
parser/List.h
parser/List.cc
parser/RequestParser.h 
parser/RequestParser.cc 
parser/StreamParser.h
parser/StreamParser.cc
parser/StringTools.h
parser/StringTools.cc
parser/Tokenizer.h
parser/Tokenizer.cc
parser/JSON.h
parser/JSON.cc
parser/JSONParser.h
parser/JSONParser.cc )

ecbuild_generate_yy( YYPREFIX    request_
                     YACC        parser/requesty
                     YACC_TARGET requesty
                     LEX         parser/requestl
                     LEX_TARGET  requestl
                     DEPENDANT   parser/RequestParser.cc )

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
utils/DataHandleFactory.h
utils/DataHandleFactory.cc
utils/FileHandleFactory.h
utils/FileHandleFactory.cc
utils/Interpreter.h
utils/Interpreter.cc
utils/Environment.h
utils/Environment.cc
utils/RequestHandler.h
utils/RequestHandler.cc
utils/ParameterizedRequestHandler.h
utils/ParameterizedRequestHandler.cc
utils/ExecutionContext.h
utils/ExecutionContext.cc
utils/SpecialFormHandler.h
utils/SpecialFormHandler.cc
utils/Module.cc
utils/Module.h
utils/Translator.h
utils/Translator.cc
utils/MD5.h
utils/MD5.cc
utils/Regex.h
utils/Regex.cc
utils/HyperCube.h
utils/HyperCube.cc
utils/RLE.h
utils/RLE.cc )

list( APPEND eckit_memory_srcs
memory/Builder.h
memory/Builder.cc
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
compat/StrStream.h
compat/Inited.h )

list( APPEND eckit_bases_srcs
bases/Watcher.h
bases/Watcher.cc
bases/Loader.h
bases/Loader.cc )

list( APPEND eckit_transaction_srcs
transaction/TxnLog.h
transaction/TxnLog.cc
transaction/TxnEvent.h
transaction/TxnEvent.cc )

list( APPEND eckit_testing_srcs
testing/UnitTest.h
testing/UnitTest.cc)

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
	utils
	value
)

foreach( dir ${eckit_dirs} )
  list( APPEND eckit_srcs ${eckit_${dir}_srcs} )
endforeach()

list( APPEND eckit_templates
					container/BTree.cc
					container/MappedArray.cc
					container/SimpleMappedArray.cc
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
						${CMAKE_THREAD_LIBS_INIT} ${AIO_LIBRARIES} )

### sub-directories

add_subdirectory( cmd )

add_subdirectory( web )

add_subdirectory( maths )

add_subdirectory( geometry )

add_subdirectory( grib )

add_subdirectory( mpi )


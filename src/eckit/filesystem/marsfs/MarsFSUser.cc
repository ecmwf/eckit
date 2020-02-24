/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/marsfs/MarsFSUser.h"

#include <netdb.h>

#include <algorithm>
#include <deque>
#include <fstream>
#include <memory>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/ResizableBuffer.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/Timer.h"
#include "eckit/net/TCPServer.h"
#include "eckit/os/Stat.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/utils/Hash.h"


using namespace eckit::net;

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// TODO: Return errno

MarsFSUser::MarsFSUser(net::TCPSocket& protocol):
	NetUser(protocol)
{
	std::string marsFsHashing = eckit::Resource<std::string>("marsFsHashing", "None");

	Log::info() << "MarsFS using hash " << marsFsHashing << std::endl;

	hash_.reset( eckit::HashFactory::instance().build(marsFsHashing) );
}

MarsFSUser::~MarsFSUser()
{
}

static const char *b = "KMGPH";  ///< @todo This is also in data handle

static double rate(double x, char& c)
{
	const char* p = b;
	c = ' ';
	while (x > 100)
	{
		x /= 1024;
		c = *p++;
	}
	if (x >= 10)
		return long(x + 0.5);
	else
		return long(x * 10 + 0.5) / 10.0;
}

#define T(z) o = timer.elapsed(); z ; streamTime += (timer.elapsed()-o); c++;

void MarsFSUser::serve(eckit::Stream& s, std::istream& in, std::ostream& out)
{
	std::unique_ptr<TCPSocket> data;
	std::unique_ptr<DataHandle> file;

	std::unique_ptr<TCPServer> server;

	std::string cmd;
	ResizableBuffer buffer(64 * 1024);
	std::string path;
	std::string mode;
	Length total = 0;

	double readTime = 0;
	double lastRead = 0;
	double writeTime = 0;
	double lastWrite = 0;
	// unused //	bool first = true;
	// unused //	double start = 0;

	double openTime = 0;
	double statTime = 0;
	double closeTime = 0;
	double acceptTime = 0;
	double streamTime = 0;
	double o = 0;
	long c = 0;

	bool ok = true;

	static int marsFSServerTimeout = eckit::Resource<int>("marsFSServerTimeout", 60);
	static int dataPortStartOffset = eckit::Resource<int>("dataPortStartOffset", 10000);
	static int useEphemeralPort = eckit::Resource<bool>("useEphemeralPort", true);

	// We don't use an ephemeral port due to the bug we saw Centos 7 and the same
	// port being allocated to two processes

	int dataPort = dataPortStartOffset + Monitor::instance().self();

	NodeInfo info;

	try
	{
		Timer timer("Server");

		info = NodeInfo::acceptLogin(s);
		Monitor::instance().name(info.name());

		Log::info() << "Connection from " << info << std::endl;

		for (;;)
		{

			Log::status() << info << std::endl;
			Log::message() << cmd << std::endl;

//			T(s >> cmd);
			if (!s.next(cmd))
			{
				Log::info() << "Connection with " << info << " terminated" << std::endl;
				ASSERT(!file.get());
				break;
			}

			Log::message() << '*' << cmd << std::endl;

			Log::debug() << "CMD: " << cmd << " " << info << std::endl;

			if (cmd == "open")
			{
				bool overwrite;
				ASSERT(!file.get());
				ASSERT(!data.get());

				T(s >> path; s >> mode; s >> overwrite);

				//Log::info() << cmd << " " << path << " " << mode << std::endl;
				Monitor::instance().state(mode[0]);

				ASSERT(data.get() == 0);
				double a = timer.elapsed();

				if (!server) {
					if (useEphemeralPort) {
						server.reset(new EphemeralTCPServer());
					}
					else {
						server.reset(new TCPServer(dataPort));
					}
				}


				s << server->localPort();

				Log::info() << "Waiting for data connection on port " << server->localPort() << ", " << path << std::endl;
				Log::info() << "Server " <<*server << std::endl;
				data.reset(new net::TCPSocket(server->accept("Waiting for data connection", marsFSServerTimeout)));
				Log::info() << "Accepted " <<*data << std::endl;

				Log::status() << data->remoteHost() << " " << path << std::endl;
				acceptTime = timer.elapsed() - a;

				double d = timer.elapsed();

				file.reset(new FileHandle(path, overwrite));

				if (mode == "r")
				{
					file->openForRead();
				}
				else
				{
					file->openForWrite(Length(0));
				}

				openTime = timer.elapsed() - d;

				unsigned long long len = 0;
				if (mode == "r")
				{
					double d = timer.elapsed();
					len = file->estimate();
					statTime = timer.elapsed() - d;
				}
				T(s << len;);
				continue;
			}

			if (cmd == "read")
			{
				long size;

				T(s >> size;);

				ASSERT(file.get());
				ASSERT(data.get());
				ASSERT(size >= 0);

				if (buffer.size() < size)
					buffer.resize(size);

				lastRead = timer.elapsed();
				size = file->read(buffer, size);
				double r = timer.elapsed() - lastRead;
				readTime += r;

				T(s << size;);

				// hash integrety check
				{
					// Timer t("MarsFSUser 'read' hashing");
					T(s << hash_->compute(buffer, size));
				}

				lastWrite = timer.elapsed();
				ASSERT(data->write(buffer, size) == size);
				double w = timer.elapsed() - lastWrite;
				writeTime += w;

				total += size;

				char c1, c2;
				Log::message() << rate(total / readTime, c1) << c1 << " " << rate(total / writeTime, c2) << c2 << std::endl;

				continue;
			}

			if (cmd == "write")
			{
				long size;
				T(s >> size;);

				ASSERT(file.get());
				ASSERT(data.get());
				ASSERT(size >= 0);

				if (buffer.size() < size)
					buffer.resize(size);

				lastRead = timer.elapsed();
				ASSERT(data->read(buffer, size) == size);
				double r = timer.elapsed() - lastRead;
				readTime += r;

				// hash integrety check
				{
					// Timer t("MarsFSUser 'write' hashing");
					std::string remoteHash;
					s >> remoteHash;

					const std::string localHash = hash_->compute(buffer, size);

					if (localHash != remoteHash) {
						std::ostringstream msg;
						msg << "Mismatch of hash while writing to MarsFS:"
						    << " expected remote hash " << remoteHash
						    << " local computed hash " << localHash;
						throw eckit::BadValue(msg.str(), Here());
					}
				}

				lastWrite = timer.elapsed();
				size = file->write(buffer, size);
				double w = timer.elapsed() - lastWrite;
				writeTime += w;

				T(s << size;);

				total += size;

				char c1, c2;
				Log::message() << rate(total / readTime, c1) << c1 << " " << rate(total / writeTime, c2) << c2 << std::endl;

				continue;
			}

			if (cmd == "seek")
			{
				unsigned long long pos;

				T(s >> pos; );

				// Log::info() << "seek(" << pos << ")" << std::endl;

				ASSERT(file.get());
				ASSERT(data.get());
				ASSERT(pos >= 0);

				pos = file->seek(pos);

				T(s << pos;);

				continue;
			}

			if (cmd == "skip")
			{
				unsigned long long n;
				bool ok = true;

				T(s >> n; );

				//Log::info() << "skip(" << n << ")" << std::endl;

				// TODO: AASERT(n fits in size_t

				ASSERT(file.get());
				ASSERT(data.get());

				file->skip(n);

				T(s << ok;);

				continue;
			}

			if (cmd == "close")
			{
				ASSERT(file.get());
				ASSERT(data.get());

				double d = timer.elapsed();

				Log::info() << "Close " << path << std::endl;

				file->close();
                file.reset();

				data->close();
                data.reset();

				closeTime = timer.elapsed() - d;

				T(s << ok);

				/*
				Log::info() << "Total: " << Bytes(total) << std::endl;
				Log::info() << "Elapsed: " << Seconds(timer.elapsed()) << std::endl;
				Log::info() << "Rate: " << Bytes(total, timer.elapsed()) << std::endl;
				Log::info() << "Overhead: " << Seconds((timer.elapsed() - start) - (writeTime + readTime)) << std::endl;
				Log::info() << "eckit::Stream: " << Seconds(streamTime) << " Commands: " << BigNum(c) << std::endl;
				Log::info() << "Open: " << Seconds(openTime) << std::endl;
				Log::info() << "Stat: " << Seconds(statTime) << std::endl;
				Log::info() << "Close: " << Seconds(closeTime) << std::endl;
				Log::info() << "Read: " << Seconds(readTime) << std::endl;
				Log::info() << "Write: " << Seconds(writeTime) << std::endl;
				Log::info() << "Accept: " << Seconds(acceptTime) << std::endl;
				Log::info() << "Read  rate: " << Bytes(total, readTime) << "/s" << std::endl;
				Log::info() << "Write rate: " << Bytes(total, writeTime) << "/s" << std::endl;
				*/

				continue;
			}

			if (cmd == "size")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;

				Stat::Struct info;
				SYSCALL(Stat::stat(p.c_str(), &info));
				s << (unsigned long long) info.st_size;
				continue;
			}

			if (cmd == "mkdir")
			{
				ASSERT(!file.get());
				std::string p;
				short mode;
				s >> p;
				s >> mode;
				//Log::info() << "mkdir(" << p << "," << mode << ")" << std::endl;
				LocalPathName(p).mkdir(mode);
				s << ok;
				continue;
			}

			if (cmd == "rename")
			{
				ASSERT(!file.get());
				std::string f;
				std::string t;
				s >> f;
				s >> t;
				Log::info() << "rename(" << f << "," << t << ")" << std::endl;
				LocalPathName::rename(LocalPathName(f), LocalPathName(t));
				s << ok;
				continue;
			}

			if (cmd == "link")
			{
				ASSERT(!file.get());
				std::string f;
				std::string t;
				s >> f;
				s >> t;
				Log::info() << "link(" << f << "," << t << ")" << std::endl;
				LocalPathName::link(LocalPathName(f), LocalPathName(t));
				s << ok;
				continue;
			}

			if (cmd == "sameas")
			{
				ASSERT(!file.get());
				std::string p1;
				std::string p2;
				s >> p1;
				s >> p2;
				Log::info() << "sameas(" << p1 << "," << p2 << ")" << std::endl;
				s << LocalPathName(p1).sameAs(LocalPathName(p2));
				continue;
			}

			if (cmd == "unlink")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;
				//Log::info() << "unlink(" << p << ")" << std::endl;
				// Unlink will print...
				LocalPathName(p).unlink();
				s << ok;
				continue;
			}

			if (cmd == "syncParentDirectory")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;
				//Log::info() << "syncParentDirectory(" << p << ")" << endl;
				// syncParentDirectory will print...
				LocalPathName(p).syncParentDirectory();
				s << ok;
				continue;
			}

			if (cmd == "rmdir")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;
				Log::info() << "rmdir(" << p << ")" << std::endl;
				LocalPathName(p).rmdir();
				s << ok;
				continue;
			}

			if (cmd == "touch")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;
				Log::info() << "touch(" << p << ")" << std::endl;
				LocalPathName(p).touch();
				s << ok;
				continue;
			}

			if (cmd == "exists")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;
				//Log::info() << "exists(" << p << ")" << std::endl;
				s << LocalPathName(p).exists();
				continue;
			}

			if (cmd == "mountp")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;
				//Log::info() << "mountp(" << p << ")" << std::endl;
				//Log::info() << LocalPathName(p).mountPoint() << std::endl;
				s << LocalPathName(p).mountPoint();
				continue;
			}

			if (cmd == "created")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;
				//Log::info() << "created(" << p << ")" << std::endl;
				s << (unsigned long long) LocalPathName(p).created();
				continue;
			}

			if (cmd == "lastAccess")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;
				//Log::info() << "lastAccess(" << p << ")" << std::endl;
				s << (unsigned long long) LocalPathName(p).lastAccess();
				continue;
			}

			if (cmd == "lastModified")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;
				//Log::info() << "lastModified(" << p << ")" << std::endl;
				s << (unsigned long long) LocalPathName(p).lastModified();
				continue;
			}

			if (cmd == "unique")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;
				Log::info() << "unique(" << p << ")" << std::endl;
				std::string u(LocalPathName::unique(p));
				s << u;
				continue;
			}

			if (cmd == "baseName")
			{
				ASSERT(!file.get());
				std::string p;
				bool ext;
				s >> p;
				s >> ext;
				Log::info() << "baseName(" << p << "," << ext << ")" << std::endl;
				s << LocalPathName(p).baseName(ext);
				continue;
			}

			if (cmd == "fullName")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;
				Log::info() << "fullName(" << p << ")" << std::endl;
				s << LocalPathName(p).fullName();
				continue;
			}

			if (cmd == "dirName")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;
				Log::info() << "dirName(" << p << ")" << std::endl;
				s << LocalPathName(p).dirName();
				continue;
			}

			if (cmd == "statfs")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;

//				Log::info() << "statfs(" << p << ")" << std::endl;

				FileSystemSize fs;
				LocalPathName(p).fileSystemSize(fs);
				s << fs.available;
				s << fs.total;
				continue;

			}

			if (cmd == "match")
			{
				ASSERT(!file.get());
				std::string p;
				bool recurse;
				s >> p;
				s >> recurse;

				Log::info() << "statfs(" << p << "," << recurse << ")" << std::endl;

				std::vector<LocalPathName> v;
				LocalPathName::match(p, v, recurse);

				s << long(v.size());
				for (std::vector<LocalPathName>::const_iterator j = v.begin(); j != v.end(); ++j)
					s << std::string(*j);
				continue;

			}

			if (cmd == "children")
			{
				ASSERT(!file.get());
				std::string p;
				s >> p;

				Log::info() << "children(" << p << std::endl;

				std::vector<LocalPathName> f;
				std::vector<LocalPathName> d;
				LocalPathName(p).children(f, d);

				/// @note follow the children() signature order, send files first then dirs

				s << long(f.size());
				for (std::vector<LocalPathName>::const_iterator j = f.begin(); j != f.end(); ++j)
					s << std::string(*j);

				s << long(d.size());
				for (std::vector<LocalPathName>::const_iterator j = d.begin(); j != d.end(); ++j)
					s << std::string(*j);

				continue;

			}

			if (cmd == "quit" || cmd == "bye")
			{
				ASSERT(!file.get());
				break;
			}

			s << eckit::UserError(std::string("Unknown command [") + cmd + "]");
			break;

		}
	}
	catch (std::exception& e)
	{
		Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
		Log::error() << "** Exception is hanlded" << std::endl;
		try
		{
			s << e;
		}
		catch (std::exception& a)
		{
			Log::error() << "** " << a.what() << " Caught in " << Here() << std::endl;
			Log::error() << "** Exception is ignored" << std::endl;
		}
	}

	try
	{
		if (file.get()) {
			Log::warning() << "Closing " << *file << std::endl;
			file->close();
		}
	}
	catch (std::exception& e)
	{
		Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
		Log::error() << "** Exception is ignored" << std::endl;
	}

	try
	{
		if (data.get()) {
			Log::warning() << "Closing " << *data << std::endl;
			data->close();
		}
	}
	catch (std::exception& e)
	{
		Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
		Log::error() << "** Exception is ignored" << std::endl;
	}

    file.reset();
    data.reset();

	Log::info() << "End of connection from " << info << std::endl;
}

//-------------------------------------------------------------------------------------------------

} // namespace eckit

#include "Swiften/LinkLocal/BonjourQuerier.h"

#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "Swiften/LinkLocal/BonjourBrowseQuery.h"
#include "Swiften/LinkLocal/BonjourRegisterQuery.h"
#include "Swiften/LinkLocal/BonjourResolveServiceQuery.h"
#include "Swiften/LinkLocal/BonjourResolveHostnameQuery.h"
#include "Swiften/Base/foreach.h"

namespace Swift {

BonjourQuerier::BonjourQuerier() : stopRequested(false), thread(0) {
	int fds[2];
	int result = pipe(fds);
	assert(result == 0);
	interruptSelectReadSocket = fds[0];
	fcntl(interruptSelectReadSocket, F_SETFL, fcntl(interruptSelectReadSocket, F_GETFL)|O_NONBLOCK);
	interruptSelectWriteSocket = fds[1];
}

BonjourQuerier::~BonjourQuerier() {
	stop();
}

boost::shared_ptr<DNSSDBrowseQuery> BonjourQuerier::createBrowseQuery() {
	return boost::shared_ptr<DNSSDBrowseQuery>(new BonjourBrowseQuery(shared_from_this()));
}

boost::shared_ptr<DNSSDRegisterQuery> BonjourQuerier::createRegisterQuery(const String& name, int port, const LinkLocalServiceInfo& info) {
	return boost::shared_ptr<DNSSDRegisterQuery>(new BonjourRegisterQuery(name, port, info, shared_from_this()));
}

boost::shared_ptr<DNSSDResolveServiceQuery> BonjourQuerier::createResolveServiceQuery(const LinkLocalServiceID& service) {
	return boost::shared_ptr<DNSSDResolveServiceQuery>(new BonjourResolveServiceQuery(service, shared_from_this()));
}

boost::shared_ptr<DNSSDResolveHostnameQuery> BonjourQuerier::createResolveHostnameQuery(const String& hostname, int interfaceIndex) {
	return boost::shared_ptr<DNSSDResolveHostnameQuery>(new BonjourResolveHostnameQuery(hostname, interfaceIndex, shared_from_this()));
}

void BonjourQuerier::addRunningQuery(boost::shared_ptr<BonjourQuery> query) {
	{
		boost::lock_guard<boost::mutex> lock(runningQueriesMutex);
		runningQueries.push_back(query);
	}
	runningQueriesAvailableEvent.notify_one();
	interruptSelect();
}

void BonjourQuerier::removeRunningQuery(boost::shared_ptr<BonjourQuery> query) {
	{
		boost::lock_guard<boost::mutex> lock(runningQueriesMutex);
		runningQueries.erase(std::remove(
			runningQueries.begin(), runningQueries.end(), query), runningQueries.end());
	}
}

void BonjourQuerier::interruptSelect() {
	char c = 0;
	write(interruptSelectWriteSocket, &c, 1);
}

void BonjourQuerier::start() {
	stop();
	thread = new boost::thread(boost::bind(&BonjourQuerier::run, shared_from_this()));
}

void BonjourQuerier::stop() {
	if (thread) {
		stopRequested = true;
		runningQueries.clear(); // TODO: Is this the right thing to do?
		runningQueriesAvailableEvent.notify_one();
		interruptSelect();
		thread->join();
		delete thread;
		stopRequested = false;
	}
}

void BonjourQuerier::run() {
	while (!stopRequested) {
		fd_set fdSet;
		int maxSocket;
		{
			boost::unique_lock<boost::mutex> lock(runningQueriesMutex);
			if (runningQueries.empty()) {
				runningQueriesAvailableEvent.wait(lock);
				if (runningQueries.empty()) {
					continue;
				}
			}

			// Run all running queries
			FD_ZERO(&fdSet);
			maxSocket = interruptSelectReadSocket;
			FD_SET(interruptSelectReadSocket, &fdSet);

			foreach(const boost::shared_ptr<BonjourQuery>& query, runningQueries) {
				int socketID = query->getSocketID();
				maxSocket = std::max(maxSocket, socketID);
				FD_SET(socketID, &fdSet);
			}
		}

		if (select(maxSocket+1, &fdSet, NULL, NULL, 0) <= 0) {
			continue;
		}

		if (FD_ISSET(interruptSelectReadSocket, &fdSet)) {
			char dummy;
			while (read(interruptSelectReadSocket, &dummy, 1) > 0) {}
		}

		{
			boost::lock_guard<boost::mutex> lock(runningQueriesMutex);
			foreach(boost::shared_ptr<BonjourQuery> query, runningQueries) {
				if (FD_ISSET(query->getSocketID(), &fdSet)) {
					query->processResult();
				}
			}
		}
	}
}

}

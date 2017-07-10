/*
 * Copyright (C) 2010 Maoxu Li. Email: maoxu@lebula.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NET_BASE_SOCKET_ADDRESS_HPP
#define NET_BASE_SOCKET_ADDRESS_HPP

#include "Config.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <string>
#include <cstring>

NET_BASE_BEGIN

//
// SocketAddress is an wrapper object that is compatible to the major data 
// structures used for socket address, such as struct sockaddr, struct sockaddr_in, 
// and struct sockaddr_storage. 
// 
class SocketAddress
{
private:
	// sockaddr_storage is big enough for all fmilies of socket address
	// ss_len is ignored in this implementation
	// The actual lenth of the adress is determined by ss_family
	sockaddr_storage mAddress;

public:
	// ss_family == AF_UNSPEC means the address is empty and the length of 
	// address is sizeof(sockaddr_storage)
	void Reset() { memset(&mAddress, 0, sizeof(struct sockaddr_storage)); }
	bool Empty() const { return mAddress.ss_family == AF_UNSPEC; }

	sa_family_t Family() const { return mAddress.ss_family; }
	socklen_t Length() const;

	std::string Host() const;
	unsigned short Port() const;
	void Port(unsigned short port);
	std::string ToString() const;

	const struct sockaddr* SockAddr() const 
	{
		return (struct sockaddr*)&mAddress;
	}

	struct sockaddr* SockAddr() 
	{
		return (struct sockaddr*)&mAddress;
	}

public: 
	// Constructor, empty address, all set to 0, AF_UNSPEC
	SocketAddress() throw() { memset(&mAddress, 0, sizeof(sockaddr_storage)); }

	// Constructor
	// if host is null and port is 0, loopback address, used for local host only
	// if host is null and port is non 0, wildcard address, used for server 
	SocketAddress(const char* host, unsigned short port, sa_family_t family = AF_INET) throw();

	// Copy constructor
	SocketAddress(const SocketAddress& sa) throw() { memcpy(&mAddress, &sa.mAddress,sizeof(sockaddr_storage)); }

	// Assignment operator
	inline SocketAddress& operator=(const SocketAddress& sa) throw()
	{
		if(&sa != this) 
			memcpy(&mAddress, &sa.mAddress, sizeof(struct sockaddr_storage));
		return *this;
	}

public: 
	// Copy counstructor, from data structures used in socket address
	SocketAddress(const struct sockaddr& sa) throw() { *this = sa; }
	explicit SocketAddress(const struct sockaddr* sa) throw() { *this = *sa; }

	SocketAddress(const struct sockaddr_in& sa) throw() { *this = sa; }
	explicit SocketAddress(const struct sockaddr_in* sa) throw() { *this = *sa; }

	SocketAddress(const struct sockaddr_in6& sa) throw() { *this = sa; }
	explicit SocketAddress(const struct sockaddr_in6* sa) throw() { *this = *sa; }
	
	SocketAddress(const struct sockaddr_storage& sa) throw() { *this = sa; }
	explicit SocketAddress(const struct sockaddr_storage* sa) throw() { *this = *sa; }

	// Assignment operator, from data structures used in socket address
	inline SocketAddress& operator=(const struct sockaddr& sa) throw()
	{
		if(reinterpret_cast<const struct sockaddr_storage*>(&sa) != &mAddress) 
		{
			memset(&mAddress, 0, sizeof(struct sockaddr_storage));
			switch(sa.sa_family) 
			{
				case AF_INET:
					memcpy(&mAddress, &sa, sizeof(struct sockaddr_in));
					break;
				case AF_INET6:
					memcpy(&mAddress, &sa, sizeof(struct sockaddr_in6));
					break;
			}
		}
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr_in& sa) throw()
	{
		if(reinterpret_cast<const struct sockaddr_storage*>(&sa) != &mAddress) 
		{
			memset(&mAddress, 0, sizeof(struct sockaddr_storage));
			memcpy(&mAddress, &sa, sizeof(struct sockaddr_in));
		}
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr_in6& sa) throw()
	{
		if(reinterpret_cast<const struct sockaddr_storage*>(&sa) != &mAddress) 
		{
			memset(&mAddress, 0, sizeof(struct sockaddr_storage));
			memcpy(&mAddress, &sa, sizeof(struct sockaddr_in6));
		}
		return *this;
	}

	inline SocketAddress& operator=(const struct sockaddr_storage& sa) throw()
	{
		if(&sa != &mAddress)
			memcpy(&mAddress, &sa,sizeof(struct sockaddr_storage));
		return *this;
	}

public:
	// Comparing operators, necessary for values in collection objects
	bool operator==(const SocketAddress& a) const throw();
	bool operator<(const SocketAddress& a) const throw();
	inline bool operator!=(const SocketAddress& a) const throw() { return !(*this == a); }
	inline bool operator>(const SocketAddress& a) const throw() { return (a < *this); }
	inline bool operator<=(const SocketAddress& a) const throw() { return !(a < *this); }
	inline bool operator>=(const SocketAddress& a) const throw() { return !(*this < a); }
};

NET_BASE_END

#endif

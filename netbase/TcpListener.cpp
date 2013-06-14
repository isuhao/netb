/*
 * Copyright (C) 2010-2016, Maoxu Li. Email: maoxu@lebula.com
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

#include "TcpListener.hpp"
#include "EventLoop.hpp"

NET_BASE_BEGIN

using namespace std::placeholders;

TcpListener::TcpListener(EventLoop* loop, sa_family_t family)
: mLoop(loop)
, mAddress()
, mSocket(family, SOCK_STREAM, IPPROTO_TCP)
, mHandler(loop, mSocket)
, mListening(false)
{
    mHandler.SetReadCallback(std::bind(&TcpListener::OnRead, this, _1));
}

TcpListener::TcpListener(EventLoop* loop, unsigned short port, sa_family_t family)
: mLoop(loop)
, mAddress(port, family)
, mSocket(family, SOCK_STREAM, IPPROTO_TCP)
, mHandler(loop, mSocket)
, mListening(false)
{
    mHandler.SetReadCallback(std::bind(&TcpListener::OnRead, this, _1));
}

TcpListener::TcpListener(EventLoop* loop, const char* host, unsigned short port, sa_family_t family)
: mLoop(loop)
, mAddress(host, port, family)
, mSocket(family, SOCK_STREAM, IPPROTO_TCP)
, mHandler(loop, mSocket)
, mListening(false)
{
    mHandler.SetReadCallback(std::bind(&TcpListener::OnRead, this, _1));
}

TcpListener::TcpListener(EventLoop* loop, const SocketAddress& addr)
: mLoop(loop)
, mAddress(addr)
, mSocket(addr.Family(), SOCK_STREAM, IPPROTO_TCP)
, mHandler(loop, mSocket)
, mListening(false)
{
    mHandler.SetReadCallback(std::bind(&TcpListener::OnRead, this, _1));
}

// Thead safe deletion
// Cleaning up on thread
// Block until cleanup is completed
TcpListener::~TcpListener()
{
    // Stop accepting new connections, but not close the socket
    // The socket will be closed by Socket object on destroy
    mSocket.Shutdown(); 

    // TcpListener is the OWNER AND MANAGER of the established connections
    for(std::vector<TcpConnection*>::iterator it = mConnections.begin(); 
        it != mConnections.end(); ++it)
    {
        TcpConnection* conn = *it;
        conn->Close();
        delete conn;
    }
}

// Listen on address passed on instantiation
bool TcpListener::Listen(int backlog)
{
    // If address is not assigned, listen without bind
    // Otherwise bind
    if(!mAddress.Empty() && !mSocket.Bind(mAddress.SockAddr(), mAddress.SockAddrLen()))
    {
        return false;
    }

    // Listen
    if(!mSocket.Listen(backlog))
    {
        return false;
    }
   
    // Enable async mode to accept incomming connections
    mSocket.Block(false);
    mHandler.EnableReading();
    mListening = true;
    return true;
}

// Listen on given port, 
// If address has been assigned, update port, 
// otherwise assign new address for given port,
// Loopback address for 0 port, and wildcard for others
bool TcpListener::Listen(unsigned short port, int backlog)
{
    // Update address
    if(!mAddress.Empty())
    {
        mAddress.Port(port);
    } 
    else
    {
        mAddress = SocketAddress(port, mSocket.Family());
    }
    assert(!mAddress.Empty());
    
    return Listen(backlog);
}

// Listen on given address
// Loopback address for NULL host
bool TcpListener::Listen(const char* host, unsigned short port, int backlog)
{
    // Update address
    mAddress = SocketAddress(host, port, mSocket.Family());
    assert(!mAddress.Empty());

    return Listen(backlog);
}

// Listen on given address
bool TcpListener::Listen(const SocketAddress& addr, int backlog)
{
    if(!addr.Empty() && addr.Family() != mSocket.Family())
    {
        return false;
    }
    mAddress = addr;
    return Listen(backlog);
}

// Todo: thread safe
SocketAddress TcpListener::Address() const
{
    SocketAddress addr;
    socklen_t addrlen = addr.SockAddrLen();
    mSocket.LocalAddress(addr.SockAddr(), &addrlen);
    assert(addr.SockAddrLen() == addrlen);
    return addr;
}

// Read is ready
// Callback by the eventloop
// By default all connections running on the same thread of listener
// Todo: multi-thread support, e.g. thread pool
void TcpListener::OnRead(SOCKET s)
{
    assert(s == mSocket);
    // Accept anyway
    SOCKET sock = mSocket.Accept();
    TcpConnection* conn = new TcpConnection(mLoop, sock);
    assert(conn != NULL);
    mConnections[sock] = conn;
    conn->SetConnectedCallback(mConnectedCallback);
    conn->Connected();
}

NET_BASE_END
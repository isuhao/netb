/*
 * Copyright (C) 2010, Maoxu Li. Email: maoxu@lebula.com
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

#ifndef NETB_CONFIG_HPP
#define NETB_CONFIG_HPP

//
// Namespace may be changed to adapt to project
//
#define NETB_NAMESPACE

#ifdef NETB_NAMESPACE
#   define NETB_BEGIN   namespace netb {
#   define NETB_END     }
#else
#   define NETB_BEGIN
#   define NETB_END
#endif

#define SAFE_NEW(ptr, ctor) do{ try{ ptr = new ctor; } catch(std::bad_alloc&){ ptr = nullptr; } } while(0) 
#define SAFE_DELETE(ptr) do{ if(ptr) { delete ptr; ptr = nullptr; } } while(0)

//
// Header files that used everywhere
// 
#include <iostream>
#include <sstream>
#include <cassert>

#endif

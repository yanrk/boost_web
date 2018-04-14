/********************************************************
 * Description : address
 * Data        : 2018-04-14 18:32:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#include <type_traits>
#include "address.h"

namespace BoostWeb { // namespace BoostWeb begin

Address::Address()
    : m_host_ip()
    , m_host_port(0)
    , m_peer_ip()
    , m_peer_port(0)
{

}

Address::Address(const Address & other)
    : m_host_ip(other.m_host_ip)
    , m_host_port(other.m_host_port)
    , m_peer_ip(other.m_peer_ip)
    , m_peer_port(other.m_peer_port)
{

}

Address::Address(Address && other)
    : m_host_ip(std::move(other.m_host_ip))
    , m_host_port(other.m_host_port)
    , m_peer_ip(std::move(other.m_peer_ip))
    , m_peer_port(other.m_peer_port)
{
    other.m_host_port = 0;
    other.m_peer_port = 0;
}

} // namespace BoostWeb end

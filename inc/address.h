/********************************************************
 * Description : address
 * Data        : 2018-04-14 18:32:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#ifndef BOOST_WEB_ADDRESS_H
#define BOOST_WEB_ADDRESS_H


#include <string>

namespace BoostWeb { // namespace BoostWeb begin

struct Address
{
    Address();
    Address(const Address &);
    Address(Address &&);

    std::string                                                                 m_host_ip;
    unsigned short                                                              m_host_port;
    std::string                                                                 m_peer_ip;
    unsigned short                                                              m_peer_port;
};

} // namespace BoostWeb end


#endif // BOOST_WEB_ADDRESS_H

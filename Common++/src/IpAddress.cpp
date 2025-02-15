#define LOG_MODULE CommonLogModuleIpUtils

#include "Logger.h"
#include "IpAddress.h"
#include "IpUtils.h"
#include <string.h>
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif


namespace pcpp
{

bool IPAddress::equals(const IPAddress* other) const
{
	if (other == NULL)
		return false;

	if (other->getType() != getType())
		return false;

	if (other->getType() == IPv4AddressType && getType() == IPv4AddressType)
		return *((IPv4Address*)other) == *((IPv4Address*)this);

	if (other->getType() == IPv6AddressType && getType() == IPv6AddressType)
		return *((IPv6Address*)other) == *((IPv6Address*)this);

	return false;
}

IPAddress::Ptr_t IPAddress::fromString(char* addressAsString)
{
	in_addr ip4Addr;
	in6_addr ip6Addr;
	if (inet_pton(AF_INET, addressAsString, &ip4Addr) != 0)
	{
		return IPAddress::Ptr_t(new IPv4Address(addressAsString));
	}
	else if (inet_pton(AF_INET6, addressAsString, &ip6Addr) != 0)
	{
		return IPAddress::Ptr_t(new IPv6Address(addressAsString));
	}

	return IPAddress::Ptr_t();
}

IPAddress::Ptr_t IPAddress::fromString(std::string addressAsString)
{
	return fromString((char*)addressAsString.c_str());
}


IPv4Address IPv4Address::Zero((uint32_t)0);

IPv4Address::IPv4Address(const IPv4Address& other)
{
	m_pInAddr = new in_addr();
	memcpy(m_pInAddr, other.m_pInAddr, sizeof(in_addr));

	strncpy(m_AddressAsString, other.m_AddressAsString, MAX_ADDR_STRING_LEN);
	m_IsValid = other.m_IsValid;
}

IPv4Address::IPv4Address(uint32_t addressAsInt)
{
	m_pInAddr = new in_addr();
	memcpy(m_pInAddr, &addressAsInt, sizeof(addressAsInt));
	if (inet_ntop(AF_INET, m_pInAddr, m_AddressAsString, MAX_ADDR_STRING_LEN) == 0)
		m_IsValid = false;
	else
		m_IsValid = true;
}

IPv4Address::IPv4Address(in_addr* inAddr)
{
	m_pInAddr = new in_addr();
	memcpy(m_pInAddr, inAddr, sizeof(in_addr));
	if (inet_ntop(AF_INET, m_pInAddr, m_AddressAsString, MAX_ADDR_STRING_LEN) == 0)
		m_IsValid = false;
	else
		m_IsValid = true;
}

IPAddress* IPv4Address::clone() const
{
	return new IPv4Address(*this);
}

void IPv4Address::init(const char* addressAsString)
{
	m_pInAddr = new in_addr();
	if (inet_pton(AF_INET, addressAsString , m_pInAddr) == 0)
	{
		m_IsValid = false;
		return;
	}

	strncpy(m_AddressAsString, addressAsString, MAX_IPV4_STRING_LEN-1);
	m_AddressAsString[MAX_IPV4_STRING_LEN - 1] = '\0';
	m_IsValid = true;
}

IPv4Address::~IPv4Address()
{
	delete m_pInAddr;
}

IPv4Address::IPv4Address(const char* addressAsString)
{
	init(addressAsString);
}

IPv4Address::IPv4Address(std::string addressAsString)
{
	init((char*)addressAsString.c_str());
}

uint32_t IPv4Address::toInt() const
{
	uint32_t result;
	memcpy(&result, m_pInAddr, sizeof(uint32_t));
	return result;
}

IPv4Address& IPv4Address::operator=(const IPv4Address& other)
{
	if (m_pInAddr == NULL)
		m_pInAddr = new in_addr();
	memcpy(m_pInAddr, other.m_pInAddr, sizeof(in_addr));

	strncpy(m_AddressAsString, other.m_AddressAsString, MAX_ADDR_STRING_LEN);
	m_IsValid = other.m_IsValid;

	return *this;
}

bool IPv4Address::matchSubnet(const IPv4Address& subnet, const std::string& subnetMask) const
{
	IPv4Address maskAsIpAddr(subnetMask);
	if (!maskAsIpAddr.isValid())
	{
		LOG_ERROR("Subnet mask '%s' is in illegal format", subnetMask.c_str());
		return false;
	}

	int thisAddrAfterMask = toInt() & maskAsIpAddr.toInt();
	int subnetAddrAfterMask = subnet.toInt() & maskAsIpAddr.toInt();
	return (thisAddrAfterMask == subnetAddrAfterMask);
}

bool IPv4Address::matchSubnet(const IPv4Address& subnet, const IPv4Address& subnetMask) const
{
	uint32_t maskAsInt = subnetMask.toInt();
	uint32_t thisAddrAfterMask = toInt() & maskAsInt;
	uint32_t subnetAddrAfterMask = subnet.toInt() & maskAsInt;
	return thisAddrAfterMask == subnetAddrAfterMask;
}


IPv6Address IPv6Address::Zero(std::string("0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0"));

IPv6Address::IPv6Address(const IPv6Address& other)
{
	m_pInAddr = (in6_addr*)new uint8_t[sizeof(in6_addr)];
	memcpy(m_pInAddr, other.m_pInAddr, sizeof(in6_addr));

	strncpy(m_AddressAsString, other.m_AddressAsString, MAX_ADDR_STRING_LEN);
	m_IsValid = other.m_IsValid;
}

IPv6Address::~IPv6Address()
{
	delete[] m_pInAddr;
}

IPAddress* IPv6Address::clone() const
{
	return new IPv6Address(*this);
}

void IPv6Address::init(char* addressAsString)
{
	m_pInAddr = (in6_addr*)new uint8_t[sizeof(in6_addr)];
	if (inet_pton(AF_INET6, addressAsString , m_pInAddr) == 0)
	{
		m_IsValid = false;
		return;
	}

	strncpy(m_AddressAsString, addressAsString, MAX_ADDR_STRING_LEN-1);
	m_AddressAsString[MAX_ADDR_STRING_LEN - 1] = '\0';
	m_IsValid = true;
}

IPv6Address::IPv6Address(uint8_t* addressAsUintArr)
{
	m_pInAddr = (in6_addr*)new uint8_t[sizeof(in6_addr)];
	memcpy(m_pInAddr, addressAsUintArr, sizeof(in6_addr));
	if (inet_ntop(AF_INET6, m_pInAddr, m_AddressAsString, MAX_ADDR_STRING_LEN) == 0)
		m_IsValid = false;
	else
		m_IsValid = true;
}

IPv6Address::IPv6Address(char* addressAsString)
{
	init(addressAsString);
}

IPv6Address::IPv6Address(std::string addressAsString)
{
	init((char*)addressAsString.c_str());
}

IPv6Address::IPv6Address(in6_addr* inAddr)
{
  m_pInAddr = (in6_addr*)new uint8_t[sizeof(in6_addr)];
  memcpy(m_pInAddr, inAddr, sizeof(in6_addr));
  if (inet_ntop(AF_INET6, m_pInAddr, m_AddressAsString, MAX_ADDR_STRING_LEN) == 0)
    m_IsValid = false;
  else
    m_IsValid = true;
}

void IPv6Address::copyTo(uint8_t** arr, size_t& length) const
{
	const size_t addrLen = sizeof(in6_addr);
	length = addrLen;
	(*arr) = new uint8_t[addrLen];
	memcpy((*arr), m_pInAddr, addrLen);
}

void IPv6Address::copyTo(uint8_t* arr) const
{
	memcpy(arr, m_pInAddr, sizeof(in6_addr));
}

bool IPv6Address::matchSubnet(const IPv6Address& subnet, const IPv6Address& subnetMask) const
{

        struct in6_addr thisAfterMask = *m_pInAddr;
#ifdef LINUX
        thisAfterMask.s6_addr32[0] &= subnetMask.m_pInAddr->s6_addr32[0];
        thisAfterMask.s6_addr32[1] &= subnetMask.m_pInAddr->s6_addr32[1];
        thisAfterMask.s6_addr32[2] &= subnetMask.m_pInAddr->s6_addr32[2];
        thisAfterMask.s6_addr32[3] &= subnetMask.m_pInAddr->s6_addr32[3];
#else
        thisAfterMask.__u6_addr.__u6_addr32[0] &= subnetMask.m_pInAddr->__u6_addr.__u6_addr32[0];
        thisAfterMask.__u6_addr.__u6_addr32[1] &= subnetMask.m_pInAddr->__u6_addr.__u6_addr32[1];
        thisAfterMask.__u6_addr.__u6_addr32[2] &= subnetMask.m_pInAddr->__u6_addr.__u6_addr32[2];
        thisAfterMask.__u6_addr.__u6_addr32[3] &= subnetMask.m_pInAddr->__u6_addr.__u6_addr32[3];
#endif
        struct in6_addr subnetAfterMask = *subnet.m_pInAddr;
#ifdef LINUX
        subnetAfterMask.s6_addr32[0] &= subnetMask.m_pInAddr->s6_addr32[0];
        subnetAfterMask.s6_addr32[1] &= subnetMask.m_pInAddr->s6_addr32[1];
        subnetAfterMask.s6_addr32[2] &= subnetMask.m_pInAddr->s6_addr32[2];
        subnetAfterMask.s6_addr32[3] &= subnetMask.m_pInAddr->s6_addr32[3];
#else
        subnetAfterMask.__u6_addr.__u6_addr32[0] &= subnetMask.m_pInAddr->__u6_addr.__u6_addr32[0];
        subnetAfterMask.__u6_addr.__u6_addr32[1] &= subnetMask.m_pInAddr->__u6_addr.__u6_addr32[1];
        subnetAfterMask.__u6_addr.__u6_addr32[2] &= subnetMask.m_pInAddr->__u6_addr.__u6_addr32[2];
        subnetAfterMask.__u6_addr.__u6_addr32[3] &= subnetMask.m_pInAddr->__u6_addr.__u6_addr32[3];
#endif
        return memcmp(&thisAfterMask, &subnetAfterMask, sizeof(in6_addr)) == 0;

}

bool IPv6Address::operator==(const IPv6Address& other) const
{
	return (memcmp(m_pInAddr, other.m_pInAddr, sizeof(in6_addr)) == 0);
}

bool IPv6Address::operator!=(const IPv6Address& other) const
{
	return !(*this == other);
}

IPv6Address& IPv6Address::operator=(const IPv6Address& other)
{
	if (m_pInAddr == NULL)
		m_pInAddr = (in6_addr*)new uint8_t[sizeof(in6_addr)];
	memcpy(m_pInAddr, other.m_pInAddr, sizeof(in6_addr));

	strncpy(m_AddressAsString, other.m_AddressAsString, MAX_ADDR_STRING_LEN);
	m_IsValid = other.m_IsValid;

	return *this;
}

} // namespace pcpp

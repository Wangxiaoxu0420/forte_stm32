

#ifndef LWIP_HDR_PROT_IANA_H
#define LWIP_HDR_PROT_IANA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup iana
 * Hardware types
 */
enum lwip_iana_hwtype {
  /** Ethernet */
  LWIP_IANA_HWTYPE_ETHERNET = 1
};

/**
 * @ingroup iana
 * Port numbers
 * 
 */
enum lwip_iana_port_number {
  /** SMTP */
  LWIP_IANA_PORT_SMTP        = 25,
  /** DHCP server */
  LWIP_IANA_PORT_DHCP_SERVER = 67,
  /** DHCP client */
  LWIP_IANA_PORT_DHCP_CLIENT = 68,
  /** TFTP */
  LWIP_IANA_PORT_TFTP        = 69,
  /** HTTP */
  LWIP_IANA_PORT_HTTP        = 80,
  /** SNTP */
  LWIP_IANA_PORT_SNTP        = 123,
  /** NETBIOS */
  LWIP_IANA_PORT_NETBIOS     = 137,
  /** SNMP */
  LWIP_IANA_PORT_SNMP        = 161,
  /** SNMP traps */
  LWIP_IANA_PORT_SNMP_TRAP   = 162,
  /** HTTPS */
  LWIP_IANA_PORT_HTTPS       = 443,
  /** SMTPS */
  LWIP_IANA_PORT_SMTPS       = 465,
  /** MQTT */
  LWIP_IANA_PORT_MQTT        = 1883,
  /** MDNS */
  LWIP_IANA_PORT_MDNS        = 5353,
  /** Secure MQTT */
  LWIP_IANA_PORT_SECURE_MQTT = 8883
};

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_PROT_IANA_H */

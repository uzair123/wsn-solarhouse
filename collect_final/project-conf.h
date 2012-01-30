// Configuration of the Contiki Netstack

//
#ifdef NETSTACK_CONF_NETWORK
#undef NETSTACK_CONF_NETWORK
#endif
#define NETSTACK_CONF_NETWORK					rime_driver

// Radio Duty Cycling (RDC) channel check rate
#ifdef NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE
#undef NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE
#endif
#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE	128
//#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE	16

// radio duty cycling (RDC) driver: (nullrdc_driver | contikimac_driver |
#ifdef NETSTACK_CONF_RDC
#undef NETSTACK_CONF_RDC
#endif
//#define NETSTACK_CONF_RDC						nullrdc_driver
#define NETSTACK_CONF_RDC						contikimac_driver


// MAC driver: (nullmac_driver | csma_driver )
#ifdef NETSTACK_CONF_MAC
#undef NETSTACK_CONF_MAC
#endif
//#define NETSTACK_CONF_MAC						nullmac_driver
#define NETSTACK_CONF_MAC						csma_driver

// MAC framer, responsible for constructing and parsing the header in MAC frames (framer_nullmac | framer_802154 )
// Note 2011/07/27: With framer_nullmac, messages towards the root node with frag_unicast are NOT sent at all!!
#ifdef NETSTACK_CONF_FRAMER
#undef NETSTACK_CONF_FRAMER
#endif
//#define NETSTACK_CONF_FRAMER					framer_nullmac
#define NETSTACK_CONF_FRAMER					framer_802154


// Enables or disables scopes fragmentation
#define FRAGMENTATION_ENABLED

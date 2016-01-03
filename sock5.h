#ifndef _CS_SOCK5_H_
#define _CS_SOCK5_H_

#define S5_VERSION 0x5

/*
 * negotiation for the authentication method
 * negotiation request
 * +----+----------+----------+
 * |VER | NMETHODS | METHODS  |
 * +----+----------+----------+
 * | 1  |    1     | 1 to 255 |
 * +----+----------+----------+
 * negotiation reply
 * +----+--------+
 * |VER | METHOD |
 * +----+--------+
 * | 1  |   1    |
 * +----+--------+
 *
 * request
 * +----+-----+-------+------+----------+----------+
 * |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
 * +----+-----+-------+------+----------+----------+
 * | 1  |  1  | X'00' |  1   | Variable |    2     |
 * +----+-----+-------+------+----------+----------+
 * reply
 * +----+-----+-------+------+----------+----------+
 * |VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
 * +----+-----+-------+------+----------+----------+
 * | 1  |  1  | X'00' |  1   | Variable |    2     |
 * +----+-----+-------+------+----------+----------+
 *
 */

/* authentication method */
#define S5_AUTH_METHOD_NO_AUTH 		0x00	/* NO AUTHENTICATION REQUIRED */
#define S5_AUTH_METHOD_GSSAPI 		0x01	/* GSSAPI */
#define S5_AUTH_METHOD_USERNAME_PASS 	0x02	/* USERNAME/PASSWORD */
/*
 *  X'03' to X'7F' IANA ASSIGNED
 *  X'80' to X'FE' RESERVED FOR PRIVATE METHODS
 */
#define S5_AUTH_METHOD_NO_ACCEPTABLE 	0xff	/* NO ACCEPTABLE METHODS */

/* command */
#define S5_CMD_CONNECT 	0x01
#define S5_CMD_BIND 	0x02
#define S5_CMD_UDP_ASS 	0x03


/* address type */
#define S5_ATYPE_IPV4		0x01
#define S5_ATYPE_DOMAIN		0x02
#define S5_ATYPE_IPV6		0x03

/* reply field */
#define S5_REP_SUCCESSED		0x00
#define S5_REP_GENERAL_SERVER_FAILURE	0x01
#define S5_REP_CONNECT_NOT_ALLOWED	0x02
#define S5_REP_NETWORK_UNREACHABLE	0x03
#define S5_REP_HOST_UNREACHABLE		0x04
#define S5_REP_CONNECTION_REFUSED	0x05
#define S5_REP_TTL_EXPIRED		0x06
#define S5_REP_COMMAND_NOT_SUPPORTED	0x07
#define S5_REP_ADDRESS_NOT_SUPPORTED	0x08
/* X'09' to X'FF' unassigned */

#define S5_RSV_FILED 0x00 /* Fields marked RESERVED (RSV) must be set to X'00'. */


#endif /* define _CS_SOCK5_H_ */

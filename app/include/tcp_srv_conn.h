#ifndef __TCP_SERV_CONN_H__
#define __TCP_SERV_CONN_H__
/***********************************
 * FileName: tcp_srv_conn.h
 * Tcp �������� ��� ESP8266
 * PV` ver1.0 20/12/2014
 ***********************************/

#include "user_interface.h"
#include "os_type.h"

#include "lwip/err.h"

enum srvconn_state {
    SRVCONN_NONE =0,
    SRVCONN_CLOSEWAIT,  // ������� ��������
    SRVCONN_CLIENT, // ��������� ���������� (������)
    SRVCONN_LISTEN, // ���������� �������, ���� rx ��� tx
    SRVCONN_CONNECT, // ���������� �������, ���� rx ��� tx
    SRVCONN_CLOSED // ���������� �������
};

// ���-�� ������������ �������� ���������� �� ���������
#ifndef TCP_SRV_MAX_CONNECTIONS
 #define TCP_SRV_MAX_CONNECTIONS 10
#endif

// ���� ������� �� ���������
#ifndef TCP_SRV_SERVER_PORT
 #define TCP_SRV_SERVER_PORT 80
#endif

// ������� ������ ���������� ���� �� ���������
#ifndef DEBUGSOO
 #define DEBUGSOO 2
#endif

// ����� (���), �� ���������, �������� ������� (�������� ������) �� �������, �� ����-�������� ����������,
// ��� = 0 ���������� �� ��� 5 ���.
#ifndef TCP_SRV_RECV_WAIT
 #define TCP_SRV_RECV_WAIT  5
#endif
// ����� (���), �� ���������, �� ����-�������� ���������� ����� ������ ��� ��������,
// ��� = 0 ���������� �� ��� 5 ���.
#ifndef TCP_SRV_END_WAIT
 #define TCP_SRV_END_WAIT  5
#endif

// ����� (� 1/4 ���) �������� �� ��������������� �������� ���������� ����� ������� FIN.
#define TCP_SRV_CLOSE_WAIT 120 // 120/4 = 30 ���

// ����������� ������ heap �� ���������, ��� �������� ������ ����������, ��� = 0 ���������� �� ���:
#define TCP_SRV_MIN_HEAP_SIZE 14528  // ����� ������� �� 6Kb

// ������������ ������ ����������� ������ � heap ��� ������ ������
#ifndef TCP_SRV_SERVER_MAX_RXBUF
 #define TCP_SRV_SERVER_MAX_RXBUF (TCP_MSS*3) // 1460*2=2920, 1460*3=4380, 1460*4=5840
#endif

// ������ ����������� ������ � heap ��� �������� ��� ���������������� ������
#ifndef TCP_SRV_SERVER_DEF_TXBUF
 #define TCP_SRV_SERVER_DEF_TXBUF (TCP_MSS*3) // 1460*2=2920, 1460*3=4380, 1460*4=5840
#endif

#define ID_CLIENTS_PORT 1
#define TCP_CLIENT_NEXT_CONNECT_MS		30000 // ����� 3 ���
#define TCP_CLIENT_MAX_CONNECT_RETRY	7 // 7 ��� ����� 3 ���

//--------------------------------------------------------------------------
// ��������� ����������
//
typedef struct t_tcpsrv_conn_flags  {
	uint16 client:				1; //0001 ������ ���������� �� ������, � ������!
	uint16 pcb_time_wait_free:	1; //0002 ����������� pcb ��� ������ disconnect() (����� pcb TIME_WAIT 60 ��� http://www.serverframework.com/asynchronousevents/2011/01/time-wait-and-its-design-implications-for-protocols-and-scalable-servers.html)
	uint16 nagle_disabled: 		1; //0004 ���������� nagle
	uint16 rx_buf: 				1; //0008 ����� � �����, ������������ ������ ���������� �������� ���� TCP
	uint16 rx_null:				1; //0010 ���������� ������ func_received_data() � ����� � null (��������������� ������������� ��� ������ tcpsrv_disconnect())
	uint16 tx_null:				1; //0020 ���������� ������ func_sent_callback() � �������� � null (��������������� ������������� ��� ������ tcpsrv_disconnect())
	uint16 wait_sent:			1; //0040 ������ ����������/������������� �������� �� lwip
	uint16 busy_bufo:			1; //0080 ���� ��������� bufo
	uint16 busy_bufi:			1; //0100 ���� ��������� bufi
	// ����� ���� ���� �� ����������� � ������ tcp_srv_conn
	uint16 user_flg1:			1; //0200 ��� ���� �������� ������� ���� (����������� � tcp_terminal.c)
	uint16 user_flg2:			1; //0400 ��� ���� �������� ������� ���� (���� ��������)
	uint16 user_option1:		1; //0800 ��� ���� �������� ��������� ���������� (����������� ��� hexdump � web_int_callbacks.c)
	uint16 user_option2:		1; //1000 ��� ���� �������� ��������� ���������� (���� ��������)
} __attribute__((packed)) tcpsrv_conn_flags;

typedef struct t_TCP_SERV_CONN {
	volatile tcpsrv_conn_flags flag;//+0 ����� ���������
	enum srvconn_state state;		//+4 ���������
	struct t_TCP_SERV_CFG *pcfg;  	//+8 ��������� �� ������� ��������� �������
	uint16 recv_check;   			//+12 ���� ����� ���������� � tcpsrv_poll
	uint16 remote_port;  			//+16 ����� ����� �������
	union {              			//+20 ip �������
	  uint32 dw;
	  uint8  b[4];
	} remote_ip;
	struct t_TCP_SERV_CONN *next; 	//+24 ��������� �� ��������� ���������
	struct tcp_pcb *pcb; 			//+28 ��������� �� pcb � Lwip
	os_timer_t ptimer;  			//+32 ������������ ��� ������� �������� � ������ tcpsrv_close_cb
	uint8 *pbufo; 					//+36 ��������� �� ������� � ������������� �������
	uint8 *ptrtx; 					//+40 ��������� �� ��� �� ���������� ������
	uint8 *pbufi;   				//+44 ��������� �� ������� ������ � ������������ �������
	uint16 sizeo; 					//+48 ������ ������ ��������
	uint16 sizei; 					//+52 ������ ��������� ������
	uint16 cntro; 					//+56 ���-�� ������������ ���� � ������ ��������
	uint16 cntri; 					//+60 ���-�� �� ������������ ���� � ������ ������
	uint16 unrecved_bytes; 			//+64 ������������ ��� ������ ���������� TCP WIN / This can be used to throttle data reception
	// ����� ���� ���������� �� ����������� � ������ tcp_srv_conn
	uint8 *linkd; 					//+68 ��������� �� ������������ ������ ������������ (��� �������� ���������� ���������� os_close(linkd), ���� linkd != NULL;
} TCP_SERV_CONN;


//--------------------------------------------------------------------------
// ���������� ������� ������������ (calback-�)
//
typedef void (*func_disconect_calback)(TCP_SERV_CONN *ts_conn); // ���������� �������
typedef err_t (*func_listen)(TCP_SERV_CONN *ts_conn); // ����� ������
typedef err_t (*func_received_data)(TCP_SERV_CONN *ts_conn); // ������� ����� ts_conn->sizei ����, ����� � ������ �� ts_conn->pbufi, �� ������ ����������� ������������ ts_conn->cntri;
typedef err_t (*func_sent_callback)(TCP_SERV_CONN *ts_conn); // ���� ������ �������

//--------------------------------------------------------------------------
// ��������� ������������ tcp �������
//
typedef struct t_TCP_SERV_CFG {
	    struct t_tcpsrv_conn_flags flag;  // ��������� ����� ��� ����������
        uint16 port;		// ����� ����� (=1 - client)
        uint16 max_conn;	// ������������ ���-�� ������������� ����������, �� ��������� = 5.
        uint16 conn_count;	// ���-�� ������� ����������, ��� ������������� ����������� 0
        uint16 min_heap;	// ����������� ������ heap ��� �������� ������ ����������, ��� = 0 ���������� �� 8192.
        uint16 time_wait_rec;	// ����� (���) �������� ������� (�������� ������) �� �������, �� ����-�������� ����������, �� ��������� TCP_SRV_RECV_WAIT ��� .
        uint16 time_wait_cls;	// ����� (���) �� ����-�������� ���������� ����� ������ ��� ��������, �� ��������� TCP_SRV_END_WAIT ���.
        TCP_SERV_CONN * conn_links;	// ��������� �� ������� �������� ����������, ��� ������������� ��� ��������� �������� ���������� = NULL
        struct tcp_pcb *pcb;	// ��������� pcb [LISTEN] ���� ������, ����� NULL
        func_disconect_calback func_discon_cb;	// ������� ���������� ����� �������� ����������, ���� = NULL - �� ����������
        func_listen func_listen;			// ������� ���������� ��� ������������� ������� ��� �������� � �������, ���� = NULL - �� ����������
        func_sent_callback func_sent_cb;	// ������� ���������� ����� �������� ������ ��� ������� ����� � ip ����� ��� �������� �������� ������, ���� = NULL - �� ���������� (+��. �����)
        func_received_data func_recv;		// ������� ���������� ��� ������ ������, ���� = NULL - �� ���������� (+��. �����)
        struct t_TCP_SERV_CFG *next;		// ��������� ��������� ��������� �������/�������
}TCP_SERV_CFG;
//--------------------------------------------------------------------------
// ������
//
extern TCP_SERV_CFG *phcfg; // ��������� �� ������� TCP_SERV_CFG (������������ ��������)
//--------------------------------------------------------------------------
// �������
//
err_t tcpsrv_int_sent_data(TCP_SERV_CONN * ts_conn, uint8 *psent, uint16 length) ICACHE_FLASH_ATTR; // �������� length ���� (��������� ������� - ������� ��������)
void tcpsrv_disconnect(TCP_SERV_CONN * ts_conn) ICACHE_FLASH_ATTR; // ������� ����������
void tcpsrv_print_remote_info(TCP_SERV_CONN *ts_conn) ICACHE_FLASH_ATTR; // ������� remote_ip:remote_port [conn_count] os_printf("srv x.x.x.x:x [n] ")
TCP_SERV_CFG * tcpsrv_port2pcfg(uint16 portn) ICACHE_FLASH_ATTR; // ����� ��������� ������� �� ������ �����
void tcpsrv_unrecved_win(TCP_SERV_CONN *ts_conn) ICACHE_FLASH_ATTR; // ������������ ������ TCP WIN, ���� ������������ ������ ���������� �������� ���� TCP

void tcpsrv_disconnect_calback_default(TCP_SERV_CONN *ts_conn) ICACHE_FLASH_ATTR;
err_t tcpsrv_listen_default(TCP_SERV_CONN *ts_conn) ICACHE_FLASH_ATTR;
err_t tcpsrv_sent_callback_default(TCP_SERV_CONN *ts_conn) ICACHE_FLASH_ATTR;
err_t tcpsrv_received_data_default(TCP_SERV_CONN *ts_conn) ICACHE_FLASH_ATTR;

TCP_SERV_CFG *tcpsrv_init(uint16 portn) ICACHE_FLASH_ATTR;
#define tcpsrv_init_client() tcpsrv_init(ID_CLIENTS_PORT)
err_t tcpsrv_start(TCP_SERV_CFG *p) ICACHE_FLASH_ATTR;
err_t tcpsrv_client_start(TCP_SERV_CFG * p, uint32 remote_ip, uint16 remote_port) ICACHE_FLASH_ATTR;
err_t tcpsrv_close(TCP_SERV_CFG *p) ICACHE_FLASH_ATTR;
err_t tcpsrv_close_port(uint16 portn) ICACHE_FLASH_ATTR;
err_t tcpsrv_close_all(void) ICACHE_FLASH_ATTR;

#endif // __TCP_SERV_CONN_H__

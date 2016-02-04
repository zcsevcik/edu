/*
 * modbus_pdu.h:   TQS1-TQS3 adapter.
 *
 * Date:           $Format:%aD$
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 *
 * This file is part of tqs-adapter.
 */

/* ==================================================================== */
/* MODBUS Application Protocol Specification V1.1b                      */
/*     -- http://modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf */
/* ==================================================================== */
#ifndef MODBUS_PDU_H
# define MODBUS_PDU_H

# include <stdint.h>

/* ==================================================================== */
/*      F  U  N  C  T  I  O  N     C  O  D  E  S                        */
/* ==================================================================== */
enum {
    /** Read Coils */
    MB_FR_COIL = 0x01,

    /** Read Discrete Inputs */
    MB_FR_INPUT = 0x02,

    /** Read Holding Registers */
    MB_FR_HOLDING_REG = 0x03,

    /** Read Input Registers */
    MB_FR_INPUT_REG = 0x04,

    /** Write Single Coil */
    MB_FW_COIL = 0x05,

    /** Write Single Register */
    MB_FW_REG = 0x06,

    /** Read Exception Status */
    MB_FR_EXCEP = 0x07,

    /** Diagnostics */
    MB_FDIAG = 0x08,

    /** Get Comm Event Counter */
    MB_FCOM_EVENT_CTR = 0x0B,

    /** Get Comm Event Log */
    MB_FCOM_EVENT_LOG = 0x0C,

    /** Write Multiple Coils */
    MB_FW_COILS = 0x0F,

    /** Write Multiple registers */
    MB_FW_REGS = 0x10,

    /** Report Slave ID */
    MB_FR_SLAVEID = 0x11,

    /** Read File Record */
    MB_FR_FILE = 0x14,

    /** Write File Record */
    MB_FW_FILE = 0x15,

    /** Mask Write Register */
    MB_FMASK_WREG = 0x16,

    /** Read/Write Multiple registers */
    MB_FRW_REGS = 0x17,

    /** Read FIFO Queue */
    MB_FR_FIFO = 0x18,

    /** Encapsulated Interface Transport */
    MB_FMEI = 0x2B,
};

/* ==================================================================== */
/*      S  U  B  -  F  U  N  C  T  I  O  N     C  O  D  E  S            */
/* ==================================================================== */
enum {
    /** Return Query Data */
    MB_FDIAG_QDATA = 0x0000,

    /** Restart Communications Option */
    MB_FDIAG_RST_COM_OPT = 0x0001,

    /** Return Diagnostic Register */
    MB_FDIAG_REG = 0x0002,

    /** Change ASCII Input Delimiter */
    MB_FDIAG_CHNG_DELIM = 0x0003,

    /** Force Listen Only Mode */
    MB_FDIAG_FORCE_LISTEN = 0x0004,

    /** Clear Counters and Diagnostic Register */
    MB_FDIAG_CLR_REG_CTR = 0x000A,

    /** Return Bus Message Count */
    MB_FDIAG_CTR_MSG = 0x000B,

    /** Return Bus Communication Error Count */
    MB_FDIAG_CTR_COM_ERR = 0x000C,

    /** Return Bus Exception Error Count */
    MB_FDIAG_CTR_EXCEP_ERR = 0x000D,

    /** Return Slave Message Count */
    MB_FDIAG_CTR_SLAVE_MSG = 0x000E,

    /** Return Slave No Response Count */
    MB_FDIAG_CTR_NORESP = 0x000F,

    /** Return Slave NAK Count */
    MB_FDIAG_CTR_NAK = 0x0010,

    /** Return Slave Busy Count */
    MB_FDIAG_CTR_BUSY = 0x0011,

    /** Return Bus Character Overrun Count */
    MB_FDIAG_CTR_OR = 0x0012,

    /** Clear Overrun Counter and Flag */
    MB_FDIAG_CLR_OR = 0x0014,
};

enum {
    /** CANopen General Reference Request and Response PDU */
    MB_FMEI_CANOPEN = 0x0D,

    /** Read Device Identification */
    MB_FMEI_DEVICEID = 0x0E,
};

/* ==================================================================== */
/*      D  E  V  I  C  E     I  D  E  N  T  I  F  I  C  A  T  I  O  N 	*/
/* ==================================================================== */
enum {
    /** Vendor Name */
    MB_DEVICEID_VENDOR = 0x00,

    /** Product Code */
    MB_DEVICEID_CODE = 0x01,

    /** Version */
    MB_DEVICEID_VERSION = 0x02,

    /** Url */
    MB_DEVICEID_URL = 0x03,

    /** Product Name */
    MB_DEVICEID_NAME = 0x04,

    /** Model Name */
    MB_DEVICEID_MODEL = 0x05,

    /** User Application Name */
    MB_DEVICEID_APPNAME = 0x06,

    /** Device Dependant */
    MB_DEVICEID_CUSTOM = 0x80,
};

enum {
    /** basic identification (stream access only) */
    MB_DEVICEID_BASIC = 0x01,

    /** regular identification (stream access only) */
    MB_DEVICEID_REGULAR = 0x02,

    /** extended identification (stream access only) */
    MB_DEVICEID_EXTENDED = 0x03,

    /** specific identification (individual access only) */
    MB_DEVICEID_SPECIFIC = 0x04,

    /** basic identification (stream access and individual access) */
    MB_DEVICEID_BASIC_ = 0x81,

    /** regular identification (stream access and individual access) */
    MB_DEVICEID_REGULAR_ = 0x82,

    /** extended identification(stream access and individual access) */
    MB_DEVICEID_EXTENDED_ = 0x83,
};

/* ==================================================================== */
/*      E  X  C  E  P  T  I  O  N     C  O  D  E  S                     */
/* ==================================================================== */
enum {
    /**
     * ILLEGAL FUNCTION
     *
     * The function code received in the query is not an
     * allowable action for the server (or slave). This
     * may be because the function code is only
     * applicable to newer devices, and was not
     * implemented in the unit selected. It could also
     * indicate that the server (or slave) is in the wrong
     * state to process a request of this type, for
     * example because it is unconfigured and is being
     * asked to return register values.
     */
    MB_EFUNC = 0x01,

    /**
     * ILLEGAL DATA ADDRESS
     *
     * The data address received in the query is not an
     * allowable address for the server (or slave). More
     * specifically, the combination of reference number
     * and transfer length is invalid. For a controller with
     * 100 registers, the PDU addresses the first
     * register as 0, and the last one as 99. If a request
     * is submitted with a starting register address of 96
     * and a quantity of registers of 4, then this request
     * will successfully operate (address-wise at least)
     * on registers 96, 97, 98, 99. If a request is
     * submitted with a starting register address of 96
     * and a quantity of registers of 5, then this request
     * will fail with Exception Code 0x02 “Illegal Data
     * Address” since it attempts to operate on registers
     * 96, 97, 98, 99 and 100, and there is no register
     * with address 100.
     */
    MB_EADDR = 0x02,

    /**
     * ILLEGAL DATA VALUE
     *
     * A value contained in the query data field is not an
     * allowable value for server (or slave). This
     * indicates a fault in the structure of the remainder
     * of a complex request, such as that the implied
     * length is incorrect. It specifically does NOT mean
     * that a data item submitted for storage in a register
     * has a value outside the expectation of the
     * application program, since the MODBUS protocol
     * is unaware of the significance of any particular
     * value of any particular register.
     */
    MB_EVAL = 0x03,

    /**
     * SLAVE DEVICE FAILURE
     *
     * An unrecoverable error occurred while the server
     * (or slave) was attempting to perform the
     * requested action.
     */
    MB_EDEVFAIL = 0x04,

    /**
     * ACKNOWLEDGE
     *
     * Specialized use in conjunction with programming
     * commands.
     * The server (or slave) has accepted the request
     * and is processing it, but a long duration of time
     * will be required to do so. This response is
     * returned to prevent a timeout error from occurring
     * in the client (or master). The client (or master)
     * can next issue a Poll Program Complete message
     * to determine if processing is completed.
     */
    MB_EACK = 0x05,

    /**
     * SLAVE DEVICE BUSY
     *
     * Specialized use in conjunction with programming
     * commands.
     * The server (or slave) is engaged in processing a
     * long–duration program command. The client (or
     * master) should retransmit the message later when
     * the server (or slave) is free.
     */
    MB_EDEVBUSY = 0x06,

    /**
     * MEMORY PARITY ERROR
     *
     * Specialized use in conjunction with function codes
     * 20 and 21 and reference type 6, to indicate that
     * the extended file area failed to pass a consistency
     * check.
     * The server (or slave) attempted to read record
     * file, but detected a parity error in the memory.
     * The client (or master) can retry the request, but
     * service may be required on the server (or slave)
     * device.
     */
    MB_EMEMPAR = 0x08,

    /**
     * GATEWAY PATH UNAVAILABLE
     *
     * Specialized use in conjunction with gateways,
     * indicates that the gateway was unable to allocate
     * an internal communication path from the input
     * port to the output port for processing the request.
     * Usually means that the gateway is misconfigured
     * or overloaded.
     */
    MB_EGATEPATH = 0x0A,

    /**
     * GATEWAY TARGET DEVICE FAILED TO RESPOND
     *
     * Specialized use in conjunction with gateways,
     * indicates that no response was obtained from the
     * target device. Usually means that the device is
     * not present on the network.
     */
    MB_EGATETRGT = 0x0B,
};

/* ==================================================================== */
/*      P  R  O  T  O  C  O  L     D  A  T  A     U  N  I  T            */
/* ==================================================================== */
/**
 * MODBUS Request PDU
 */
struct mb_req_pdu {
    uint8_t function_code;
    uint8_t request_data[];
};

/**
 * MODBUS Response PDU
 */
struct mb_rsp_pdu {
    uint8_t function_code;
    uint8_t response_data[];
};

/**
 * MODBUS Exception Response PDU
 */
struct mb_excep_rsp_pdu {
    uint8_t function_code;
    uint8_t exception_code;
};

/* ==================================================================== */
/*      P  U  B  L  I  C     F  U  N  C  T  I  O  N  S                  */
/* ==================================================================== */
typedef struct {
    uint16_t address;
    uint16_t quantity;
} mb_req_r_coil,
    mb_req_r_input,
    mb_req_r_holding_reg,
    mb_req_r_input_reg, mb_rsp_w_coils, mb_rsp_w_regs;

typedef struct {
    uint8_t cByte;
    union {
        uint8_t cValues[];
        uint16_t wValues[];
    };
} mb_rsp_r_coil, mb_rsp_r_input, mb_rsp_r_holding_reg, mb_rsp_r_input_reg,
    mb_rsp_rw_regs;

typedef struct {
    uint16_t address;
    uint16_t value;
} mb_req_w_coil, mb_req_w_reg, mb_rsp_w_coil, mb_rsp_w_reg;

typedef struct {
    uint8_t data;
} mb_rsp_r_excep;

typedef struct {
    uint16_t subfunction_code;
    uint16_t data[];
} mb_req_diag, mb_rsp_diag;

typedef void mb_req_com_event_ctr;
typedef struct {
    uint16_t status;
    uint16_t count;
} mb_rsp_com_event_ctr;

typedef void mb_req_com_event_log;
typedef struct {
    uint8_t cByte;
    uint16_t status;
    uint16_t count_event;
    uint16_t count_message;
    uint8_t events[];
} mb_rsp_com_event_log;

typedef struct {
    uint16_t address;
    uint16_t quantity;
    uint8_t cByte;
    union {
        uint8_t cValues[];
        uint16_t wValues[];
    };
} mb_req_w_coils, mb_req_w_regs;

typedef void mb_req_slaveid;
typedef struct {
    uint8_t cByte;
    uint8_t slaveid;
    uint8_t running;
    uint8_t data[];
} mb_rsp_slaveid;

typedef struct {
    uint8_t reftype;
    uint16_t filenum;
    uint16_t recnum;
    uint16_t reclen;
} _mb_rfile_subreq;

typedef struct {
    uint8_t cByte;
    _mb_rfile_subreq subreq[];
} mb_req_r_file;

typedef struct {
    uint8_t len;
    uint8_t reftype;
    uint16_t data[];
} _mb_rfile_subrsp;

typedef struct {
    uint8_t cByte;
    _mb_rfile_subrsp subrsp[];
} mb_rsp_r_file;

typedef struct {
    uint8_t reftype;
    uint16_t filenum;
    uint16_t recnum;
    uint16_t reclen;
    uint16_t data[];
} _mb_wfile_sub;

typedef struct {
    uint8_t cByte;
    _mb_wfile_sub sub[];
} mb_w_file;

typedef struct {
    uint16_t address;
    uint16_t mask_and;
    uint16_t mask_or;
} mb_req_mask_wreg, mb_rsp_mask_wreg;

typedef struct {
    uint16_t read_address;
    uint16_t read_quantity;
    uint16_t write_address;
    uint16_t write_quantity;
    uint8_t write_cByte;
    uint16_t write_values[];
} mb_req_rw_regs;

typedef struct {
    uint16_t address;
} mb_req_r_fifo;

typedef struct {
    uint16_t cByte;
    uint16_t count;
    uint16_t values[];
} mb_rsp_r_fifo;

typedef struct {
    uint8_t type;
    uint8_t data[];
} mb_mei;

/* ==================================================================== */
/*      M  E  I     D  E  V  I  C  E  I  D                              */
/* ==================================================================== */
typedef struct {
    uint8_t code;
    uint8_t oid;
} mb_req_mei_deviceid;

typedef struct {
    uint8_t oid;
    uint8_t cByte;
    uint8_t data[];
} _mb_mei_object;

typedef struct {
    uint8_t code;
    uint8_t conformity_lvl;
    uint8_t more_follows;
    uint8_t next_oid;
    uint8_t lObjects;
    _mb_mei_object objects[];
} mb_rsp_mei_deviceid;

#endif                          //MODBUS_PDU_H

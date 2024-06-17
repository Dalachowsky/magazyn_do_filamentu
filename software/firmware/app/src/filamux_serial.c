
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/crc.h>
#include <filamux_proto.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(serial, LOG_LEVEL_INF);

#define MAX_MSG_SIZE       64
#define FRAME_START_MARKER 0x7E

#define UART_NODE DT_ALIAS(filamux_serial)
static const struct device *const serial_dev = DEVICE_DT_GET(UART_NODE);

static const int start_field_pos = 0;
static const int type_field_pos = 1;
static const int length_field_pos = 2;
static const int data_field_pos = 3;

enum serial_state {
	FILAMUX_SERIAL_IDLE = 1,
	FILAMUX_SERIAL_RECEIVING,
	FILAMUX_SERIAL_RECEIVED,
};

static uint8_t rx_buf[MAX_MSG_SIZE]; // Receive buffer
static uint8_t rx_msg[MAX_MSG_SIZE]; // Received message
static uint8_t tx_buf[MAX_MSG_SIZE];
static int rx_buf_pos;
static int msg_len;
static int payload_size;
static int remaining_bytes;
static enum serial_state rx_state;

struct filamux *filamux = NULL;

static void serial_cb(const struct device *dev, void *user_data)
{
	if (!uart_irq_update(dev)) {
		return;
	}
	if (!uart_irq_rx_ready(dev)) {
		return;
	}

	if (rx_state == FILAMUX_SERIAL_RECEIVED) {
		return;
	}
	rx_state = FILAMUX_SERIAL_RECEIVING;

	uint8_t c;
	while (uart_fifo_read(dev, &c, 1) == 1) {
		LOG_DBG("0x%02X | %d", c, c);
		if (rx_buf_pos < MAX_MSG_SIZE - 1) {
			/* Byte received */
			if (rx_buf_pos == start_field_pos) {
				/* Check for frame start */
				if (c == FRAME_START_MARKER) {
					/* Frame start detected */
					LOG_DBG("Frame start");
					rx_buf[rx_buf_pos++] = c;
				}
			} else if (rx_buf_pos == length_field_pos) {
				/* Length field */
				remaining_bytes = c;
				LOG_DBG("Payload length: %d", remaining_bytes);
				rx_buf[rx_buf_pos++] = c;
			} else {
				/* Data or CRC field */
				rx_buf[rx_buf_pos++] = c;
			}
		}
		/* Bytes exceeding the buffer are dropped */
		remaining_bytes--;
		LOG_DBG("Remaining bytes: %d", remaining_bytes);
		if (rx_buf_pos > length_field_pos && remaining_bytes < 0) {
			/* Full frame received */
			memcpy(rx_msg, rx_buf, rx_buf_pos);
			msg_len = rx_buf_pos;
			payload_size = msg_len - data_field_pos - 2;
			rx_state = FILAMUX_SERIAL_RECEIVED;
			rx_buf_pos = 0;
			remaining_bytes = 0;
		}
	}
}

static void send_frame(uint8_t length)
{
	tx_buf[start_field_pos] = FRAME_START_MARKER;
	tx_buf[length_field_pos] = length;
	uint16_t crc = crc16_ccitt(0, &tx_buf[data_field_pos], length);
	tx_buf[data_field_pos + length] = (crc >> 8);
	tx_buf[data_field_pos + length + 1] = crc & 0xFF;

	LOG_DBG("Transmitting frame");
	LOG_HEXDUMP_DBG(tx_buf, data_field_pos + length + 2, "Frame:");
	for (int i = 0; i < data_field_pos + length + 2; i++) {
		uart_poll_out(serial_dev, tx_buf[i]);
	}
}

static void handle_set_spool_params()
{
	LOG_INF("SetSpoolParamsReq received");
	int ret;
	Filamux__SetSpoolParamsReq *req =
		filamux__set_spool_params_req__unpack(NULL, payload_size, &rx_msg[data_field_pos]);
	Filamux__SetSpoolParamsRes res = FILAMUX__SET_SPOOL_PARAMS_RES__INIT;

	// TODO
	res.ok = true;

	size_t len = filamux__set_spool_params_res__pack(&res, &tx_buf[data_field_pos]);
	if (len > MAX_MSG_SIZE) {
		LOG_ERR("Cannot transmit frame. Too long: %d", len);
		return;
	}
	tx_buf[type_field_pos] = FILAMUX__MESSAGE_TYPE__MSG_SET_SPOOL_PARAMS;
	send_frame(len);

	filamux__set_spool_params_req__free_unpacked(req, NULL);
}

static void handle_set_target_spool()
{
	LOG_INF("SetTargetSpoolReq received");
	int ret;
	Filamux__SetTargetSpoolReq *req =
		filamux__set_target_spool_req__unpack(NULL, payload_size, &rx_msg[data_field_pos]);
	Filamux__SetTargetSpoolRes res = FILAMUX__SET_TARGET_SPOOL_RES__INIT;

	res.ok = filamux_set_target_spool(filamux, req->index) == 0;

	size_t len = filamux__set_target_spool_res__pack(&res, &tx_buf[data_field_pos]);
	if (len > MAX_MSG_SIZE) {
		LOG_ERR("Cannot transmit frame. Too long: %d", len);
		return;
	}
	tx_buf[type_field_pos] = FILAMUX__MESSAGE_TYPE__MSG_SET_TARGET_SPOOL;
	send_frame(len);

	filamux__set_target_spool_req__free_unpacked(req, NULL);
}

static void handle_extruder_feed()
{
	LOG_INF("ExtruderFeedReq received");
	int ret;
	Filamux__ExtruderFeedReq *req =
		filamux__extruder_feed_req__unpack(NULL, payload_size, &rx_msg[data_field_pos]);
	Filamux__ExtruderFeedRes res = FILAMUX__EXTRUDER_FEED_RES__INIT;

	res.ok = filamux_feed_extruder(filamux, req->speed, req->distance);

	size_t len = filamux__extruder_feed_res__pack(&res, &tx_buf[data_field_pos]);
	if (len > MAX_MSG_SIZE) {
		LOG_ERR("Cannot transmit frame. Too long: %d", len);
		return;
	}
	tx_buf[type_field_pos] = FILAMUX__MESSAGE_TYPE__MSG_EXTRUDER_FEED;
	send_frame(len);

	filamux__extruder_feed_req__free_unpacked(req, NULL);
}

static void handle_extruder_gcode()
{
	LOG_INF("ExtruderGcodeReq received");
	int ret;
	Filamux__ExtruderGCodeReq *req =
		filamux__extruder_gcode_req__unpack(NULL, payload_size, &rx_msg[data_field_pos]);
	Filamux__ExtruderGCodeRes res = FILAMUX__EXTRUDER_GCODE_RES__INIT;

	char gcode[256];
	memcpy(gcode, req->gcode, payload_size);
	LOG_INF("Gcode: %s", gcode);

	res.status = FILAMUX__EXTRUDER_GCODE_RES__STATUS__OK;

	size_t len = filamux__extruder_gcode_res__pack(&res, &tx_buf[data_field_pos]);
	if (len > MAX_MSG_SIZE) {
		LOG_ERR("Cannot transmit frame. Too long: %d", len);
		return;
	}
	tx_buf[type_field_pos] = FILAMUX__MESSAGE_TYPE__MSG_EXTRUDER_GCODE;
	send_frame(len);

	filamux__extruder_gcode_req__free_unpacked(req, NULL);
}

static void filamux_serial_process_msg()
{
	if (msg_len < data_field_pos + 2) {
		LOG_WRN("Frame dropped. Length too short: %d", msg_len);
		return;
	}

	// if (crc16_ccitt(0, &rx_msg[data_field_pos], msg_len - data_field_pos) != 0) {
	// 	LOG_WRN("Received frame dropped - crc error");
	// 	return;
	// }

	Filamux__MessageType type = (Filamux__MessageType)rx_msg[type_field_pos];
	switch (type) {
	case FILAMUX__MESSAGE_TYPE__MSG_SET_SPOOL_PARAMS:
		handle_set_spool_params();
		break;
	case FILAMUX__MESSAGE_TYPE__MSG_SET_TARGET_SPOOL:
		handle_set_target_spool();
		break;
	case FILAMUX__MESSAGE_TYPE__MSG_EXTRUDER_FEED:
		handle_extruder_feed();
		break;
	case FILAMUX__MESSAGE_TYPE__MSG_EXTRUDER_GCODE:
		handle_extruder_gcode();
		break;
	default:
		LOG_WRN("Received unknown message type: %d", type);
	}
}

void filamux_serial_init(struct filamux *_filamux)
{
	filamux = _filamux;
	msg_len = 0;
	payload_size = 0;
	rx_buf_pos = 0;
	rx_state = FILAMUX_SERIAL_IDLE;
	remaining_bytes = 0;

	const struct uart_config cfg = {.baudrate = 115200,
					.parity = UART_CFG_PARITY_NONE,
					.stop_bits = UART_CFG_STOP_BITS_1,
					.data_bits = UART_CFG_DATA_BITS_8,
					.flow_ctrl = UART_CFG_FLOW_CTRL_NONE};
	if (uart_configure(serial_dev, &cfg) != 0) {
		LOG_ERR("Could not configure uart");
		return;
	}
	uart_irq_callback_set(serial_dev, serial_cb);
	uart_irq_rx_enable(serial_dev);
}

void filamux_serial_process()
{
	if (rx_state == FILAMUX_SERIAL_RECEIVED) {
		filamux_serial_process_msg();
		rx_state = FILAMUX_SERIAL_IDLE;
	}
}

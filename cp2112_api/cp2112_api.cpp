// to compile the project, run the command:
//  g++ ./main.cpp -o cp2112_api -I/usr/include/hidapi -lhidapi-libusb
using namespace std;
#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include "hidapi.h"

#define MAX_STR 255
#define VENDOR_ID 0x10c4
#define PRODUCT_ID 0xea90

struct HexCharStruct
{
	unsigned char c;
	HexCharStruct(unsigned char _c) : c(_c) {}
};

inline std::ostream &operator<<(std::ostream &o, const HexCharStruct &hs)
{
	return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c)
{
	return HexCharStruct(_c);
}

std::string ToNarrow(const wchar_t *s, char dfault = '?',
					 const std::locale &loc = std::locale())
{
	std::ostringstream stm;

	while (*s != L'\0')
	{
		stm << std::use_facet<std::ctype<wchar_t>>(loc).narrow(*s++, dfault);
	}
	return stm.str();
}

class PowerBoard
{
private:
	hid_device *_device;
	unsigned char current_data[2];
	unsigned char voltage_data[2];

	int data_read_request()
	{
		int res;
		unsigned char buf[4];
		buf[0] = 0x10;
		buf[1] = 0x6c;
		buf[2] = 0x00;
		buf[3] = 0x02;

		res = hid_write(_device, buf, 4);
		// cout << "data_read_request status: " << res << endl;
		return res;
	}

	int data_write(char data[2])
	{
		int res;
		unsigned char buf[5];
		buf[0] = 0x14;
		buf[1] = 0x6c;
		buf[2] = 0x02;
		buf[3] = data[0];
		buf[4] = data[1];

		res = hid_write(_device, buf, 5);
		// cout << "data_read_request status: " << res << endl;
		return res;
	}

	int transfer_status_request()
	{
		int res;
		unsigned char buf[2];
		buf[0] = 0x15;
		buf[1] = 0x01;

		res = hid_write(_device, buf, 2);
		// cout << "transfer_status_request status: " << res << endl;
		return res;
	}

	int get_read_write_transfer_status()
	{
		int res;
		unsigned char buf[7];

		res = hid_read(_device, buf, 7);
		// cout << "get_read_write_transfer_status status: " << res << endl;
		// // Print out the returned buffer.
		// for (int i = 0; i < 7; i++)
		// 	cout << "buf[" << i << "]: " << hex(buf[i]) << dec << endl;
		return res;
	}

	int data_force_read_request()
	{
		int res;
		unsigned char buf[3];
		buf[0] = 0x12;
		buf[1] = 0x00;
		buf[2] = 0x02;

		res = hid_write(_device, buf, 3);
		// cout << "data_force_read_response status: " << res << endl;
		return res;
	}

	int data_get_current_response()
	{
		int res;
		unsigned char buf[65];
		res = hid_read(_device, buf, 65);
		// cout << "data_get_read_response status: " << res << endl;
		// // Print out the returned buffer.
		// for (int i = 0; i < 5; i++)
		// 	cout << "buf[" << i << "]: " << hex(buf[i]) << dec << endl;
		current_data[0] = buf[3];
		current_data[1] = buf[4];
		return res;
	}

	int data_get_voltage_response()
	{
		int res;
		unsigned char buf[65];
		res = hid_read(_device, buf, 65);
		// cout << "data_get_read_response status: " << res << endl;
		// // Print out the returned buffer.
		// for (int i = 0; i < 5; i++)
		// 	cout << "buf[" << i << "]: " << hex(buf[i]) << dec << endl;
		voltage_data[0] = buf[3];
		voltage_data[1] = buf[4];
		return res;
	}

	int request_current_data()
	{
		int res;
		char data[2] = {0xd2, 0x01};
		data_write(data);
		data_read_request();
		transfer_status_request();
		get_read_write_transfer_status();
		data_force_read_request();
		res = data_get_current_response();
		return res;
	}

	int request_voltage_data()
	{
		int res;
		char data[2] = {0xd2, 0x03};
		data_write(data);
		data_read_request();
		transfer_status_request();
		get_read_write_transfer_status();
		data_force_read_request();
		res = data_get_voltage_response();
		return res;
	}

public:
	int connect()
	{
		int res;
		// Initialize the hidapi library
		res = hid_init();
		// Open the device using the VID, PID,
		// and optionally the Serial number.
		_device = hid_open(VENDOR_ID, PRODUCT_ID, NULL);
		if (_device == 0)
		{
			throw runtime_error("Cannot connect to the power board please check if it connected properly");
		}
		return res;
	}

	int disconnect()
	{
		int res;
		try
		{
			res = hid_exit();
		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << '\n';
		}

		return res;
	}

	int cp2112_device_reset()
	{
		int res;
		unsigned char buf[2];
		buf[0] = 0x01;
		buf[1] = 0x01;

		res = hid_write(_device, buf, 2);
		// cout << "cp2112 device reset status: " << res << endl;
		return res;
	}

	// configre GPIOs 1 & 2 as output and Push-Pull
	int configure_gpios_as_output()
	{
		int res;
		unsigned char buf[5];
		buf[0] = 0x02;
		buf[1] = 0x03;
		buf[2] = 0x03;
		buf[3] = 0x00;
		buf[4] = 0x00;

		res = hid_send_feature_report(_device, buf, 5);
		cout << "configureing GPIO 0&1 as output Push-Pull status: " << res << endl;
		return res;
	}

	int set_gpio0_value(uint8_t value)
	{
		int res;
		unsigned char buf[3];
		buf[0] = 0x04;
		if (value == 1)
			buf[1] = 0x01;
		else
			buf[1] = 0x00;
		buf[2] = 0x01;

		res = hid_send_feature_report(_device, buf, 3);
		cout << "Setting GPIO 0 value to " << to_string(value) << " status: " << res << endl;
		return res;
	}

	int gpio0_reset()
	{
		int res;
		configure_gpios_as_output();
		set_gpio0_value(0);
		usleep(1000000);
		res = set_gpio0_value(1);
		usleep(1000000);
		return res;
	}

	int set_gpio1_value(uint8_t value)
	{
		int res;
		unsigned char buf[3];
		buf[0] = 0x04;
		if (value == 1)
			buf[1] = 0x02;
		else
			buf[1] = 0x00;
		buf[2] = 0x02;

		res = hid_send_feature_report(_device, buf, 3);
		cout << "Setting GPIO 1 value to " << to_string(value) << " status: " << res << endl;
		return res;
	}

	void print_manufacturer_string()
	{
		int res;
		wchar_t wstr[MAX_STR];

		// Read the Manufacturer String
		res = hid_get_manufacturer_string(_device, wstr, MAX_STR);
		cout << "Manufacturer String: " << ToNarrow(wstr) << endl;
	}

	void print_product_string()
	{
		int res;
		wchar_t wstr[MAX_STR];

		// Read the Product String
		res = hid_get_product_string(_device, wstr, MAX_STR);
		cout << "Product String: " << ToNarrow(wstr) << endl;
	}

	void print_serial_number_string()
	{
		int res;
		wchar_t wstr[MAX_STR];

		// Read the Serial Number String
		res = hid_get_serial_number_string(_device, wstr, MAX_STR);
		cout << "Serial Number String: " << ToNarrow(wstr) << endl;
	}

	uint16_t get_current_data()
	{
		uint16_t data;
		request_current_data();
		for (int i = 0; i < 2; i++)
		{
			cout << "buf[" << i << "]: " << hex(current_data[i]) << dec << endl;
		}
		data = current_data[1] | (current_data[0] & 0x0F) << 8;
		return data;
	}

	uint16_t get_voltage_data()
	{
		uint16_t data;
		request_voltage_data();
		for (int i = 0; i < 2; i++)
		{
			cout << "buf[" << i << "]: " << hex(voltage_data[i]) << dec << endl;
		}
		data = voltage_data[1] | (voltage_data[0] & 0x0F) << 8;
		return data;
	}
};

// int main(int argc, char *argv[])
// {
// 	PowerBoard pB;

// 	pB.connect();

// 	pB.print_manufacturer_string();
// 	pB.print_product_string();

// 	// Configuring GPIOs
// 	pB.configure_gpios_as_output();

// 	// set gain to 100;
// 	pB.set_gpio1_value(1);

// 	// set gain to 10;
// 	// pB.set_gpio1_value(0);

// 	// Getting current data
// 	for (int i = 0; i < 40000; i++)
// 	{

// 		cout << "============================ iter: " << i << " ===============================" << endl;
// 		pB.get_current_data();
// 		pB.print_current_data();
// 		usleep(250000);
// 	}

// 	usleep(1000000);
// 	pB.disconnect();
// 	return 0;
// }

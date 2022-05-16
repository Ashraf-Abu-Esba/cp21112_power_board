yes | sudo apt-get update
yes | sudo apt install libhidapi-dev
yes | sudo apt-get install libudev-dev libusb-1.0-0-dev libfox-1.6-dev
sudo cp 69-hid.rules /etc/udev/rules.d/
cd cp2112_api/
g++ ./get_power_data.cpp -o get_power_data -I/usr/include/hidapi -lhidapi-libusb
g++ ./set_gpio_value.cpp -o set_gpio_value -I/usr/include/hidapi -lhidapi-libusb
g++ ./get_device_info.cpp -o get_device_info -I/usr/include/hidapi -lhidapi-libusb

#include "cp2112_api.cpp"

int main(int argc, char *argv[])
{
    PowerBoard pB;

    pB.connect();

    pB.print_manufacturer_string();
    pB.print_product_string();
    pB.print_serial_number_string();

    // 	usleep(1000000);
    // 	pB.disconnect();
    return 0;
}
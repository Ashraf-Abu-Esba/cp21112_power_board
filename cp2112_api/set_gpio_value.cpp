using namespace std;
#include <string>
#include <vector>
#include "cp2112_api.cpp"

int main(int argc, char *argv[])
{
    uint8_t pin;
    uint8_t value;
    vector<string> args;
    vector<string> needed_args;
    needed_args.push_back("-p");
    needed_args.push_back("-v");
    if (argc != 5)
    {
        cout << "Wrong command line arguments" << endl;
        cout << "usage: " << argv[0] << " -p <pin> -v <pin value>" << endl;
        return 1;
    }

    for (int i = 0; i < argc; i++)
    {
        args.push_back(argv[i]);
    }

    for (int i = 0; i < needed_args.size(); i++)
    {
        bool arg_exist = false;
        for (int j = 0; j < args.size(); j++)
        {
            if (args[j] == needed_args[i])
            {
                arg_exist = true;
                break;
            }
        }
        if (!arg_exist)
        {
            cout << "Command line argument: " << needed_args[i] << " are missing" << endl;
            cout << "usage: " << argv[0] << " -p <pin name> -v <pin value>" << endl;
            return 1;
        }
    }

    for (int i = 0; i < args.size(); i++)
    {
        if (args[i] == "-p")
            pin = (uint8_t)stoi(args[i + 1]);
        else if (args[i] == "-v")
            value = (uint8_t)stoi(args[i + 1]);
    }
    cout << "pin: " << (int)pin << endl;
    cout << "Value: " << (int)value << endl;

    PowerBoard pB;
    pB.connect();

    // Configuring GPIOs
    pB.configure_gpios_as_output();

    if (pin == 0)
        pB.set_gpio0_value(value);
    else
        pB.set_gpio1_value(value);

    // usleep(1000000);
    // pB.disconnect();
    return 0;
}
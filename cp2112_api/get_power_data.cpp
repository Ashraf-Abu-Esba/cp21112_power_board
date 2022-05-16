using namespace std;
#include <string>
#include <vector>
#include <fstream>
#include "cp2112_api.cpp"

int main(int argc, char *argv[])
{
	int rate;
	int time;
	string file_name;
	vector<string> args;
	vector<string> needed_args;
	needed_args.push_back("-r");
	needed_args.push_back("-t");
	needed_args.push_back("-f");
	if (argc != 7)
	{
		cout << "Wrong command line arguments" << endl;
		cout << "usage: " << argv[0] << " -r <sample rate> -t <time to run in seconeds> -f <save data file name>" << endl;
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
			cout << "usage: " << argv[0] << " -r <sample rate> -t <time to run in seconeds> -f <save data file name>" << endl;
			return 1;
		}
	}

	for (int i = 0; i < args.size(); i++)
	{
		if (args[i] == "-r")
			rate = stoi(args[i + 1]);
		else if (args[i] == "-t")
			time = stoi(args[i + 1]);
		else if (args[i] == "-f")
			file_name = args[i + 1];
	}
	cout << "rate: " << rate << endl;
	cout << "time in seconds: " << time << endl;
	cout << "file name: " << file_name << endl;

	PowerBoard pB;

	pB.connect();

	// Getting current data
	int samples_count = rate * time;
	int sample_time = (1000000 / rate);
	cout << "samples count: " << samples_count << endl;
	cout << "sample time: " << sample_time << " us" << endl;

	ofstream poewrDataCsv;
	poewrDataCsv.open(file_name, std::ios_base::app);
	if (poewrDataCsv.fail())
	{
		cout << "Cannot open file: " << file_name << endl;
	}
	poewrDataCsv << "Current value," << endl;
	uint16_t current_data; 
	for (int i = 0; i < samples_count; i++)
	{

		cout << "============================ sample: " << i << " ===============================" << endl;
		current_data = pB.get_current_data();
		cout << "current: " << to_string(current_data) << endl;
		poewrDataCsv << to_string(current_data)  << "," << endl;
		usleep(sample_time);
	}

	poewrDataCsv.close();

	// usleep(1000000);
	// pB.disconnect();
	return 0;
}
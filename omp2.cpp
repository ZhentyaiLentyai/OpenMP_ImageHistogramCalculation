#include <iostream>
#include <ctime>
#include <omp.h>
#include <fstream>
#include <sstream>

using namespace std;

struct FileInfo {
	int width = 0;
	int height = 0;
	int max_value = 0;
	unsigned char** matrix = 0;
};

int CheckArgc(int argc)
{
	if (argc < 4 || argc > 4) {
		cout << "Wrong number of arguments\n";
		return 1;
	}
	return 0;
}

int DefinitionThreads(char* argv[])
{
	int threads = atoi(argv[3]);

	switch (threads)
	{
	case -1:
		threads = 1;
		break;
	case 0:
		threads = omp_get_max_threads();
		break;
	default:

		break;
	}
	return threads;
}

bool Omp(int threads)
{
	bool omp = true;
	if (threads == 1) {
		omp = false;
	}

	return omp;
}

int CheckAndReadFileInput(char* argv[], FileInfo* file)
{
	ifstream image(argv[1], ios::binary);

	if (!image.is_open()) {
		cout << "Failed to open file" << endl;
		return 1;
	}

	string inputLine = "";

	getline(image, inputLine);

	if (inputLine.compare("P5") != 0) {
		cout << "Wrong format" << endl;
		return 1;
	}

	stringstream read;

	read << image.rdbuf();
	read >> file->width >> file->height;
	read >> file->max_value;

	if (file->max_value != 255) {		//на всякий случай :)
		cout << "Wrong format" << endl;
		return 1;
	}

	image.ignore();

	file->matrix = new unsigned char* [file->height]();
	for (int i = 0; i < file->height; i++) {
		file->matrix[i] = new unsigned char[file->width]();
	}

	unsigned char pixel;

	for (int i = 0; i < file->height; i++) {
		for (int j = 0; j < file->width; j++) {
			read >> pixel;
			file->matrix[i][j] = pixel;
		}
	}

	image.close();

	return 0;
}

void ColorCount(FileInfo* file, unsigned int* temp, int threads, bool omp)
{
#pragma omp parallel if (omp) num_threads(threads)
	{ 
		if(omp_in_parallel())
		{
#pragma omp for schedule(static, 4)
			for (int i = 0; i < file->height; i++) {
				for (int j = 0; j < file->width; j++) {
					temp[file->matrix[i][j]]++;
				}
			}
		}
		else 
		{
#pragma omp single
			for (int i = 0; i < file->height; i++) {
				for (int j = 0; j < file->width; j++) {
					temp[file->matrix[i][j]]++;
				}
			}
		}
	}
}

int CheckFileOutPut(char* argv[], unsigned int* temp)
{
	ofstream output(argv[2], ios::binary);

	if (!output.is_open()) {
		cout << "Failed to open file_output" << endl;
		return 1;
	}

	for (int j = 0; j < 256; j++) {

		int temp1;
		unsigned int arr[32];

		for (int i = 31; i >= 0; --i) {
			arr[i] = ((temp[j] >> i) & 1);
		}

		int temp2 = 24;
		int temp3 = 0;
		int count = 0;

		while (count < 4) {
			for (int i = 31 - temp2; i >= temp3; --i) {
				temp1 = i;
				if (++temp1 % 8 == 0) {
					output << " ";
				}
				output << arr[i];
			}
			temp2 -= 8;
			temp3 += 8;
			count++;
		}
		output << endl;
	}
	output.close();

	return 0;
}

void FreeMatrixMemory(FileInfo* file, unsigned int* temp)
{
	for (int i = 0; i < file->height; i++) {
		delete file->matrix[i];
	}

	delete file;

	delete[]temp;
}

int main(int argc, char* argv[])
{
	if (CheckArgc(argc) == 1) {
		return 1;
	}

	int threads = DefinitionThreads(argv);
	bool omp = Omp(threads);

	FileInfo* file = new FileInfo;

	if (CheckAndReadFileInput(argv, file) == 1) {
		return 1;
	}

	unsigned int* temp = new unsigned int[256];
	for (int i = 0; i < 256; i++) {
		temp[i] = 0;
	}

	double startTime = omp_get_wtime();

	ColorCount(file, temp, threads, omp);

	double endTime = omp_get_wtime();
	double time = (endTime - startTime) / 1000;

	if (CheckFileOutPut(argv, temp) == 1) {
		FreeMatrixMemory(file, temp);

		return 1;
	}

	FreeMatrixMemory(file, temp);

	printf("\nTime %i thread(s) %g ms", threads, time);

	return 0;
}

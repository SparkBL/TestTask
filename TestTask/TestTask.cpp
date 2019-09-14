//TestTask.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <fstream>
#include "time.h"
#include <sstream>
#include <iostream>
#pragma warning( disable : 4996)
clock_t t;
char* files[4] = { "0.txt","1.txt","2.txt","3.txt" };
char* c_file = "config.ini";
char* param_names[4] = { "L","MAX_s","MAX_i","max_str_len" };

struct Config {
	int L = 100000;
	char *MAX_s = "\\\\\\";
	int MAX_i = -1;
	int max_str_len = 100;
};

void load_config(Config &config)
{
	std::ifstream fin;
	fin.open(c_file, std::ifstream::in);
	if (fin.fail())
	{
		std::cerr << "\nUsing default configuration..\n";
		return;
	}
	std::string line;
	while (!fin.eof())
	{
		getline(fin, line);
		std::istringstream sin(line.substr(line.find("=") + 1));
		if (line.find(param_names[0]) != -1)
			sin >> config.L;
		else if (line.find(param_names[1]) != -1)
		{
			config.MAX_s = new char[10];
			sin >> config.MAX_s;
		}
		else if (line.find(param_names[2]) != -1)
			sin >> config.MAX_i;
		else if (line.find(param_names[3]) != -1)
			sin >> config.max_str_len;
		if (config.L <= 0 || config.max_str_len < 5)
		{
			std::cerr << "\nWrong configuration\nMAX_ELEMENTS_STORED_IN_RAM(>1) or MAX_STR_LENGTH(>4) have incorrect values\n";
			fin.close();
			exit(1);
		}
	}
	fin.close();
}

void write_config(char* params[])
{
	FILE* f = fopen(c_file, "w");
	for (int i = 0;i < 4;i++)
	{
		fprintf(f, "%s=%s\n", param_names[i], params[i + 2]);
	}
	fclose(f);
}

void merge_int(int *A, int begin, int end, int *D)
{
	int center = (begin + end) / 2;
	if (begin < center) merge_int(A, begin, center, D);
	if (center + 1 < end) merge_int(A, center + 1, end, D);
	int i = begin, j = center + 1, k;
	for (k = begin; k <= end; k++)
		if (j > end) D[k] = A[i++];
		else if (i > center) D[k] = A[j++];
		else if (A[i] <= A[j]) D[k] = A[i++];
		else D[k] = A[j++];
		for (int i = begin; i <= end; i++)
			A[i] = D[i];
}

void merge_str(char **A, int begin, int end, char **D)
{
	int center = (begin + end) / 2;
	if (begin < center) merge_str(A, begin, center, D);
	if (center + 1 < end) merge_str(A, center + 1, end, D);
	int i = begin, j = center + 1, k;
	for (k = begin; k <= end; k++)
		if (j > end) D[k] = A[i++];
		else if (i > center) D[k] = A[j++];
		else if (strcmp(A[i], A[j]) <= 0) D[k] = A[i++];
		else D[k] = A[j++];
		for (int i = begin; i <= end; i++)
			A[i] = D[i];
}

void twoway_merge_int_asc(int input_c, char* inputs[], Config &conf)
{
	FILE **f, *in;
	f = new FILE*[4];
	int *buffer = new int[conf.L];
	int *work_buffer = new int[conf.L];
	int r = 0, w = 2, k = 0, i = 0;
	int x, y;
	f[r] = fopen(files[2 - w], "w+");
	f[r + 1] = fopen(files[2 - w + 1], "w+");
	for (int q = 1;q < input_c;q++)
	{
		in = fopen(inputs[q], "r");
		if (in != NULL)
		{
			if (fscanf(in, "%c", &x) == EOF)
			{
				std::cerr << "\nInput file " << inputs[q] << " is empty\n";
				fclose(in);
				continue;
			}
			rewind(in);
		}
		else {
			std::cerr << "\nCannot open file " << inputs[q] << "\n";
			continue;
		}
		while (!feof(in))
		{
			while ((i < conf.L) && (!feof(in)))
			{
				fscanf_s(in, "%i\n", &buffer[i]);
				i++;
			}
			k++;
			merge_int(buffer, 0, i - 1, work_buffer);
			for (int j = 0;j < i;j++)fprintf_s(f[r], "%i\n", buffer[j]);
			fprintf_s(f[r], "%i\n", conf.MAX_i);
			i = 0;
			r = 1 - r;
		}
	}
	r = 0;
	fclose(f[r]);fclose(f[r + 1]);
	while (k > 1) {
		conf.L *= 2;
		k = 0;
		f[r] = fopen(files[2 - w], "rt");
		f[r + 1] = fopen(files[2 - w + 1], "rt");
		f[w] = fopen(files[2 - r], "wt");
		f[w + 1] = fopen(files[2 - r + 1], "wt");
		while (!feof(f[r]) || !feof(f[r + 1])) {
			fscanf_s(f[r], "%i\n", &x);  fscanf_s(f[r + 1], "%i\n", &y);
			while (x != conf.MAX_i || y != conf.MAX_i) {
				if (y == conf.MAX_i)
				{
					fprintf_s(f[w], "%i\n", x);
					fscanf_s(f[r], "%i\n", &x);
				}
				else if (x == conf.MAX_i)
				{
					fprintf_s(f[w], "%i\n", y);
					fscanf_s(f[r + 1], "%i\n", &y);
				}
				else
					if (x <= y)
					{
						{
							fprintf_s(f[w], "%i\n", x);
							fscanf_s(f[r], "%i\n", &x);
						}
					}
					else
					{
						fprintf_s(f[w], "%i\n", y);
						fscanf_s(f[r + 1], "%i\n", &y);
					}
			}
			fprintf_s(f[w], "%i\n", conf.MAX_i);
			k++;
			if (w % 2 == 1) w--; else w++;
		}
		for (int i = 0;i < 4;i++)fclose(f[i]);
		r = 2 - r;
		w = 2 - r;
	}
	if (r == 2)
	{
		f[2] = fopen(files[2], "rt");
		f[1] = fopen(inputs[0], "wt");
		fscanf_s(f[2], "%i\n", &x);
		while (!feof(f[2]) && x != conf.MAX_i)
		{
			fprintf_s(f[1], "%i\n", x);
			fscanf_s(f[2], "%i\n", &x);
		}
		fclose(f[2]);fclose(f[1]);
	}
	else
	{
		f[0] = fopen(files[0], "rt");
		f[1] = fopen(inputs[0], "wt");
		fscanf_s(f[0], "%i\n", &x);
		while (!feof(f[0]) && x != conf.MAX_i)
		{
			fprintf_s(f[1], "%i\n", x);
			fscanf_s(f[0], "%i\n", &x);
		}
		fclose(f[0]);fclose(f[1]);
	}
}

void twoway_merge_str_asc(int input_c, char* inputs[], Config &conf)
{
	FILE **f, *in;
	f = new FILE*[4];
	char **buffer = new char*[conf.L];
	char **work_buffer = new char*[conf.L];
	for (int q = 0;q < conf.L;q++)
	{
		buffer[q] = new char[conf.max_str_len];
		work_buffer[q] = new char[conf.max_str_len];
	}
	int r = 0, w = 2, k = 0, i = 0;
	char *x = new char[conf.max_str_len], *y = new char[conf.max_str_len];
	f[r] = fopen(files[2 - w], "w+");
	f[r + 1] = fopen(files[2 - w + 1], "w+");
	for (int q = 1;q < input_c;q++)
	{
		in = fopen(inputs[q], "r");
		if (in != NULL)
		{
			if (fscanf(in, "%c", &x) == EOF)
			{
				std::cerr << "\nInput file " << inputs[q] << " is empty\n";
				fclose(in);
				continue;
			}
			rewind(in);
		}
		else {
			std::cerr << "\nCannot open file " << inputs[q] << "\n";
			continue;
		}
		while (!feof(in))
		{
			while ((i < conf.L) && (!feof(in)))
			{
				fscanf_s(in, "%s\n", &buffer[i][0], conf.max_str_len);
				i++;
			}
			k++;
			merge_str(buffer, 0, i - 1, work_buffer);
			for (int j = 0;j < i;j++)fprintf_s(f[r], "%s\n", buffer[j]);
			fprintf_s(f[r], "%s\n", conf.MAX_s);
			i = 0;
			r = 1 - r;
		}
	}
	r = 0;
	fclose(f[r]);fclose(f[r + 1]);
	while (k > 1) {
		conf.L *= 2;
		k = 0;
		f[r] = fopen(files[2 - w], "rt");
		f[r + 1] = fopen(files[2 - w + 1], "rt");
		f[w] = fopen(files[2 - r], "wt");
		f[w + 1] = fopen(files[2 - r + 1], "wt");
		while (!feof(f[r]) || !feof(f[r + 1])) {
			fscanf_s(f[r], "%s\n", &x[0], conf.max_str_len);  fscanf_s(f[r + 1], "%s\n", &y[0], conf.max_str_len);
			while (strcmp(x, conf.MAX_s) != 0 || strcmp(y, conf.MAX_s) != 0) {
				if (strcmp(y, conf.MAX_s) == 0)
				{
					fprintf_s(f[w], "%s\n", x);
					fscanf_s(f[r], "%s\n", &x[0], conf.max_str_len);
				}
				else if (strcmp(x, conf.MAX_s) == 0)
				{
					fprintf_s(f[w], "%s\n", y);
					fscanf_s(f[r + 1], "%s\n", &y[0], conf.max_str_len);
				}
				else
					if (strcmp(x, y) <= 0)
					{
						{
							fprintf_s(f[w], "%s\n", x);
							fscanf_s(f[r], "%s\n", &x[0], conf.max_str_len);
						}
					}
					else
					{
						fprintf_s(f[w], "%s\n", y);
						fscanf_s(f[r + 1], "%s\n", &y[0], conf.max_str_len);
					}
			}
			fprintf_s(f[w], "%s\n", conf.MAX_s);
			k++;
			if (w % 2 == 1) w--; else w++;
		}
		for (int i = 0;i < 4;i++)fclose(f[i]);
		r = 2 - r;
		w = 2 - r;
	}
	if (r == 2)
	{
		f[2] = fopen(files[2], "rt");
		f[1] = fopen(inputs[0], "wt");
		fscanf_s(f[2], "%s\n", &x[0], conf.max_str_len);
		while (!feof(f[2]) && strcmp(x, conf.MAX_s) != 0)
		{
			fprintf_s(f[1], "%s\n", x);
			fscanf_s(f[2], "%s\n", &x[0], conf.max_str_len);
		}
		fclose(f[2]);fclose(f[1]);
	}
	else
	{
		f[0] = fopen(files[0], "rt");
		f[1] = fopen(inputs[0], "wt");
		fscanf_s(f[0], "%s\n", &x[0], conf.max_str_len);
		while (!feof(f[0]) && strcmp(x, conf.MAX_s) != 0)
		{
			fprintf_s(f[1], "%s\n", x);
			fscanf_s(f[0], "%s\n", &x[0], conf.max_str_len);
		}
		fclose(f[0]);fclose(f[1]);
	}
}

void twoway_merge_int_dec(int input_c, char* inputs[], Config &conf)
{
	FILE **f, *in;
	f = new FILE*[4];
	int *buffer = new int[conf.L];
	int *work_buffer = new int[conf.L];
	int r = 0, w = 2, k = 0, i = 0;
	int x, y;
	f[r] = fopen(files[2 - w], "w+");
	f[r + 1] = fopen(files[2 - w + 1], "w+");
	for (int q = 1;q < input_c;q++)
	{
		in = fopen(inputs[q], "r");
		if (in != NULL)
		{
			if (fscanf(in, "%c", &x) == EOF)
			{
				std::cerr << "\nInput file " << inputs[q] << " is empty\n";
				fclose(in);
				continue;
			}
			rewind(in);
		}
		else {
			std::cerr << "\nCannot open file " << inputs[q] << "\n";
			continue;
		}
		while (!feof(in))
		{
			while ((i < conf.L) && (!feof(in)))
			{
				fscanf_s(in, "%i\n", &buffer[i]);
				i++;
			}
			k++;
			merge_int(buffer, 0, i - 1, work_buffer);
			for (int j = i - 1;j != -1;j--)fprintf_s(f[r], "%i\n", buffer[j]);
			fprintf_s(f[r], "%i\n", conf.MAX_i);
			i = 0;
			r = 1 - r;
		}
	}
	r = 0;
	fclose(f[r]);fclose(f[r + 1]);
	while (k > 1) {
		conf.L *= 2;
		k = 0;
		f[r] = fopen(files[2 - w], "rt");
		f[r + 1] = fopen(files[2 - w + 1], "rt");
		f[w] = fopen(files[2 - r], "wt");
		f[w + 1] = fopen(files[2 - r + 1], "wt");
		while (!feof(f[r]) || !feof(f[r + 1])) {
			fscanf_s(f[r], "%i\n", &x);  fscanf_s(f[r + 1], "%i\n", &y);
			while (x != conf.MAX_i || y != conf.MAX_i) {
				if (y == conf.MAX_i)
				{
					fprintf_s(f[w], "%i\n", x);
					fscanf_s(f[r], "%i\n", &x);
				}
				else if (x == conf.MAX_i)
				{
					fprintf_s(f[w], "%i\n", y);
					fscanf_s(f[r + 1], "%i\n", &y);
				}
				else
					if (x >= y)
					{
						{
							fprintf_s(f[w], "%i\n", x);
							fscanf_s(f[r], "%i\n", &x);
						}
					}
					else
					{
						fprintf_s(f[w], "%i\n", y);
						fscanf_s(f[r + 1], "%i\n", &y);
					}
			}
			fprintf_s(f[w], "%i\n", conf.MAX_i);
			k++;
			if (w % 2 == 1) w--; else w++;
		}
		for (int i = 0;i < 4;i++)fclose(f[i]);
		r = 2 - r;
		w = 2 - r;
	}
	if (r == 2)
	{
		f[2] = fopen(files[2], "rt");
		f[1] = fopen(inputs[0], "wt");
		fscanf_s(f[2], "%i\n", &x);
		while (!feof(f[2]) && x != conf.MAX_i)
		{
			fprintf_s(f[1], "%i\n", x);
			fscanf_s(f[2], "%i\n", &x);
		}
		fclose(f[2]);fclose(f[1]);
	}
	else
	{
		f[0] = fopen(files[0], "rt");
		f[1] = fopen(inputs[0], "wt");
		fscanf_s(f[0], "%i\n", &x);
		while (!feof(f[0]) && x != conf.MAX_i)
		{
			fprintf_s(f[1], "%i\n", x);
			fscanf_s(f[0], "%i\n", &x);
		}
		fclose(f[0]);fclose(f[1]);
	}
}

void twoway_merge_str_dec(int input_c, char* inputs[], Config &conf)
{
	FILE **f, *in;
	f = new FILE*[4];
	char **buffer = new char*[conf.L];
	char **work_buffer = new char*[conf.L];
	for (int q = 0;q < conf.L;q++)
	{
		buffer[q] = new char[conf.max_str_len];
		work_buffer[q] = new char[conf.max_str_len];
	}
	int r = 0, w = 2, k = 0, i = 0;
	char *x = new char[conf.max_str_len], *y = new char[conf.max_str_len];
	f[r] = fopen(files[2 - w], "w+");
	f[r + 1] = fopen(files[2 - w + 1], "w+");
	for (int q = 1;q < input_c;q++)
	{
		in = fopen(inputs[q], "r");
		if (in != NULL)
		{
			if (fscanf(in, "%c", &x) == EOF)
			{
				std::cerr << "\nInput file " << inputs[q] << " is empty\n";
				fclose(in);
				continue;
			}
			rewind(in);
		}
		else {
			std::cerr << "\nCannot open file " << inputs[q] << "\n";
			continue;
		}
		while (!feof(in))
		{
			while ((i < conf.L) && (!feof(in)))
			{
				fscanf_s(in, "%s\n", &buffer[i][0], conf.max_str_len);
				i++;
			}
			k++;
			merge_str(buffer, 0, i - 1, work_buffer);
			for (int j = i - 1;j != -1;j--)fprintf_s(f[r], "%s\n", buffer[j]);
			fprintf_s(f[r], "%s\n", conf.MAX_s);
			i = 0;
			r = 1 - r;
		}
	}
	r = 0;
	fclose(f[r]);fclose(f[r + 1]);
	while (k > 1) {
		conf.L *= 2;
		k = 0;
		f[r] = fopen(files[2 - w], "rt");
		f[r + 1] = fopen(files[2 - w + 1], "rt");
		f[w] = fopen(files[2 - r], "wt");
		f[w + 1] = fopen(files[2 - r + 1], "wt");
		while (!feof(f[r]) || !feof(f[r + 1])) {
			fscanf_s(f[r], "%s\n", &x[0], conf.max_str_len);  fscanf_s(f[r + 1], "%s\n", &y[0], conf.max_str_len);
			while (strcmp(x, conf.MAX_s) != 0 || strcmp(y, conf.MAX_s) != 0) {
				if (strcmp(y, conf.MAX_s) == 0)
				{
					fprintf_s(f[w], "%s\n", x);
					fscanf_s(f[r], "%s\n", &x[0], conf.max_str_len);
				}
				else if (strcmp(x, conf.MAX_s) == 0)
				{
					fprintf_s(f[w], "%s\n", y);
					fscanf_s(f[r + 1], "%s\n", &y[0], conf.max_str_len);
				}
				else
					if (strcmp(x, y) >= 0)
					{
						{
							fprintf_s(f[w], "%s\n", x);
							fscanf_s(f[r], "%s\n", &x[0], conf.max_str_len);
						}
					}
					else
					{
						fprintf_s(f[w], "%s\n", y);
						fscanf_s(f[r + 1], "%s\n", &y[0], conf.max_str_len);
					}
			}
			fprintf_s(f[w], "%s\n", conf.MAX_s);
			k++;
			if (w % 2 == 1) w--; else w++;
		}
		for (int i = 0;i < 4;i++)fclose(f[i]);
		r = 2 - r;
		w = 2 - r;
	}
	if (r == 2)
	{
		f[2] = fopen(files[2], "rt");
		f[1] = fopen(inputs[0], "wt");
		fscanf_s(f[2], "%s\n", &x[0], conf.max_str_len);
		while (!feof(f[2]) && strcmp(x, conf.MAX_s) != 0)
		{
			fprintf_s(f[1], "%s\n", x);
			fscanf_s(f[2], "%s\n", &x[0], conf.max_str_len);
		}
		fclose(f[2]);fclose(f[1]);
	}
	else
	{
		f[0] = fopen(files[0], "rt");
		f[1] = fopen(inputs[0], "wt");
		fscanf_s(f[0], "%s\n", &x[0], conf.max_str_len);
		while (!feof(f[0]) && strcmp(x, conf.MAX_s) != 0)
		{
			fprintf_s(f[1], "%s\n", x);
			fscanf_s(f[0], "%s\n", &x[0], conf.max_str_len);
		}
		fclose(f[0]);fclose(f[1]);
	}
}



void gettime()
{
	printf("\nSorting time: %.10Lf \n", (long double)(t) / CLOCKS_PER_SEC);
};

bool check_asc(char* type, char* input, Config &conf)
{
	if (strcmp(type, "-i") == 0)
	{
		FILE* f = fopen(input, "r");
		int  x, y;
		while (!feof(f))
		{
			fscanf_s(f, "%i\n", &x);
			fscanf_s(f, "%i\n", &y);
			if (x > y)  break;
		}
		if (feof(f))
		{
			fclose(f);
			return true;
		}
		else return false;
	}
	else
		if (strcmp(type, "-s") == 0)
		{
			FILE* f = fopen(input, "r");
			char *z = new char[conf.max_str_len], *r = new char[conf.max_str_len];
			while (!feof(f))
			{
				fscanf_s(f, "%s\n", &z[0], conf.max_str_len);
				fscanf_s(f, "%s\n", &r[0], conf.max_str_len);
				if (strcmp(z, r) > 0)  break;
			}
			if (feof(f))
			{
				fclose(f);
				return true;
			}
			else return false;
		}
}

bool check_dec(char* type, char* input, Config &conf)
{
	if (strcmp(type, "-i") == 0)
	{
		FILE* f = fopen(input, "r");
		int  x, y;
		while (!feof(f))
		{
			fscanf_s(f, "%i\n", &x);
			fscanf_s(f, "%i\n", &y);
			if (x < y)  break;
		}
		if (feof(f))
		{
			fclose(f);
			return true;
		}
		else return false;
	}
	else
		if (strcmp(type, "-s") == 0)
		{
			FILE* f = fopen(input, "r");
			char *z = new char[conf.max_str_len], *r = new char[conf.max_str_len];
			while (!feof(f))
			{
				fscanf_s(f, "%s\n", &z[0], conf.max_str_len);
				fscanf_s(f, "%s\n", &r[0], conf.max_str_len);
				if (strcmp(z, r) < 0)  break;
			}
			if (feof(f))
			{
				fclose(f);
				return true;
			}
			else return false;
		}
}

int main(int argc, char* argv[])
{
	Config conf;
	if (argc == 1)
	{
		std::cerr << "\nUsage: " << argv[0] << "  DIRECTION( -a , -d)(optional)   TYPE( -i , -s)   OUTPUT   INPUTS\n";
		return 1;
	}
	if (strcmp(argv[1], "-c") == 0)
	{
		if (argc < 6)
		{
			std::cerr << "\nConfig usage :\n" << argv[0] << " -c MAX_ELEMENTS_STORED_IN_RAM   STR_SEPARATOR   INT_SEPARATOR   MAX_STR_LENGTH\n";
			return 1;
		}
		else
			write_config(argv);
		std::cout << "\nConfigured successfully\n";
		return 0;
	}
	load_config(conf);
	if (strcmp(argv[1], "-a") == 0)
	{
		if (argc < 5) {
			std::cerr << "\nUsage: " << argv[0] << "  DIRECTION( -a , -d)(optional)   TYPE( -i , -s)   OUTPUT   INPUTS\n";
			return 1;
		}
		else
		{
			if (strcmp(argv[2], "-i") == 0)
			{
				char ** inputs = new char*[argc - 3];
				for (int i = 0;i < argc - 3;i++)
				{
					inputs[i] = new char;
					inputs[i] = argv[i + 3];
				}
				t = clock();
				twoway_merge_int_asc(argc - 3, inputs, conf);
				t = clock() - t;
				std::cout << "\nCheck: " << check_asc(argv[2], inputs[0], conf);
				gettime();
			}
			else
				if (strcmp(argv[2], "-s") == 0)
				{
					char ** inputs = new char*[argc - 3];
					for (int i = 0;i < argc - 3;i++)
					{
						inputs[i] = new char;
						inputs[i] = argv[i + 3];
					}
					t = clock();
					twoway_merge_str_asc(argc - 3, inputs, conf);
					t = clock() - t;
					std::cout << "\nCheck: " << check_asc(argv[2], inputs[0], conf);
					gettime();
				}
				else
				{
					std::cerr << "\nWrong type\nUsage: " << argv[0] << "  DIRECTION( -a , -d)(optional)   TYPE( -i , -s)   OUTPUT   INPUTS\n";
					return 1;
				}
		}
	}
	else
	{
		if (strcmp(argv[1], "-d") == 0)
		{
			if (argc < 5) {
				std::cerr << "\nUsage: " << argv[0] << "  DIRECTION( -a , -d)(optional)   TYPE( -i , -s)   OUTPUT   INPUTS\n";
				return 1;
			}
			else
			{
				if (strcmp(argv[2], "-i") == 0)
				{
					char ** inputs = new char*[argc - 3];
					for (int i = 0;i < argc - 3;i++)
					{
						inputs[i] = new char;
						inputs[i] = argv[i + 3];
					}
					t = clock();
					twoway_merge_int_dec(argc - 3, inputs, conf);
					t = clock() - t;
					std::cout << "\nCheck: " << check_dec(argv[2], inputs[0], conf);
					gettime();
				}
				else
					if (strcmp(argv[2], "-s") == 0)
					{
						char ** inputs = new char*[argc - 3];
						for (int i = 0;i < argc - 3;i++)
						{
							inputs[i] = new char;
							inputs[i] = argv[i + 3];
						}
						t = clock();
						twoway_merge_str_dec(argc - 3, inputs, conf);
						t = clock() - t;
						std::cout << "\nCheck: " << check_dec(argv[2], inputs[0], conf);
						gettime();
					}
					else
					{
						std::cerr << "\nWrong type\nUsage: " << argv[0] << "  DIRECTION( -a , -d)(optional)   TYPE( -i , -s)   OUTPUT   INPUTS\n";
						return 1;
					}
			}
		}
		else
		{
			if (argc < 4) {
				std::cerr << "\nUsage: " << argv[0] << "  DIRECTION( -a , -d)(optional)   TYPE( -i , -s)   OUTPUT   INPUTS\n";
				return 1;
			}
			else
			{
				if (strcmp(argv[1], "-i") == 0)
				{
					char ** inputs = new char*[argc - 2];
					for (int i = 0;i < argc - 2;i++)
					{
						inputs[i] = new char;
						inputs[i] = argv[i + 2];
					}
					t = clock();
					twoway_merge_int_asc(argc - 2, inputs, conf);
					t = clock() - t;
					std::cout << "\nCheck: " << check_asc(argv[1], inputs[0], conf);
					gettime();
				}
				else
					if (strcmp(argv[1], "-s") == 0)
					{
						char ** inputs = new char*[argc - 2];
						for (int i = 0;i < argc - 2;i++)
						{
							inputs[i] = new char;
							inputs[i] = argv[i + 2];
						}
						t = clock();
						twoway_merge_str_asc(argc - 2, inputs, conf);
						t = clock() - t;
						std::cout << "\nCheck: " << check_asc(argv[1], inputs[0], conf);
						gettime();
					}
					else
					{
						std::cerr << "\nWrong type\nUsage: " << argv[0] << "  DIRECTION( -a , -d)(optional)   TYPE( -i , -s)   OUTPUT   INPUTS\n";
						return 1;
					}
			}
		}
	}
	return 0;
}
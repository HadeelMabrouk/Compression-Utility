#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <vector>
#include <bitset>
#include <cstring>
#include <cmath>

using namespace std;
const int SIZE = 300;

struct node {
	int freq;
	int c;
	node* right;
	node* left;
};


struct CompareFreq {
	bool operator()(node* const& right, node* const& left)
	{
		return right->freq > left->freq;
	}
};


void getFrequencyArr(string fileName, int arr[], long long& originalsize)
{
	originalsize = 0;
	for (int i = 0; i < SIZE; i++)
		arr[i] = 0;
	ifstream infile;
	infile.open(fileName.c_str());
	if (infile.is_open())
	{
		char c;
		while (!infile.eof())
		{
			infile.get(c);
			if (!infile.eof())
			{
				arr[int(c)]++;
				originalsize++;
			}
		}
		infile.close();
	}
	else
		cout << "File not found" << endl;
}

void getCharsCode(node* root, string str, string arr[])
{
	if (root->right == NULL)
	{
		arr[root->c] = str;
		return;
	}
	else
	{
		getCharsCode(root->right, str + "0", arr);
		getCharsCode(root->left, str + "1", arr);
	}

}


void makeCompressedFile(string fileName, string fileNameBin, string arr[])
{
	ifstream infile;
	ofstream outfile;
	outfile.open(fileNameBin.c_str(), ios::binary | ios::out);
	infile.open(fileName.c_str());
	if (outfile.is_open() && infile.is_open())
	{
		char c;
		string s = "";
		int size = 0;
		//int base = 1;
		//unsigned __int8 buffer = 0;
		while (!infile.eof())
		{
			infile.get(c);
			if (!infile.eof())
			{
				s += arr[int(c)];
				size++;
				/*for (int i = 0; i < arr[int(c)].size(); i++)
				{
					if (base < 256)
					{
						buffer += (arr[int(c)][i] - '0')*base;
						base *= 2;
					}
					else
					{
						outfile.write(reinterpret_cast<const char*>(&buffer), sizeof(unsigned __int8));
						base = 1;
						buffer = (arr[int(c)][i] - '0')*base;
					}
				}*/
			}

		}
		//s += "00000000";
		int index = 0;
		int base = 1;
		int counter = 1;
		while (index < s.size())
		{
			unsigned _int8 x = 0;
			base = 1;
			for (int i = 0; i < 8; i++)
			{
				if ((8 * counter - 1 - i) < 0 || (8 * counter - 1 - i) >= s.size())
					x += base;
				else
					x += (s[8 * counter - 1 - i] - '0')*base;
				base *= 2;
				index++;
			}
			outfile.write(reinterpret_cast<const char*>(&x), sizeof(unsigned __int8));
			counter++;
		}
		/*if (base !=1)
			outfile.write(reinterpret_cast<const char*>(&buffer), sizeof(unsigned __int8));*/
		outfile.close();
		infile.close();
	}
	else
	{
		cout << "Binary File Not Found" << endl;
	}
}


void Decompress(string fileName, string fileNameBin, node* root, long long originalsize)
{
	ifstream infile;
	ofstream outfile;
	string str = "";
	node* temp = new node;
	temp = root;
	infile.open(fileNameBin.c_str(), ios::binary | ios::in);
	outfile.open(fileName.c_str());
	if (outfile.is_open() && infile.is_open())
	{
		int base = 128;
		unsigned _int8 test;
		infile.read(reinterpret_cast<char*>(&test), sizeof(unsigned _int8));
		while (!infile.eof() && str.size() < originalsize)
		{

			if (base < 1)
			{
				infile.read(reinterpret_cast<char*>(&test), sizeof(unsigned _int8));
				base = 128;
			}
			if (!infile.eof() )//&& str.size() < originalsize)
			{
				while (temp->right && base > 0)
				{
					if (test / base == 0)
						temp = temp->right;
					else
						temp = temp->left;
					test %= base;
					base /= 2;
				}
				if (!temp->right)
				{
					str += char(temp->c);
					temp = root;
				}
			}

		}
		for (int i = 0; i < originalsize; i++)
			outfile << str[i];
		outfile.close();
		infile.close();

	}
	else
	{
		cout << "Binary File Not Found" << endl;
	}
}
void main()
{
	int freqArr[SIZE];
	long long originalsize;
	string fileName, fileNameBin;
	cout << "Enter the name of the file you want to compress" << endl;
	cin >> fileName;
	//fileName = "infile.txt";
	cout << "Enter the name of the binary file" << endl;
	cin >> fileNameBin;
	//fileNameBin = "compressed.bin";

	getFrequencyArr(fileName, freqArr, originalsize);
	float H = 0;
	priority_queue <node*, vector<node*>, CompareFreq > minHeap;
	for (int i = 0; i < SIZE; i++)
	{
		if (freqArr[i] != 0)
		{
			float p = freqArr[i] / float(originalsize);
			H += (p*log2(p));
			node* leaf = new node;
			leaf->freq = freqArr[i];
			leaf->c = i;
			leaf->right = NULL;
			leaf->left = NULL;
			minHeap.push(leaf);
		}
	}

	bool oneChar = false;
	if(minHeap.size()==1)
		oneChar = true;
	H = -1 * H;
	node* root = new node;
	while (minHeap.size() > 0)
	{
		node* right = new node;
		right = minHeap.top();
		minHeap.pop();
		if (minHeap.empty())
		{
			root = right;
		}
		else
		{
			node* left = new node;
			left = minHeap.top();
			minHeap.pop();
			node* n = new node();
			n->c = -1;
			n->freq = right->freq + left->freq;
			n->left = left;
			n->right = right;
			minHeap.push(n);
			root = n;
		}
	}
	string codeArr[SIZE];
	string str = "";
	if (oneChar) str = "0";
	getCharsCode(root, str, codeArr);
	float L = 0;
	for (int i = 0; i < SIZE; i++)
	{
		L += codeArr[i].size() * freqArr[i] / float(originalsize);
	}

	//cout << L << endl << H;
	makeCompressedFile(fileName, fileNameBin, codeArr);
	cout << "The code efficiency = " << H / L << endl;
	cout << "The compression ratio = " << L / 8.0 << endl;

	string DeCompFileName;
	cout << "Enter the decompressed file name" << endl;
	//DeCompFileName = "decomp.txt";
	cin >> DeCompFileName;
	Decompress(DeCompFileName, fileNameBin, root, originalsize);
	system("pause");
}
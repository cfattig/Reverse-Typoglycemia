/*************************************************************************//**
 * @file 
 *
 * @mainpage program 3 - Reverse Typoglycemia
 * 
 * @section course_section Course Information 
 *
 * @author Christian Fattig
 * 
 * @date Nov 18, 2016
 * 
 * @par Professor: 
 *         Roger Schrader
 * 
 * @par Course: 
 *         CSC 250 - M001 -  1:00-pm
 * 
 * @par Location: 
 *         CB - 107
 *
 * @section program_section Program Information 
 * 
 * @details 
 * This program is a word decryption tool. Given that both the first letter
 * and the last letter are in the correct locations, the program will 
 * permute all possible combinations of the letters in between the first 
 * and last letters untill a word has been found. The program is given 
 * a file of scrambled words and it will process each word and then replace
 * these words into a new file while preserving whitespace.
 * 
 * The program starts by checking if it has a valid argument count given
 * where, if valid, will proceed to open and check for the neccessary files
 * and will otherwise produce and error message and ussage statement while
 * terminating the program. From there the program loads dictionary.txt in
 * to a vector and then sorts it. Next the program reads in the words from
 * the scrambled file one at a time where it then permutes every 
 * combination of indexes for the word and returns the first word that it 
 * finds when being compared to the dictionary vector. These words are 
 * then added to a new file while preserving whitespace, capitalization, and
 * punctuation. 
 *
 * When a word is inputted from the file to be unscrambled, it is sent to a
 * permute function where all possible combinations of indexes are created 
 * untill a word is found. This is done by creating 2 dynamically allocated
 * arrays fitted for the word to be used that are then put into the permute
 * function. Recursion and the arrays are used to find all possible
 * combinations of indexes where a binaray search is then used after the 
 * word is created from the indexes to find if that word created from the
 * new indexes is an actual word. It does this by searching through the 
 * vector. Once the word is found, it has its capitalization and punctuation
 * restored if need be and is then outputed to the new file. 
 *
 *
 * @section compile_section Compiling and Usage 
 *		Usage: prog3.exe message.tgc message.rst
 *
 * @par Compiling Instructions: 
 *		None
 * 
 * @par Usage: 
   @verbatim  
   c:\> prog3.exe 
   d:\> c:\bin\prog3.exe 
   @endverbatim 
 *
 * @section todo_bugs_modification_section Todo, Bugs, and Modifications
 * 
 * 
 * 
 * @par Modifications and Development Timeline: 
   @verbatim 
   Date          Modification 
   ------------  -------------------------------------------------------------- 
   Oct 31, 2016  Created argument usage, error statements, and open files
   Nov 03, 2016  Can read from one file and write to another preserving
				 whitespace and newlines
   Nov 07, 2016  Preserves punctuation and capitalization
   Nov 11, 2016  Permutes all combos for a word given and preserves
				 the first and last letters
   Nov 16, 2016  Returns first matched word from dictionary and handles
				 non-words with asterisks
   Nov 18, 2016  Finished program and documentation
   @endverbatim
 *
 *****************************************************************************/

#include<iostream>
#include<cctype>
#include<iomanip>
#include<fstream>
#include<vector>
#include<string>
#include<algorithm>
 
using namespace std;


/*******************************************************************************
*                         Function Prototypes
******************************************************************************/

void readAndWrite(ifstream &fin, vector<string> v, ofstream &fout);
void permute(int *p, int *used, int size, int pos, string &word, 
	vector<string> dict, bool &check);
void fixWord(string &word, vector<string> dict, bool &check);


/***************************************************************************//**
 * @author Christian Fattig
 *
 * @par Description:
 * This is the starting point to the program. It will use the files that 
 * are given in the command prompt and open them up for editing. If
 * an invalid argument count is given, an error will be printed as well as 
 * a usage statement followed by the termination of the program. Once the
 * files have been opened, a vector will be filled from dictionary.txt and
 * sorted. The reading and writing then occurs followed by the closing of
 * any remaining opened files.
 *
 * @param[in] argc - the number of arguments from the command prompt.
 * @param[in] argv - a 2d array of characters containing the arguments.
 *
 * @returns 0 Program ran sucessfully
 * @returns 1 Program failed, invalid argument count was given
 *
 ******************************************************************************/
int main(int argc, char** argv)
{
	ifstream fin, finD;// typo file and dictionary file respectively
	ofstream fout;// reset file
	vector<string> words;// used to hold dictionary
	string word;// used to read a word in to push onto the vector

	if (argc != 3)// checks for correct argument count
	{
		cout << "Invalid arguments given!" << endl 
			<< "Usage: prog3.exe message.tgc message.rst" << endl;
		return 1;
	}

	fin.open(argv[1]);// opens necessary files
	finD.open("dictionary.txt");
	fout.open(argv[2]);

	if (!fin || !fout || !finD)// checks that files opened
	{
		cout << "Error, one or more files did not open!" << endl;
		fin.close();
		fout.close();
		finD.close();
		return 1;
	}
	
	while (finD)// fills vector with words from dictionary.txt
	{
		finD >> word;
		words.push_back(word);
	}

	sort(words.begin(), words.end());// sorts vector
	finD.close();
	
	readAndWrite(fin, words, fout);// fixes typo file and writes to other file

	fin.close();
	fout.close();

	return 0;
}


/***************************************************************************//**
 * @author Christian Fattig
 *
 * @par Description:
 * This function handles reading from one file and writing to another
 * while preparing the words to be unscrambled. It individually reads in 
 * each word and preserves the punctuation and capitalization. Once the word
 * is prepared, it is sent to be unscrambled. The word is then restored of 
 * punctuation and capitalization if it had any and if a word was not found
 * it will be given an aterisk before it. It is then written to the reset
 * file while preserving spacing and newlines. This happens untill the 
 * entire typo file has been processed.
 *
 * @param[in,out]  fin - the tyop file to be read.
 * @param[in]        v - the vector holding dictionary.txt.
 * @param[in,out] fout - the file to be written to.
 *
 ******************************************************************************/
void readAndWrite(ifstream &fin, vector<string> v, ofstream &fout)
{
	string word;//word that is being processed
	bool check = false;//checks if a word was found or not
	bool cap = false;//checks if the word was capitalized
	bool punctCheck = false;//checks if word had punctuation
	char punct;//stores punctuation mark if any

	while (fin)//reads till end of file
	{
		check = false;//resets checks for each new word
		cap = false;
		punctCheck = false;

		fin >> word;

		if (ispunct(word.back()))//preserves punctuation
		{
			punctCheck = true;
			punct = word.back();
			word.pop_back();
		}

		if (isupper(word.at(0)))//preserves capitalization
		{
			cap = true;
			word.at(0) = tolower(word.at(0));
		}

		fixWord(word, v, check);//unscrambles the word given or check is false

		if (cap == true)//restores capitalization
			word.at(0) = toupper(word.at(0));
		if (check == false)//adds asterisk if word was not found
			word = '*' + word;
		if (punctCheck == true)//restores punctuation
			word.push_back(punct);

		cout << word << endl;
		fout << word;

		//preserves spacing and newlines
		for (int i = 0; fin.peek() == ' ' || fin.peek() == '\n'; i++)
		{
			if (fin.peek() == ' ')
				fout << ' ';
			if (fin.peek() == '\n')
				fout << endl;
			fin.ignore();
		}
	}
}


/***************************************************************************//**
 * @author Christian Fattig
 *
 * @par Description:
 * This function prepares for the permute function and then unscrambles the 
 * word given. It starts out by allocating 2 dynamic arrays that are the 
 * size of the word given. The function checks to see that it got the
 * memory needed and exits if it didn't. Next a vector is created and filled
 * from the dictionary vector with words that have the same length as the 
 * word given as well as the same beginnning and ending characters. From 
 * there the word is unscrambled and the dynamicly allocated arrays are then
 * freed up.
 *
 * @param[in,out] word - the string to be unscrambled.
 * @param[in]     dict - the vector holding dictionary.txt.
 * @param[out]   check - used to see if a word was found.
 *
 ******************************************************************************/
void fixWord(string &word, vector<string> dict, bool &check)
{
	int *p = nullptr;//used to store indexes
	int *u = nullptr;//stores index attempts
	vector<string> criteria;//holds dictionary words that match criteria

	p = new(nothrow) int[word.size()];//memory allocation
	u = new(nothrow) int[word.size()];

	if (p == nullptr || u == nullptr)//checks for memory allocation
	{
		cout << "Memory allocation error!" << endl;
		exit(0);
	}

	for (int i = 0; i < (int)word.size(); i++)// fill u-array with 0s
		u[i] = 0;
	
	if (word.size() > 3)
	{
		//this loop creates a more narrowed vector to search based on
		//matching criteria
		for (int i = 0; i < (int)dict.size(); i++)
			if (dict.at(i).size() == word.size() && dict.at(i).at(0)
				== word.at(0) && dict.at(i).at(word.size() - 1)
				== word.at(word.size() - 1))
				criteria.push_back(dict.at(i));
				
		permute(p, u, word.size(), 0, word, criteria, check);//unscrambles word
	}
	else//doesn't permute words less than 4 characters
		check = true;

	delete [] p;//frees up memory
	delete [] u;
}


/***************************************************************************//**
 * @author Christian Fattig
 *
 * @par Description:
 * This function permutes every possible combination of indexes and 
 * compares them to a vector for possible words untill it finds the 
 * first match. It does this by recursively calling itself and checking for
 * a match and if it does, the function gets out of the recursive loop.
 * Once the match is found, the word is returned to the readAndWrite function.
 *
 * @param[in,out]	 p - holds the indexes of a string.
 * @param[in,out] used - holds 1s and 0s for attempts.
 * @param[in]     size - size of the word.
 * @param[in,out]  pos - holds the current position.
 * @param[in,out] word - the string to be unscrambled.
 * @param[in]     dict - the vector holding a narrowed dictionary.txt.
 * @param[out]   check - used to see if a word was found.
 *
 ******************************************************************************/
void permute(int *p, int *used, int size, int pos, string &word,
	vector<string> dict, bool &check)
{ 
	string fixedWord;//holds the assembled word from new indexes

	if (check == true)//if word was found, stop permuting
		return;
	if (pos == size)//once enough indexes are assembled
	{
		if (p[0] == 0 && p[pos - 1] == size - 1)//first and last index check
		{
			for (int i = 0; i < size; i++)//assembles the word from indexes
				fixedWord += word.at(p[i]);
			
			//searches for the word in the vector
			if (binary_search(dict.begin(), dict.end(), fixedWord) == true)
			{
				check = true;//found word
				word = fixedWord;//return word
				return;
			}
		}
		return;
	}

	for (int i = 0; i < size; i++)//permutes all combos of indexes
	{
		if (used[i] == 0)//if number wasn't used yet
		{
			p[pos] = i;//number to use
			used[i] = 1;//number is used
			permute(p, used, size, pos + 1, word, dict, check);//fill next index
			used[i] = 0;//reset
		}
	}
}
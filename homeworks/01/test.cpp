#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;
#endif /* __PROGTEST__ */

std::vector<int> fibonacciSequenceLessOrEqual(int n)
{
  std::vector<int> fib;
  fib.push_back(1);
  fib.push_back(2);

  while (fib.back() <= n)
  {
    fib.push_back(fib.back() + fib[fib.size() - 2]);
  }

  fib.pop_back(); // remove the last element if it's greater than n

  return fib;
}

std::string fibonacciEncoding(int n)
{
  n += 1;
  std::vector<int> fib = fibonacciSequenceLessOrEqual(n);
  int index = fib.size() - 1;
  std::string codeword(index + 3, ' ');

  if (n == 1)
  {
    codeword[0] = '1';
    codeword[1] = '1';
  }

  int i = index;
  while (n)
  {
    codeword[i] = '1';
    n = n - fib[i];
    i = i - 1;
    while (i >= 0 && fib[i] > n)
    {
      codeword[i] = '0';
      i = i - 1;
    }
  }

  codeword[index + 1] = '1';
  codeword[index + 2] = '\0';
  return codeword;
}

// Read and store UTF-8 encoded file as vector of bytes
bool store_utf8(const char *infile, std::vector<unsigned char> &file_contents)
{
  ifstream input_file(infile, ios::binary);
  if (!input_file.is_open())
  {
    std::cerr << "Error: could not open file '" << infile << std::endl;
    return false;
  }

  file_contents = std::vector<unsigned char>(std::istreambuf_iterator<char>(input_file), {});
  return true;
}

// converting UTF-8 encoding to Unicode code points
bool utf8_to_unicode(const std::vector<unsigned char> &utf_8, vector<int> &unicode_vec)
{
  for (auto it = utf_8.begin(); it != utf_8.end();)
  {
    int code_point = 0;
    unsigned char byte = *it++;

    if (byte <= 0x7F)
    {
      code_point = byte;
    }
    else if (byte <= 0xBF)
    {
      // Invalid continuation byte
      break;
    }
    else if (byte <= 0xDF)
    {
      // 2-byte sequence
      code_point = ((byte & 0x1F) << 6) | (*it++ & 0x3F);
    }
    else if (byte <= 0xEF)
    {
      // 3-byte sequence
      code_point = ((byte & 0x0F) << 12) | ((*it++ & 0x3F) << 6) | (*it++ & 0x3F);
    }
    else if (byte <= 0xF7)
    {
      // 4-byte sequence
      code_point = ((byte & 0x07) << 18) | ((*it++ & 0x3F) << 12) | ((*it++ & 0x3F) << 6) | (*it++ & 0x3F);
    }
    else
    {
      // Invalid byte
      break;
    }

    unicode_vec.push_back(code_point);
  }
  return true;
}

bool utf8ToFibonacci(const char *inFile, const char *outFile)
{
  std::vector<unsigned char> utf8;
  std::vector<int> unicode;

  if (!store_utf8(inFile, utf8))
  {
    return false;
  }
  utf8_to_unicode(utf8, unicode);

  if (unicode.empty())
  {
    std::cerr << "Error: no valid Unicode code points found in input file." << std::endl;
    return false;
  }

  string concat;
  for (auto &x : unicode)
  {
    concat += fibonacciEncoding(x);
  }

  string substr;

  int i = 0;
  while (i < concat.size())
  {
    substr = concat.substr(i, 8);
    i += 8;

    if (substr.size() < 8)
    {
      substr.append(8 - substr.size(), '0');
    }

    cout << substr << endl;
  }

  return 0;
}

bool fibonacciToUtf8(const char *inFile, const char *outFile)
{
  // todo
}

#ifndef __PROGTEST__

bool identicalFiles(const char *file1, const char *file2)
{
  // todo
}

int main(int argc, char *argv[])
{
  utf8ToFibonacci("example/src_0.utf8", "output.fib");
  // assert(utf8ToFibonacci("example/src_0.utf8", "output.fib") && identicalFiles("output.fib", "example/dst_0.fib"));
  // assert(utf8ToFibonacci("example/src_1.utf8", "output.fib") && identicalFiles("output.fib", "example/dst_1.fib"));
  // assert(utf8ToFibonacci("example/src_2.utf8", "output.fib") && identicalFiles("output.fib", "example/dst_2.fib"));
  // assert(utf8ToFibonacci("example/src_3.utf8", "output.fib") && identicalFiles("output.fib", "example/dst_3.fib"));
  // assert(utf8ToFibonacci("example/src_4.utf8", "output.fib") && identicalFiles("output.fib", "example/dst_4.fib"));
  // assert(!utf8ToFibonacci("example/src_5.utf8", "output.fib"));
  // assert(fibonacciToUtf8("example/src_6.fib", "output.utf8") && identicalFiles("output.utf8", "example/dst_6.utf8"));
  // assert(fibonacciToUtf8("example/src_7.fib", "output.utf8") && identicalFiles("output.utf8", "example/dst_7.utf8"));
  // assert(fibonacciToUtf8("example/src_8.fib", "output.utf8") && identicalFiles("output.utf8", "example/dst_8.utf8"));
  // assert(fibonacciToUtf8("example/src_9.fib", "output.utf8") && identicalFiles("output.utf8", "example/dst_9.utf8"));
  // assert(fibonacciToUtf8("example/src_10.fib", "output.utf8") && identicalFiles("output.utf8", "example/dst_10.utf8"));
  // assert(!fibonacciToUtf8("example/src_11.fib", "output.utf8"));

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */

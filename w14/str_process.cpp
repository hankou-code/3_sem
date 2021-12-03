// 5) Записать три слова, введҷнные с клавиатуры в вектор, вывести их
// на экран, отсортированные по алфавиту, определить сколько в первом
// слове букв о. Тут можно пользоваться sort.

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#define NUM_STRS 3
#define SEARCH_STR "o"

using namespace std;

int main() {

	vector<string> text;

	for (int i = 0; i < NUM_STRS; i++) {
		string buf;
		cin >> buf;
		text.push_back(buf);
	}

  std::sort(begin(text), 
            end(text), 
            [](string a, string b) { return a < b; });

	cout << "----------------" << endl;
	for (auto str: text) {
		cout << str << endl;
	}

	std::string s = text[0];
	int num = 0;
	for (size_t pos = 0; (pos = s.find(SEARCH_STR, pos)) != string::npos; num++, pos++) {
	}
	
	std::cout << num << "\n";

	return 0;
}

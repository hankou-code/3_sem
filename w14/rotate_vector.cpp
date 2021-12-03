// 2) перевернуть vector задом наперед. сделать это в функции, передать
// по ссылке. полученный вектор вывести в main

#include <iostream>
#include <vector>

using namespace std;

void rotate_vec(vector<int>& vec) {
	for (int i = 0; i < vec.size() / 2; i++) {
		int buf = vec[i];
		vec[i] = vec[vec.size() - i - 1];
		vec[vec.size() - i - 1] = buf;

	}
}


int main() {
	vector<int> vec = {1, 4, 6, 8, 7, 3, 2, 1};

	rotate_vec(vec);

	for (auto v: vec){
		cout << v << " ";
	}
	return 0;
}
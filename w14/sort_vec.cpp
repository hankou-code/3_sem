// 3) отсортировать vector по убыванию. сделать это в функции, пе-
// редать по ссылке. полученный вектор вывести в main. с помощью sort
// нельзя, ручками.

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void heap_sort(vector<int>& values) {
	make_heap(values.begin(), values.end());
	for (auto i = values.end(); i != values.begin(); --i) {
		pop_heap(values.begin(), i);
	}
}

void rotate_vec(vector<int>& vec) {
	for (int i = 0; i < vec.size() / 2; i++) {
		int buf = vec[i];
		vec[i] = vec[vec.size() - i - 1];
		vec[vec.size() - i - 1] = buf;

	}
}

int main() {
	vector<int> vec = {1, 4, 6, 8, 7, 3, 2, 1};
	heap_sort(vec);
	rotate_vec(vec);
	for (auto v: vec){
		cout << v << " ";
	}
	return 0;
}
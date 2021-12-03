// 4) Найти номер наименьшего элемента вектора, большего заданного
// числа. На вход подается это число, длина вектора и сам вектор.

#include <iostream>
#include <vector>
#include <limits>

using namespace std;

int main() {

	int boundary_num, num_digits;
	cin >> boundary_num;
	cin >> num_digits;

	int min = numeric_limits<int>::max();

	int id = -1;
	for (int i = 0; i < num_digits; i++) {
		double digit;
		cin >> digit;
		
		if (digit < min && digit > boundary_num) {
			min = digit;
			id = i + 1;
		}
	}

	if (id == -1) {
		cout << "Bad input" << endl;
	} else {
		cout << "id is: " << id << " number is: " << min << endl;
	}

	return 0;
}

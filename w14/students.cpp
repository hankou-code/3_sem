// 1) Есть N студентов, у каждого есть оценка (double). На вход подается
// натуральное число N и потом оценка каждого студента. Вывести номера
// студентов с оценкой выше среднего.

#include <iostream>
#include <vector>

using namespace std;

int main() {

	int num_studs;
	cin >> num_studs; 

	double sum_grades = .0;
	vector<double> rating; 
	for (int i = 0; i < num_studs; i++) {
		double grade;
		cin >> grade;
		rating.push_back(grade);
		sum_grades += grade;
	}

	double avg_grade = sum_grades / num_studs;

	cout << "avg grade: " << avg_grade << endl;
	for (int i = 0; i < num_studs; i++) {
		if (rating[i] > avg_grade) {
			cout << i + 1 << " "; // student number
		}
	}

	return 0;
}
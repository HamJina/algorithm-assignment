#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

void merge(vector<vector<string>>& A, int s, int m, int e) {
    vector<vector<string>> tmp(e - s + 1, vector<string>(2)); // �ӽ� �迭 ����
    int i = s, j = m + 1, k = 0;

    // �� �κ� �迭�� ���ϸ鼭 ��ġ��
    while (i <= m && j <= e) {
        if (A[i][1] < A[j][1] || (A[i][1] == A[j][1] && A[i][0] < A[j][0])) { // ������ �������� ����
            tmp[k] = A[i];
            ++i;
        }
        else {
            tmp[k] = A[j];
            ++j;
        }
        ++k;
    }

    // ���� ��ҵ� ����
    while (i <= m) {
        tmp[k] = A[i];
        ++i;
        ++k;
    }
    while (j <= e) {
        tmp[k] = A[j];
        ++j;
        ++k;
    }

    // �ӽ� �迭�� ���� �迭�� ����
    for (int idx = s; idx <= e; ++idx) {
        A[idx] = tmp[idx - s];
    }
}

void mergeSort(vector<vector<string>>& A, int s, int e) {
    if (s >= e) return; // ������ ��Ұ� �ϳ��� ���

    int m = s + (e - s) / 2;
    mergeSort(A, s, m); // ���� �κ� �迭 ����
    mergeSort(A, m + 1, e); // ������ �κ� �迭 ����
    merge(A, s, m, e); // �պ�
}

int main() {
    vector<vector<string>> v; // 2���� ���� ����

    ifstream input("birthday.in");

    if (input.fail()) {
        cout << "������ �� �� �����ϴ�." << endl;
        return 1;
    }

    string name, birthday;
    // ���Ͽ��� �����͸� �о�ͼ� ���Ϳ� ����
    while (input >> name >> birthday) {
        vector<string> row = { name, birthday }; // �� �� ����
        v.push_back(row); // ���Ϳ� �� �߰�
    }

    input.close();

    // ���� ���� �Լ� �����ϱ�
    mergeSort(v, 0, v.size() - 1);

    ofstream output;
    output.open("birthday.out");
    
    // ���ĵ� ������ ���
    for (const auto& row : v) {
        output << row[0] << " " << row[1] << endl;
    }

    output.close();

    return 0;
}

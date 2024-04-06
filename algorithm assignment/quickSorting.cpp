#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

void swap(vector<vector<string>>& A, int i, int j) { // vector<vector<string>>�� ���ڷ� ����
    vector<string> temp = A[i];
    A[i] = A[j];
    A[j] = temp;
}

int partition(vector<vector<string>>& A, int S, int E) {
    if (S + 1 == E) {
        if (stoi(A[S][1].substr(0, 2)) > stoi(A[E][1].substr(0, 2))) {
            swap(A, S, E);
        }
        return E;
    }

    int M = (S + E) / 2;
    swap(A, S, M);
    int pivot = stoi(A[S][1].substr(0, 2));
    int i = S + 1, j = E;

    while (i <= j) {
        while (pivot < stoi(A[j][1].substr(0, 2)) && j > 0) {
            j--;
        }
        while (pivot > stoi(A[i][1].substr(0, 2)) && i < A.size() - 1) {
            i++;
        }
        if (i <= j) {
            swap(A, i++, j--);
        }
    }

    swap(A, S, j); // pivot�� ��ġ�� ��Ȯ�� �Űܾ� ��
    return j;
}

void quickSort(vector<vector<string>>& A, int S, int E) {
    if (S >= E) {
        return;
    }

    int pivot = partition(A, S, E);
    quickSort(A, S, pivot - 1);
    quickSort(A, pivot + 1, E);
}



int main()
{
    vector<vector<string>> v; // 2���� ���� ����

    ifstream input("birthday.txt");

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

    //��������� �ڵ���� ������ ���� �̸��� ���� ������ �о 2���� ���Ϳ� ������ �����̴�. 
    //���� ������ �������·� �� ��ȯ�� �ϰ� �������� �̿��Ͽ� ������ �ϸ� �ȴ�. 

    //������ �Լ� �����ϱ�
    quickSort(v, 0, v.size() - 1);

    ofstream output;

    output.open("birthdayQSorting");

    for (const auto& row : v) {
        for (const auto& data : row) {
            output << data << " ";
        }
        output << endl;
    }

    output.close();

    return 0;
}

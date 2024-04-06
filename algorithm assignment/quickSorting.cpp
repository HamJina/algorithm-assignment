#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

void swap(vector<vector<string>>& A, int i, int j) { // vector<vector<string>>을 인자로 받음
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

    swap(A, S, j); // pivot의 위치를 정확히 옮겨야 함
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
    vector<vector<string>> v; // 2차원 벡터 생성

    ifstream input("birthday.txt");

    if (input.fail()) {
        cout << "파일을 열 수 없습니다." << endl;
        return 1;
    }

    string name, birthday;
    // 파일에서 데이터를 읽어와서 벡터에 저장
    while (input >> name >> birthday) {
        vector<string> row = { name, birthday }; // 한 행 생성
        v.push_back(row); // 벡터에 행 추가
    }

    input.close();

    //여기까지는 코드상의 문제는 없고 이름과 생일 정보를 읽어서 2차원 벡터에 저장한 상태이다. 
    //이제 생일을 정수형태로 형 변환을 하고 퀵정렬을 이용하여 정렬을 하면 된다. 

    //퀵정렬 함수 실행하기
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

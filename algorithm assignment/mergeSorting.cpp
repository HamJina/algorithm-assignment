#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

void merge(vector<vector<string>>& A, int s, int m, int e) {
    vector<vector<string>> tmp(e - s + 1, vector<string>(2)); // 임시 배열 생성
    int i = s, j = m + 1, k = 0;

    // 두 부분 배열을 비교하면서 합치기
    while (i <= m && j <= e) {
        if (A[i][1] < A[j][1] || (A[i][1] == A[j][1] && A[i][0] < A[j][0])) { // 생일을 기준으로 정렬
            tmp[k] = A[i];
            ++i;
        }
        else {
            tmp[k] = A[j];
            ++j;
        }
        ++k;
    }

    // 남은 요소들 복사
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

    // 임시 배열을 원래 배열로 복사
    for (int idx = s; idx <= e; ++idx) {
        A[idx] = tmp[idx - s];
    }
}

void mergeSort(vector<vector<string>>& A, int s, int e) {
    if (s >= e) return; // 범위에 요소가 하나인 경우

    int m = s + (e - s) / 2;
    mergeSort(A, s, m); // 왼쪽 부분 배열 정렬
    mergeSort(A, m + 1, e); // 오른쪽 부분 배열 정렬
    merge(A, s, m, e); // 합병
}

int main() {
    vector<vector<string>> v; // 2차원 벡터 생성

    ifstream input("birthday.in");

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

    // 병합 정렬 함수 실행하기
    mergeSort(v, 0, v.size() - 1);

    ofstream output;
    output.open("birthday.out");
    
    // 정렬된 데이터 출력
    for (const auto& row : v) {
        output << row[0] << " " << row[1] << endl;
    }

    output.close();

    return 0;
}

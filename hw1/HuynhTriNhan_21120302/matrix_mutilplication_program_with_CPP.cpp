#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <chrono>
#include <sstream>
#include <ctime>

using namespace std;
using namespace std::chrono;
int main()
{
    vector<int> len_matrix = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    vector<double> times;

    for (auto size : len_matrix)
    {
        // Đọc ma trận từ file, nếu không có thì quăng lỗi
        ifstream file1;
        ifstream file2;
        try
        {
            file1.open("datas/matrix_" + to_string(size) + "_1.txt");
            file2.open("datas/matrix_" + to_string(size) + "_2.txt");
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }

        // vector 2 chiều để chứa kích thược mảng
        vector<vector<double>> matrix1(size, vector<double>(size));
        vector<vector<double>> matrix2(size, vector<double>(size));

        for (int i = 0; i < size; i++)
        {
            string line1, line2;
            getline(file1, line1);
            getline(file2, line2);

            stringstream ss1(line1), ss2(line2);

            for (int j = 0; j < size; j++)
            {
                string val1, val2;
                if (j != size - 1)
                {
                    getline(ss1, val1, ',');
                    getline(ss2, val2, ',');
                }
                else
                {
                    getline(ss1, val1);
                    getline(ss2, val2);
                }
                // Chuyển đổi giá trị từ string sang float
                try
                {
                    stof(val1);
                    stof(val2);
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                }
                matrix1[i][j] = stof(val1);
                matrix2[i][j] = stof(val2);
            }
        }
        file1.close();
        file2.close();

        // Thực hiện tính toán
        auto start = high_resolution_clock::now();
        // Tạo ma trận kết quả với toàn phần từ 0
        vector<vector<double>> result(size, vector<double>(size, 0));
        // Nhân 2 ma trận bằng phương pháp dòng nhân cột truyền thông
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                double sum = 0;
                for (int k = 0; k < size; k++)
                {
                    sum += matrix1[i][k] * matrix2[k][j];
                }
                result[i][j] = sum;
            }
        }
        auto end = high_resolution_clock::now();

        // Lưu kết quả chạy vào file
        ofstream outfile("results/result_" + to_string(size) + "_cpp.txt");
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                outfile << result[i][j] << ",";
            }
            outfile << endl;
        }
        outfile.close();

        // Lưu thời gian chạy vào file
        double elapsed_time = duration_cast<milliseconds>(end - start).count();
        times.push_back(elapsed_time);
    }
    // Lưu kết quả thời gian chạy vào file, Nếu không có file thì tạo mới
    ofstream timefile;
    timefile.open("times/time_cpp.txt");
    for (double time : times)
    {
        timefile << time << endl;
    }
    timefile.close();
    cout << "Done!" << endl;
    return 0;
}
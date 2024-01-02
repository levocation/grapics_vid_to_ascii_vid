#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <Windows.h>

using namespace cv;
using namespace std;

int frame_num = 0;

void gotoxy(int x, int y)
{
    COORD pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void fullscreen() {
    SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_FULLSCREEN_MODE, 0);
}

void setConsoleSize(int width, int height)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD NewSBSize = { width, height };
    SMALL_RECT DisplayArea = { 0, 0, 0, 0 };

    SetConsoleScreenBufferSize(hOut, NewSBSize);

    DisplayArea.Right = width - 1;
    DisplayArea.Bottom = height - 1;

    SetConsoleWindowInfo(hOut, TRUE, &DisplayArea);
}

void setFontSize(int font_size = 20) {
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    // �ܼ� �۲� ������ �����ϴ� ����ü
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = font_size; // �۲� ũ�� ����
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas"); // �۲� �̸� ����
}

int openVid() {
    // ���� ���� ����
    VideoCapture cap("Sample.mp4");
    if (!cap.isOpened())
    {
        // ���� ������ ���ų�, ���������� return -1;
        return -1;
    }

    frame_num = 0;
    while (1)
    {
        Mat frame;
        cap >> frame;
        // video���� �� �������� �о��
        // == cap.read(frame)

        // �о�� �������� ������ ������ ���̶�� ���̹Ƿ� break
        if (frame.empty())
            break;
        string path = "img_save/";
        string filename = path + "frame" + to_string(frame_num) + ".jpg"; // �̹��� �̸� ����
        imwrite(filename, frame); // �̹��� ����
        frame_num++;
    }

    // ���� ������ �ݰ�, ��� �����츦 �����մϴ�
    cap.release();
    destroyAllWindows();
    return 0;
}

char pixelToAscii(int pixel_value) // �ȼ��� �ƽ�Ű������ ��ȯ
{
    string ascii_chars = "@%#*+=-:. ";
    return ascii_chars[pixel_value / (256 / ascii_chars.size())];
}

int imgToAscii(string img_path, string save_path) // �̹����� �ƽ�Ű�� ��ȯ
{
	Mat img = imread(img_path, IMREAD_GRAYSCALE); // �̹����� �׷��̽����Ϸ� �о��
    if (img.empty())
    {
		cout << "Could not read the image: " << img_path << endl;
		return 1;
	}
    resize(img, img, Size(img.cols / 10, img.rows / 10)); // �̹��� ũ�� ����
    // cout << "Open\n";
	ofstream fout(save_path); // ������ ���� ����

    #pragma omp parallel for // ����ó��
    for (int i = 0; i < img.rows; i++) // �̹����� ��� �ȼ��� �ƽ�Ű�� ��ȯ
    {
        for (int j = 0; j < img.cols; j++) // �̹����� ��� �ȼ��� �ƽ�Ű�� ��ȯ
        {
			fout << pixelToAscii(img.at<uchar>(i, j));
		}
		fout << endl;
	}
	fout.close(); // ���� �ݱ�
	return 0;
}

void printAsciiArt(string filename)
{
    ifstream file(filename);
    string line;

    if (file.is_open())
    {
        while (getline(file, line))
        {
            cout << line << '\n';
        }
        file.close();
    }
    else
    {
        cout << "Unable to open file: " << filename << endl;
    }
}

void show_ascii_vid(string filename) {
    printAsciiArt(filename);
    return;
}

int main()
{
    fullscreen();
    setFontSize(20);

    int openVidType = openVid();
    if (openVidType == -1) {
        cout << "Error opening video stream or file" << endl;
        return 0;
    }
    for (int i = 0; i < frame_num; i++) {
        imgToAscii("img_save/frame" + to_string(i) + ".jpg", "ascii_save/frame" + to_string(i) + ".txt");
        imgToAscii("img_save/frame" + to_string(i) + ".jpg", "ascii_save/frame" + to_string(i) + ".txt");
    }
    // frame_num = 900;
    system("cls");
    setFontSize(5);
    Sleep(2000);
    for (int i = 0; i < frame_num; i++) {
        gotoxy(0, 0);
        show_ascii_vid("ascii_save/frame" + to_string(i) + ".txt");
        //Sleep(3);
    }

    return 0;
}

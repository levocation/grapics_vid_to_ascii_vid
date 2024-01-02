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

    // 콘솔 글꼴 정보를 설정하는 구조체
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = font_size; // 글꼴 크기 설정
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas"); // 글꼴 이름 설정
}

int openVid() {
    // 비디오 파일 열기
    VideoCapture cap("Sample.mp4");
    if (!cap.isOpened())
    {
        // 비디오 파일이 없거나, 깨져있으면 return -1;
        return -1;
    }

    frame_num = 0;
    while (1)
    {
        Mat frame;
        cap >> frame;
        // video에서 한 프레임을 읽어옴
        // == cap.read(frame)

        // 읽어온 프레임이 없으면 영상의 끝이라는 뜻이므로 break
        if (frame.empty())
            break;
        string path = "img_save/";
        string filename = path + "frame" + to_string(frame_num) + ".jpg"; // 이미지 이름 설정
        imwrite(filename, frame); // 이미지 저장
        frame_num++;
    }

    // 비디오 파일을 닫고, 모든 윈도우를 제거합니다
    cap.release();
    destroyAllWindows();
    return 0;
}

char pixelToAscii(int pixel_value) // 픽셀을 아스키값으로 변환
{
    string ascii_chars = "@%#*+=-:. ";
    return ascii_chars[pixel_value / (256 / ascii_chars.size())];
}

int imgToAscii(string img_path, string save_path) // 이미지를 아스키로 변환
{
	Mat img = imread(img_path, IMREAD_GRAYSCALE); // 이미지를 그레이스케일로 읽어옴
    if (img.empty())
    {
		cout << "Could not read the image: " << img_path << endl;
		return 1;
	}
    resize(img, img, Size(img.cols / 10, img.rows / 10)); // 이미지 크기 조정
    // cout << "Open\n";
	ofstream fout(save_path); // 저장할 파일 열기

    #pragma omp parallel for // 병렬처리
    for (int i = 0; i < img.rows; i++) // 이미지의 모든 픽셀을 아스키로 변환
    {
        for (int j = 0; j < img.cols; j++) // 이미지의 모든 픽셀을 아스키로 변환
        {
			fout << pixelToAscii(img.at<uchar>(i, j));
		}
		fout << endl;
	}
	fout.close(); // 파일 닫기
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

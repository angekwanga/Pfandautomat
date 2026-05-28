#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

/*#include <vector>
#include <iostream>
#include <algorithm>

#include "rappel.h"*/

/*std::vector<Student> V{};
    std::string name{};
    double note{0.0};
    while(name != "finish") {
        std::cout << "Name:";
        std::cin >>   name;
        if(name == "finish" || name.empty())
            break;
        std::cout << "Note:";
        std::cin >> note;

        Student s{name, note};
        V.push_back(s);
    }

    if(!V.empty()) {
        std::sort(V.begin(), V.end());
        for(auto& val : V)
            std::cout << val.getName() << std::endl;
    }*/
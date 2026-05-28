cmake_minimum_required(VERSION 3.16)
project(Pfandautomat_SE2 VERSION 1.0 LANGUAGES CXX)
find_package(Qt6 REQUIRED COMPONENTS Widgets Core)
qt_standard_project_setup()
# Qt cherche les .ui dans ce dossier
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)
qt_add_executable(Pfandautomat_SE2
    # Sources
    src/main.cpp
    src/mainwindow.cpp
    src/personal/personalbereich.cpp
    src/personal/personalprotokoll.cpp
    src/logindialog.cpp
    src/displaymonitor.cpp
    src/qrdialog.cpp
        src/transaktionservice.cpp
    # Sensorik
    src/sensorik/lichtschranke.cpp
    src/sensorik/scanner.cpp
    src/sensorik/gewichtsensor.cpp
    src/sensorik/sensorkontroller.cpp
    # Mechanik
    src/mechanik/foerderband.cpp
    src/mechanik/dreheinheit.cpp
    src/mechanik/sortierweiche.cpp
    src/mechanik/kompressor.cpp
    # Lagerung
    src/lagerung/pressebehaelter.cpp
    src/lagerung/sammelbehaelter.cpp
    # Headers
    include/mainwindow.h
    include/personal/personalbereich.h
    include/personal/personalprotokoll.h
    include/logindialog.h
    include/displaymonitor.h
    include/exceptions.h
    include/enumtype.h
    include/domain.h
    include/qrdialog.h
    include/transaktionservice.h
    # Sensorik Headers
    include/sensorik/lichtschranke.h
    include/sensorik/scanner.h
    include/sensorik/gewichtsensor.h
    # Mechanik Headers
    include/mechanik/foerderband.h
    include/mechanik/dreheinheit.h
    include/mechanik/sortierweiche.h
    include/mechanik/kompressor.h
    # Lagerung Headers
    include/lagerung/behaelter.h
    include/lagerung/pressebehaelter.h
    include/lagerung/sammelbehaelter.h
    # UI-Dateien
    ui/mainwindow.ui
    ui/logindialog.ui
    ui/displaymonitor.ui
    ui/qrdialog.ui
    include/export.h
    src/export.cpp
    include/sensorik/sensorik.h

    include/lagerung.h
    include/sensorik/sensorkontroller.h
    include/sensorik/sensorkontroller.h

    include/mechanik/bandkontroller.h
    src/mechanik/bandkontroller.cpp
    include/observer.h
    include/mechanik/mechanik.h
    include/hauptkontroller.h
    src/hauptkontroller.cpp
    include/mechanik/mechanikkontroller.h
    src/mechanik/mechanikkontroller.cpp


)
set_target_properties(Pfandautomat_SE2 PROPERTIES
    WIN32_EXECUTABLE TRUE
)
file(GLOB IMAGE_FILES "img/*.png" "img/*.svg")
qt_add_resources(Pfandautomat_SE2 "images"
    PREFIX "/"
    BASE "img"
    FILES ${IMAGE_FILES}
)
target_include_directories(Pfandautomat_SE2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(Pfandautomat_SE2 PRIVATE
    Qt6::Widgets
    Qt6::Core
)
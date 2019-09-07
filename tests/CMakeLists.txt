add_executable(GeometryVisualisatorTest.exe GeometryVisualisatorTest.cpp)
target_link_libraries(GeometryVisualisatorTest.exe eventDisplay JPetFramework::JPetFramework Boost::filesystem Boost::unit_test_framework ROOT::Rint ROOT::Gui ROOT::Geom ROOT::Graf3d)

add_executable(DataProcessorTest.exe DataProcessorTest.cpp)
target_link_libraries(DataProcessorTest.exe eventDisplay JPetFramework::JPetFramework Boost::filesystem Boost::unit_test_framework ROOT::Rint ROOT::Gui ROOT::Geom ROOT::Graf3d)

add_executable(EventDisplayTest.exe EventDisplayTest.cpp)
target_link_libraries(EventDisplayTest.exe eventDisplay JPetFramework::JPetFramework Boost::filesystem Boost::unit_test_framework ROOT::Rint ROOT::Gui ROOT::Geom ROOT::Graf3d)

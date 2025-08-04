from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class MyProjectConan(ConanFile):
    name = "myproject"
    version = "1.0"
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps", "CMakeToolchain"
    exports_sources = "CMakeLists.txt", "src/*"
    requires = (
        "sqlitecpp/3.3.2"
    )

    # def layout(self):
    #     cmake_layout(self)

    # def build(self):
    #     cmake = CMake(self)
    #     cmake.configure()
    #     cmake.build()

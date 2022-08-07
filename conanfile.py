from conans import ConanFile, CMake


class CarmaBypass(ConanFile):
    name = "rec2"
    description = "Carmageddon II, revisited"
    settings = "os", "arch", "compiler", "build_type"
    generators = "cmake"
    options = {
        "hook": [True, False],
    }
    default_options = {
        "hook": True,
        "libtiff:lzma": False,
        "libtiff:jpeg": False,
        "libtiff:zlib": False,
        "libtiff:libdeflate": False,
        "libtiff:zstd": False,
        "libtiff:jbig": False,
        "libtiff:webp": False,
        "libtiff:cxx":  False,
    }

    def requirements(self):
        if self.options.hook:
            self.requires("detours/cci.20220630")
        self.requires("libtiff/4.4.0")

    def validate(self):
        assert self.settings.os == "Windows"
        assert self.settings.arch == "x86"

    def build(self):
        cmake = CMake(self)
        cmake.generator = "Ninja"
        cmake.definitions["C2_STANDALONE"] = not self.options.hook
        cmake.configure()
        cmake.build()

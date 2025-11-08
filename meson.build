project(
    'libspatialaudio',
    'cpp',
    version : '0.4.0',
    meson_version : '>= 1.1.0',
    default_options : ['warning_level=3', 'cpp_std=c++14'],
)

# Library (API/ABI) version
# Do not forget to update the major vesion when breaking ABI.
spatialaudio_lib_version = '2.0.0'

dependencies = []
conf_data = configuration_data()

libmysofa_dep = dependency('libmysofa', required : get_option('libmysofa'))
if libmysofa_dep.found()
    conf_data.set('SPATIALAUDIO_SUPPORTS_SOFA', 1)
    dependencies += libmysofa_dep
endif

if get_option('mit_hrtf').allowed()
    conf_data.set('SPATIALAUDIO_SUPPORTS_MIT_HRTF', 1)
    add_languages('c', native: false)
endif

if get_option('default_library') == 'static'
    conf_data.set('SPATIALAUDIO_STATIC', 1)
endif

subdir('include')
subdir('source')

# Make the library usable as a Meson subproject.
libspatialaudio_dep = declare_dependency(
    include_directories : spatialaudio_incdirs,
    dependencies : dependencies,
    link_with : spatialaudio_lib,
)
meson.override_dependency('libspatialaudio', libspatialaudio_dep)

# Make this library properly installable
install_headers(spatialaudio_headers,
    subdir : 'spatialaudio',
    preserve_path : true,
)

pkg_mod = import('pkgconfig')
pkg_mod.generate(
    spatialaudio_lib,
    description : 'Spatial audio rendering library',
)

subdir('tests')

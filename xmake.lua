set_rules("mode.debug","mode.release")
set_languages("c23")
set_toolchains("gcc")
add_cflags("-Wall -m64 -std=c23 -O0")
if is_plat("linux") then
    add_defines("VK_USE_PLATFORM_XCB_KHR")
    add_ldflags("-ldl -lxcb -lxcb-icccm")
end
if is_plat("windows") then
    add_defines("VK_USE_PLATFORM_WIN32_KHR")
    add_ldflags("-lgdi32")
end
if is_mode("release") then
    set_targetdir("build/release/")
end
if is_mode("debug") then
    set_targetdir("build/debug/")
    add_defines("DEBUG")
    add_cflags("-g")
end
target("cgui-app")
    set_kind("binary")
    set_filename("cgui-app")
    if is_plat("windows") then
        set_filename("cgui-app.exe")
    end
    add_files("src/main/*.c")
    add_includedirs("src/main/")
    add_files("src/functions/*.c")
    add_includedirs("src/functions/")